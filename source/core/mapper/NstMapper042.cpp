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

#include "NstMappers.h"
#include "NstMapper042.h"
		
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

MAPPER42::MAPPER42(CONTEXT& c)
:
MAPPER (c),
ExRom  (NULL)
{}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

MAPPER42::~MAPPER42()
{
	delete ExRom;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MAPPER42::LoadState(PDXFILE& file)
{
	PDX_TRY(MAPPER::LoadState(file));
	return ExRom->LoadState( file );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MAPPER42::SaveState(PDXFILE& file) const
{
	PDX_TRY(MAPPER::SaveState(file));
	return ExRom->SaveState( file );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER42::Reset()
{
	EnableIrqSync(IRQSYNC_COUNT);

	delete ExRom;
	ExRom = new EXROM( pRom.Ram(), pRom.Size() );

	cpu.SetPort( 0x6000, 0x7FFF, this, Peek_ExRom, Poke_Nop );

	for (ULONG i=0x8000; i <= 0xFFFF; ++i)
	{
		switch (i & 0xE003)
		{
    		case 0x8000: cpu.SetPort( i, this, Peek_8000, Poke_8000 ); continue;
     		case 0xE000: cpu.SetPort( i, this, Peek_E000, Poke_E000 ); continue;
     		case 0xE001: cpu.SetPort( i, this, Peek_E000, Poke_E001 ); continue;
			case 0xE002: cpu.SetPort( i, this, Peek_E000, Poke_E002 ); continue;
		}
	}

	pRom.SwapBanks<n8k,0x0000>( pRom.NumBanks<n8k>() - 4 );
	pRom.SwapBanks<n8k,0x2000>( pRom.NumBanks<n8k>() - 3 );
	pRom.SwapBanks<n8k,0x4000>( pRom.NumBanks<n8k>() - 2 );
	pRom.SwapBanks<n8k,0x6000>( pRom.NumBanks<n8k>() - 1 );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER42,8000) 
{
	ppu.Update();
	cRom.SwapBanks<n8k,0x0000>(data & 0x1F);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER42,E000) 
{
	ExRom->SwapBanks<n8k,0x0000>(data & 0xF);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER42,E001) 
{ 
	ppu.SetMirroring( (data & 0x8) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER42,E002) 
{ 
	cpu.ClearIRQ();
	SetIrqEnable(data & 0x2);

	if (!(data & 0x2))
		IrqCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(MAPPER42,ExRom)
{
	return (*ExRom)[address - 0x6000];
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER42::IrqSync(const UINT delta)
{
	IrqCount += delta;

	if (IrqCount >= 24590)
	{
		SetIrqEnable(FALSE);
	}
	else if (IrqCount >= 24576)
	{
		cpu.DoIRQ();
	}
}

NES_NAMESPACE_END

