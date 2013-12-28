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
#include "NstMapper073.h"
	  
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER73::Reset()
{
	EnableIrqSync(IRQSYNC_COUNT);

	cpu.SetPort( 0x8000, 0x8FFF, this, Peek_8000, Poke_8000 );
	cpu.SetPort( 0x9000, 0x9FFF, this, Peek_9000, Poke_9000 );
	cpu.SetPort( 0xA000, 0xAFFF, this, Peek_A000, Poke_A000 );
	cpu.SetPort( 0xB000, 0xBFFF, this, Peek_B000, Poke_B000 );
	cpu.SetPort( 0xC000, 0xCFFF, this, Peek_C000, Poke_C000 );
	cpu.SetPort( 0xF000, 0xFFFF, this, Peek_F000, Poke_F000 );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER73,8000) { IrqCount = (IrqCount & 0xFFF0) | ((data & 0xF) <<  0); }
NES_POKE(MAPPER73,9000) { IrqCount = (IrqCount & 0xFF0F) | ((data & 0xF) <<  4); }
NES_POKE(MAPPER73,A000) { IrqCount = (IrqCount & 0xF0FF) | ((data & 0xF) <<  8); }
NES_POKE(MAPPER73,B000) { IrqCount = (IrqCount & 0x0FFF) | ((data & 0xF) << 12); }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER73,C000)
{
	SetIrqEnable(data & 0x2);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER73,F000)
{
	apu.Update(); 
	pRom.SwapBanks<n16k,0x0000>(data);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER73::IrqSync(const UINT delta)
{
	IrqCount += delta;

	if (IrqCount >= 0xFFFFUL + 0xFF)
	{
		IrqCount = 0xFFFFU;
		SetIrqEnable(FALSE);
	}
	else if (IrqCount >= 0xFFFFUL)
	{
		IrqCount = 0xFFFFUL + 0xFF;
		cpu.DoIRQ();
	}
}

NES_NAMESPACE_END
