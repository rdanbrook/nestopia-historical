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
#include "NstMapper046.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER46::Reset()
{
	regs[0] = regs[1] = 0x00;

	cpu.SetPort( 0x6000, 0x7FFF, this, Peek_Nop,  Poke_6000 );
	cpu.SetPort( 0x8000, 0x9FFF, this, Peek_8000, Poke_8000 );
	cpu.SetPort( 0xA000, 0xBFFF, this, Peek_A000, Poke_8000 );
	cpu.SetPort( 0xC000, 0xDFFF, this, Peek_C000, Poke_8000 );
	cpu.SetPort( 0xE000, 0xFFFF, this, Peek_E000, Poke_8000 );

	pRom.SwapBanks<n32k,0x0000>(0);
	ppu.SetMirroring( MIRROR_VERTICAL );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER46,6000) 
{
	ppu.Update();
	apu.Update();

	regs[0] = data;
	pRom.SwapBanks<n32k,0x0000>( ((regs[1] >> 0) & 0x1) + ((data & 0x0F) << 1) );
	cRom.SwapBanks<n8k,0x0000> ( ((regs[1] >> 4) & 0x7) + ((data & 0xF0) >> 1) );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER46,8000) 
{
	ppu.Update();
	apu.Update();

	regs[1] = data;
	pRom.SwapBanks<n32k,0x0000>( ((data >> 0) & 0x1) + ((regs[0] & 0x0F) << 1) );
	cRom.SwapBanks<n8k,0x0000> ( ((data >> 4) & 0x7) + ((regs[0] & 0xF0) >> 1) );
}

NES_NAMESPACE_END