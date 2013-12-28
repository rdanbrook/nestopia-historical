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
#include "NstMapper060.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER60::Reset()
{
	if (pRomCrc == 0xF9C484A0UL)
	{
		// Reset-triggered 4-in-1

		if (HardReset)
			game = 0;

		cRom.SwapBanks<n8k,0x0000>( game );
		pRom.SwapBanks<n16k,0x0000>( game );
		pRom.SwapBanks<n16k,0x4000>( game );

		game = (game + 1) & 0x3;
	}
	else
	{
		cpu.SetPort( 0x8000, 0x9FFF, this, Peek_8000, Poke_pRom );
		cpu.SetPort( 0xA000, 0xBFFF, this, Peek_A000, Poke_pRom );
		cpu.SetPort( 0xC000, 0xDFFF, this, Peek_C000, Poke_pRom );
		cpu.SetPort( 0xE000, 0xFFFF, this, Peek_E000, Poke_pRom );

		pRom.SwapBanks<n32k,0x0000>(0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER60,pRom) 
{
	apu.Update();
	ppu.SetMirroring( (data & 0x8) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL );

	if (address & 0x80)
	{
		pRom.SwapBanks<n16k,0x0000>( (address & 0x70) >> 4 );
		pRom.SwapBanks<n16k,0x4000>( (address & 0x70) >> 4 );
	}
	else
	{
		pRom.SwapBanks<n32k,0x0000>( (address & 0x70) >> 5 );
	}

	cRom.SwapBanks<n8k,0x0000>( address & 0x07 );
}

NES_NAMESPACE_END