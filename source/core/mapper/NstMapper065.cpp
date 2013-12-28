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
#include "NstMapper065.h"
		  
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER65::Reset()
{
	EnableIrqSync(IRQSYNC_COUNT);

	switch (pRomCrc)
	{
     	case 0xE30B7F64UL: // Kaiketsu Yanchamaru 3

			cpu.SetPort( 0x9001, this, Peek_pRom, Poke_9001   );
			cpu.SetPort( 0x9005, this, Peek_pRom, Poke_9005_1 );
			cpu.SetPort( 0x9006, this, Peek_pRom, Poke_9006_1 );
			break;

		default:

			cpu.SetPort( 0x9000, this, Peek_pRom, Poke_9000   );
			cpu.SetPort( 0x9003, this, Peek_pRom, Poke_9003   );
			cpu.SetPort( 0x9004, this, Peek_pRom, Poke_9004   );
			cpu.SetPort( 0x9005, this, Peek_pRom, Poke_9005_0 );
			cpu.SetPort( 0x9006, this, Peek_pRom, Poke_9006_0 );
			break;
	}

	cpu.SetPort( 0x8000, this, Peek_pRom, Poke_8000 );
	cpu.SetPort( 0xB000, this, Peek_pRom, Poke_B000 );
	cpu.SetPort( 0xB001, this, Peek_pRom, Poke_B001 );
	cpu.SetPort( 0xB002, this, Peek_pRom, Poke_B002 );
	cpu.SetPort( 0xB003, this, Peek_pRom, Poke_B003 );
	cpu.SetPort( 0xB004, this, Peek_pRom, Poke_B004 );
	cpu.SetPort( 0xB005, this, Peek_pRom, Poke_B005 );
	cpu.SetPort( 0xB006, this, Peek_pRom, Poke_B006 );
	cpu.SetPort( 0xB007, this, Peek_pRom, Poke_B007 );
	cpu.SetPort( 0xA000, this, Peek_pRom, Poke_A000 );
	cpu.SetPort( 0xC000, this, Peek_pRom, Poke_C000 );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,8000) 
{ 
	apu.Update(); 
	pRom.SwapBanks<n8k,0x0000>(data); 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,9000) 
{ 
	ppu.SetMirroring( (data & 0x40) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL ); 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,9001) 
{ 
	ppu.SetMirroring( (data & 0x80) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL ); 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,9003) 
{ 
	SetIrqEnable( data & 0x80 );
	cpu.ClearIRQ();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,9004) 
{ 
	IrqCount = IrqLatch;  
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,9005_0) 
{
	IrqLatch = (IrqLatch & 0x00FF) | (data << 8);
}

NES_POKE(MAPPER65,9005_1) 
{
	SetIrqEnable(data);
	cpu.ClearIRQ();
	IrqCount = (data << 1) & 0xFF;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,9006_0) 
{
	IrqLatch = (IrqLatch & 0xFF00) | data;
}

NES_POKE(MAPPER65,9006_1) 
{
	SetIrqEnable(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,B000) { ppu.Update(); cRom.SwapBanks<n1k,0x0000>(data); }
NES_POKE(MAPPER65,B001) { ppu.Update(); cRom.SwapBanks<n1k,0x0400>(data); }
NES_POKE(MAPPER65,B002) { ppu.Update(); cRom.SwapBanks<n1k,0x0800>(data); }
NES_POKE(MAPPER65,B003) { ppu.Update(); cRom.SwapBanks<n1k,0x0C00>(data); }
NES_POKE(MAPPER65,B004) { ppu.Update(); cRom.SwapBanks<n1k,0x1000>(data); }
NES_POKE(MAPPER65,B005) { ppu.Update(); cRom.SwapBanks<n1k,0x1400>(data); }
NES_POKE(MAPPER65,B006) { ppu.Update(); cRom.SwapBanks<n1k,0x1800>(data); }
NES_POKE(MAPPER65,B007) { ppu.Update(); cRom.SwapBanks<n1k,0x1C00>(data); }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER65,A000) { apu.Update(); pRom.SwapBanks<n8k,0x2000>(data); }
NES_POKE(MAPPER65,C000) { apu.Update(); pRom.SwapBanks<n8k,0x4000>(data); }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER65::IrqSync(const UINT delta)
{
	if ((IrqCount -= delta) <= -4)
	{
		SetIrqEnable(FALSE);
		cpu.DoIRQ();
	}
}

NES_NAMESPACE_END