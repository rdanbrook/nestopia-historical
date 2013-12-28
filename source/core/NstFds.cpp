////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003 Martin Freij
//
// This file is part of Nestopia.
// 
// Nestopia is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// Nestopia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Nestopia; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////////////

#include "../paradox/PdxFile.h"
#include "../paradox/PdxCrc32.h"
#include "NstTypes.h"
#include "NstMap.h"
#include "NstCpu.h"
#include "NstPpu.h"
#include "NstFds.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL FDS::WriteProtect = FALSE;
BOOL FDS::BiosLoaded = FALSE;
U8   FDS::bRom[n8k];

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

FDS::FDS(CPU& c,PPU& p)
: 
sound  (c),
cpu    (c), 
ppu    (p),
offset (0),
last   (0)
{}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

FDS::~FDS()
{
	WriteDiskFile();
	cpu.RemoveEvent( this );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT FDS::Load(PDXFILE& file)
{
	Unload();

	ImageFileName = file.Name();

	if (!BiosLoaded)
		return MsgError("Missing FDS BIOS file!");

	if (!file.Read(header))
		return MsgError("Not a valid disk format!");

	if (header.magic != 0x1A534446UL)
		return MsgError("Not a valid disk format!");

	disks.Resize( header.NumDisksAndSides );
	
	for (UINT i=0; i < header.NumDisksAndSides; ++i)
	{
		if (!file.Read( disks[i].Begin(), disks[i].End() ))
			return MsgError("corrupt data!");
	}

	offset = 0;
	last   = 0;

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT FDS::GetContext(IO::FDS::CONTEXT& context) const
{
	context.NumDisks       = disks.Size() / 2;
	context.DiskInserted   = offset == DISK_EJECTED ? FALSE : TRUE;
	context.CurrentDisk    = offset == DISK_EJECTED ? 0 : offset / 2;
	context.CurrentSide    = offset == DISK_EJECTED ? 0 : offset % 2;
	context.WriteProtected = WriteProtect;
	context.bios           = NULL;

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT FDS::SetContext(const IO::FDS::CONTEXT& context)
{
	PDX_ASSERT( context.CurrentSide < 2 && context.CurrentDisk < disks.Size() );

	WriteProtect = context.WriteProtected;

	if (context.bios)
		SetBIOS( *context.bios );

	const UINT side = 
	(
	    context.DiskInserted ? 
     	(context.CurrentDisk * 2) + context.CurrentSide : 
       	DISK_EJECTED
	);

	if (offset != side)
	{
		InsertWait = 0;
		offset = side;

		PDXSTRING string("Disk ");
		string += (context.CurrentDisk+1);

		if (offset == DISK_EJECTED)
		{
			string += " ejected..";
		}
		else
		{
			string += ", Side ";
			string += (context.CurrentSide ? "B inserted.." : "A inserted..");
		}

		MsgOutput( string.String() );
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT FDS::SetBIOS(PDXFILE& file)
{
	if (file.IsOpen())
	{
		if (file.Size() < sizeof(U8) * n8k)
			return MsgError("BIOS file is corrupt!");

		if (file.Peek<U32>() == 0x1A53454EUL)
		{
			file.Seek( PDXFILE::BEGIN, sizeof(U32) );

			const UINT NumBanks = file.Read<U8>();

			if (!NumBanks)
				return MsgError("iNes BIOS file is corrupt!");

			UINT offset;

			offset	= 16; // header
			offset += (NumBanks * n16k) - n8k; // what we're interested in is in the last 8k bank
			offset += (file.Read<U8>() & 0x4) ? 0x200 : 0x000; // trainer

			if (offset + n8k > file.Size() * sizeof(U8))
				return MsgError("iNes BIOS file is corrupt!");

			file.Seek( PDXFILE::BEGIN, sizeof(U8) * offset );
		}

		if (file.Readable( sizeof(U8) * n8k ))
		{
           #ifdef NES_WARN_ABOUT_BIOS

			switch (PDXCRC32::Compute( file.At(file.Position()), n8k ))
			{
     			case 0x5E607DCFUL:
				case 0x4DF24A6CUL: // twinsys
					break;

				default:

					MsgWarning("The BIOS file was not recognized and may not work properly!");
			}

           #endif

			BiosLoaded = TRUE;
			file.Read( bRom, bRom + n8k );
			
			return PDX_OK;
		}
		else
		{
			return MsgError("BIOS file is corrupt!");
		}
	}

	return PDX_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

const U8* FDS::GetBIOS()
{
	return bRom;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FDS::Unload()
{
	WriteDiskFile();
	ImageFileName.Clear();
	disks.Clear();
	offset = DISK_EJECTED;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FDS::WriteDiskFile()
{
	if (!WriteProtect && ImageFileName.Size() && disks.Size())
	{
		PDXFILE file( ImageFileName, PDXFILE::APPEND );

		static const CHAR* msg = 
   		(
       		"The loaded FDS image file couldn't be located, probably as a result "
     		"of opening it from a compressed archive. Do you want to keep the "
     		"the current changes and write it back to a new file outside the archive?"
		);

		if (!file.Size() && !MsgQuestion( "Saving back FDS image to hard drive", msg ))
			file.Close();

		if (file.IsOpen())
		{
			if (file.IsEmpty())
			{
				file.Write( header );
			}
			else
			{
				file.Seek( PDXFILE::BEGIN, sizeof(HEADER) );
			}

			for (UINT i=0; i < disks.Size(); ++i)
				file.Write( disks[i].Begin(), disks[i].End() );

			LogOutput("FDS: the disk data was saved back to the file");
		}
		else
		{
			LogOutput("FDS: Error, the disk data couldn't be saved back to the file");
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FDS::Reset(const BOOL)
{
	LogReset();

	IrqCycles   = LONG_MAX;
	IrqLatch.d  = 0;
	IrqOnce     = 0;
	IrqWait     = LONG_MAX;
	ctrl        = 0;
	pos         = 0;
	DiskEnabled = 0;
	WriteSkip   = 0;
	InsertWait  = 180;

	cpu.RemoveEvent( this );
	cpu.SetEvent( this, IrqSync );
	cpu.SetLine(  CPU::IRQ_EXT_1 | CPU::IRQ_EXT_2 );
	cpu.ClearIRQ( CPU::IRQ_EXT_1 | CPU::IRQ_EXT_2 ); 

	cpu.SetPort( 0x4020,         this, Peek_Nop,  Poke_4020 );
	cpu.SetPort( 0x4021,         this, Peek_Nop,  Poke_4021 );
	cpu.SetPort( 0x4022,         this, Peek_Nop,  Poke_4022 );
	cpu.SetPort( 0x4023,         this, Peek_Nop,  Poke_4023 );
	cpu.SetPort( 0x4024,         this, Peek_Nop,  Poke_4024 );
	cpu.SetPort( 0x4025,         this, Peek_Nop,  Poke_4025 );
	cpu.SetPort( 0x4030,         this, Peek_4030, Poke_Nop  );
	cpu.SetPort( 0x4031,         this, Peek_4031, Poke_Nop  );
	cpu.SetPort( 0x4032,         this, Peek_4032, Poke_Nop  );
	cpu.SetPort( 0x4033,         this, Peek_4033, Poke_Nop  );
	cpu.SetPort( 0x6000, 0xDFFF, this, Peek_wRam, Poke_wRam );
	cpu.SetPort( 0xE000, 0xFFFF, this, Peek_bRom, Poke_Nop  );

	sound.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FDS::LogReset()
{
	PDXSTRING log("FDS: ");
	LogOutput( log << "reset" );
	log.Resize( 5 );
	LogOutput( log << (disks.Size() / 2) << " disk(s) present" );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT FDS::SaveState(PDXFILE& file)
{
	file.Write( wRam, wRam + n32k );

	// adjusted for compatibility

	const BOOL IrqWaitEn = (IrqWait != LONG_MAX);
	const BOOL IrqEnabled = (IrqCycles != LONG_MAX);

	LONG IrqCount = 0;

	if (IrqEnabled)
	{
		IrqCount = (IrqCycles - cpu.GetCycles<CPU::CYCLE_MASTER>());
		IrqCount = (cpu.IsPAL() ? NES_PAL_TO_CPU(IrqCount) : NES_NTSC_TO_CPU(IrqCount));
	}

	file << U16( offset              );
	file << U16( last                );
	file <<  U8( ctrl                );
	file << U16( pos                 );
	file <<  U8( DiskEnabled ? 1 : 0 );
	file <<  U8( WriteSkip           );
	file << U16( InsertWait          );
	file << U16( IrqLatch.d          );
	file <<  U8( IrqEnabled ? 1 : 0  );
	file <<  U8( IrqOnce ? 1 : 0     );
	file << I32( IrqCount            );
	file << I32( IrqWaitEn ? 1 : 0   );
	file << I32( 0                   );

	return sound.SaveState( file );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT FDS::LoadState(PDXFILE& file)
{
	if (!file.Readable( (sizeof(U8) * (n32k + 27)) ))
		return PDX_FAILURE;

	file.Read( wRam, wRam + n32k );

	// adjusted for compatibility

	BOOL IrqEnabled;

	offset      = file.Read<U16> ();
	last        = file.Read<U16> ();
	ctrl        = file.Read<U8>  ();
	pos         = file.Read<U16> ();
	DiskEnabled = file.Read<U8>  ();
	WriteSkip   = file.Read<U8>  ();
	InsertWait  = file.Read<U16> ();
	IrqLatch.d  = file.Read<U16> ();
	IrqEnabled  = file.Read<U8>  ();
	IrqOnce     = file.Read<U8>  ();	
	IrqCycles   = file.Read<I32> ();
	
	if (IrqEnabled)
	{
		IrqCycles = cpu.GetCycles<CPU::CYCLE_MASTER>() + 
		(
			cpu.IsPAL() ? NES_CPU_TO_PAL(IrqCycles) :
                     	  NES_CPU_TO_NTSC(IrqCycles)
		);
	}
	else
	{
		IrqCycles = LONG_MAX;
	}
	
	IrqWait = file.Read<I32>();
	
	if (IrqWait)
	{
		IrqWait = cpu.GetCycles<CPU::CYCLE_MASTER>() + 
		(
			cpu.IsPAL() ? NES_CPU_TO_PAL(IRQ_WAIT_LONG) :
                		  NES_CPU_TO_NTSC(IRQ_WAIT_LONG)
		);
	}
	else
	{
		IrqWait = LONG_MAX;
	}

	file.Read<I32>();

	return sound.LoadState( file );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(FDS,Nop)  { return cpu.GetCache();         }
NES_POKE(FDS,Nop)  {                                }
NES_PEEK(FDS,wRam) { return wRam[address - 0x6000]; }
NES_POKE(FDS,wRam) { wRam[address - 0x6000] = data; }
NES_PEEK(FDS,bRom) { return bRom[address - 0xE000]; }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(FDS,4020) 
{ 
	cpu.ClearIRQ(CPU::IRQ_EXT_1); 
	IrqLatch.b.l = data; 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(FDS,4021) 
{ 
	cpu.ClearIRQ(CPU::IRQ_EXT_1); 
	IrqLatch.b.h = data; 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(FDS,4022) 
{ 
	cpu.ClearIRQ(CPU::IRQ_EXT_1); 

	IrqOnce = !(data & IRQ_ALWAYS);

	if (data & IRQ_ENABLE)
	{
		IrqCycles = cpu.GetCycles<CPU::CYCLE_MASTER>() + 
		(
	       	cpu.IsPAL() ? NES_CPU_TO_PAL(IrqLatch.d+1) : 
		                  NES_CPU_TO_NTSC(IrqLatch.d+1)
		);
	}
	else
	{
		IrqCycles = LONG_MAX;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(FDS,4023) 
{
	DiskEnabled = data & DISK_IO_ENABLED;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(FDS,4024) 
{ 
	if (DiskEnabled && !(ctrl & CTRL_READ_MODE) && pos < 65000U && offset != DISK_EJECTED && offset == last)
	{
		if (WriteSkip)
		{
			--WriteSkip;
		}
		else if (pos >= 2)
		{
			disks[offset][pos-2] = data;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(FDS,4025) 
{
	cpu.ClearIRQ(CPU::IRQ_EXT_2);
	
	if (offset != DISK_EJECTED && offset == last)
	{
		if ((ctrl & CTRL_DRIVE_READY) && !(data & CTRL_CRC) && !(data & CTRL_DRIVE_READY))
		{
			IrqWait = cpu.GetCycles<CPU::CYCLE_MASTER>() + 
			(
				cpu.IsPAL() ? NES_CPU_TO_PAL(IRQ_WAIT_LONG) :
                  			  NES_CPU_TO_NTSC(IRQ_WAIT_LONG)
			);

			pos = (pos >= 2) ? pos-2 : 0;
		}

		if (!(data & CTRL_READ_MODE))
			WriteSkip = 2;

		if (data & (CTRL_DRIVE_READY|CTRL_TRANSFER_RESET))
		{
			IrqWait = cpu.GetCycles<CPU::CYCLE_MASTER>() + 
			(
				cpu.IsPAL() ? NES_CPU_TO_PAL(IRQ_WAIT_LONG) :
                  			  NES_CPU_TO_NTSC(IRQ_WAIT_LONG)
			);

			if (data & CTRL_TRANSFER_RESET)
				pos = 0;
		}
	}

	ppu.SetMirroring( (data & CTRL_MIRRORING_HORIZONTAL) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL );

	ctrl = data;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(FDS,4030) 
{ 
	UINT status = 0x00;

	if (cpu.IsIRQ(CPU::IRQ_EXT_1)) status |= IO_STATUS_IRQ_1;
	if (cpu.IsIRQ(CPU::IRQ_EXT_2)) status |= IO_STATUS_IRQ_2;

	cpu.ClearIRQ(CPU::IRQ_EXT_1 | CPU::IRQ_EXT_2);

	return status; 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(FDS,4031) 
{
	cpu.ClearIRQ(CPU::IRQ_EXT_2);

	if (offset != DISK_EJECTED && offset == last)
	{
		const UINT data = disks[offset][pos];

		if (ctrl & CTRL_MOTOR)
		{
			IrqWait = cpu.GetCycles<CPU::CYCLE_MASTER>() + 
			(
     			cpu.IsPAL() ? NES_CPU_TO_PAL(IRQ_WAIT_SHORT) :
			                  NES_CPU_TO_NTSC(IRQ_WAIT_SHORT)
			);

			pos = PDX_MIN(pos+1,65000U);
		}

		return data;
	}

	return cpu.GetCache();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(FDS,4032) 
{ 
	UINT status = cpu.GetCache() & ~0x7;

	const BOOL eject = (offset == DISK_EJECTED || offset != last);

	if (eject)
		status |= 0x5;

	if (eject || !(ctrl & CTRL_MOTOR) || (ctrl & CTRL_TRANSFER_RESET))
       	status |= STATUS_DRIVE_NOT_READY;

	if (offset != last && InsertWait > 120)
		last = offset;

	return status;  
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(FDS,4033) 
{ 
	return BATTERY_CHARGED; 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FDS::IrqSync()
{
	const ULONG CpuCycles = cpu.GetCycles<CPU::CYCLE_MASTER>();

	if (IrqCycles <= CpuCycles)
	{
		if (IrqOnce)
			IrqCycles = LONG_MAX;
		else
			IrqCycles = CpuCycles + (cpu.IsPAL() ? NES_CPU_TO_PAL(IrqLatch.d+1) : NES_CPU_TO_NTSC(IrqLatch.d+1));

		cpu.DoIRQ(CPU::IRQ_EXT_1);
	}

	if (IrqWait <= CpuCycles)
	{
		IrqWait = LONG_MAX;

		if (ctrl & CTRL_DISK_IRQ_ENABLED)
			cpu.DoIRQ(CPU::IRQ_EXT_2);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FDS::VSync()
{
	if (IrqCycles != LONG_MAX)
		IrqCycles -= cpu.GetFrameCycles<CPU::CYCLE_MASTER>();

	if (IrqWait != LONG_MAX)
		IrqWait -= cpu.GetFrameCycles<CPU::CYCLE_MASTER>();

	if (InsertWait < 180)
		++InsertWait;
}

NES_NAMESPACE_END