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
#include "NstMapper021.h"
#include "NstMapper025.h"
			  
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER25::Reset()
{
	MAPPER21::Reset();

	MAPPER21* const m21 = PDX_STATIC_CAST(MAPPER21*,this);

	cpu.SetPort( 0x8000, 0x8FFF, this, Peek_8000, Poke_8000 );
	cpu.SetPort( 0xA000, 0xAFFF, m21, MAPPER21::Peek_A000, MAPPER21::Poke_A000 );

	for (ULONG i=0x9000; i <= 0xFFFF; ++i)
	{
		switch (i & 0xF00F)
		{
	     	case 0x9000: cpu.SetPort( i, m21, MAPPER21::Peek_9000, MAPPER21::Poke_9000 ); continue;
			case 0xB000: cpu.SetPort( i, m21, MAPPER21::Peek_B000, MAPPER21::Poke_B000 ); continue;
			case 0xC000: cpu.SetPort( i, m21, MAPPER21::Peek_C000, MAPPER21::Poke_C000 ); continue;
			case 0xD000: cpu.SetPort( i, m21, MAPPER21::Peek_D000, MAPPER21::Poke_D000 ); continue;
			case 0xE000: cpu.SetPort( i, m21, MAPPER21::Peek_E000, MAPPER21::Poke_E000 ); continue;
			case 0xF000: cpu.SetPort( i, m21, MAPPER21::Peek_F000, MAPPER21::Poke_F000 ); continue;

			case 0x9001: case 0x9004: cpu.SetPort( i, this, Peek_9000, Poke_9004 ); continue;
			case 0xB001: case 0xB004: cpu.SetPort( i, m21, MAPPER21::Peek_B000, MAPPER21::Poke_B004 ); continue;
			case 0xB002: case 0xB008: cpu.SetPort( i, m21, MAPPER21::Peek_B000, MAPPER21::Poke_B002 ); continue;
			case 0xB003: case 0xB00C: cpu.SetPort( i, m21, MAPPER21::Peek_B000, MAPPER21::Poke_B006 ); continue;
			case 0xC001: case 0xC004: cpu.SetPort( i, m21, MAPPER21::Peek_C000, MAPPER21::Poke_C004 ); continue;
			case 0xC002: case 0xC008: cpu.SetPort( i, m21, MAPPER21::Peek_C000, MAPPER21::Poke_C002 ); continue;
			case 0xC003: case 0xC00C: cpu.SetPort( i, m21, MAPPER21::Peek_C000, MAPPER21::Poke_C006 ); continue;
			case 0xD001: case 0xD004: cpu.SetPort( i, m21, MAPPER21::Peek_D000, MAPPER21::Poke_D004 ); continue;
			case 0xD002: case 0xD008: cpu.SetPort( i, m21, MAPPER21::Peek_D000, MAPPER21::Poke_D002 ); continue;
			case 0xD003: case 0xD00C: cpu.SetPort( i, m21, MAPPER21::Peek_D000, MAPPER21::Poke_D006 ); continue;
			case 0xE001: case 0xE004: cpu.SetPort( i, m21, MAPPER21::Peek_E000, MAPPER21::Poke_E004 ); continue;
			case 0xE002: case 0xE008: cpu.SetPort( i, m21, MAPPER21::Peek_E000, MAPPER21::Poke_E002 ); continue;
			case 0xE003: case 0xE00C: cpu.SetPort( i, m21, MAPPER21::Peek_E000, MAPPER21::Poke_E006 ); continue;
			case 0xF001: case 0xF004: cpu.SetPort( i, m21, MAPPER21::Peek_F000, MAPPER21::Poke_F004 ); continue;
			case 0xF002: case 0xF008: cpu.SetPort( i, m21, MAPPER21::Peek_F000, MAPPER21::Poke_F002 ); continue;
			case 0xF003: case 0xF00C: cpu.SetPort( i, m21, MAPPER21::Peek_F000, MAPPER21::Poke_F003 ); continue;
		}																		
	}

	pRomSwap = 0;

	pRomSelect[0] = 0;
	pRomSelect[1] = pRom.NumBanks<n8k>() - 2;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER25,8000)
{
	apu.Update(); 

	if (pRomSwap)
	{
		pRomSelect[1] = data;
		pRom.SwapBanks<n8k,0x4000>( data ); 
	}
	else
	{
		pRomSelect[0] = data;
		pRom.SwapBanks<n8k,0x0000>( data ); 
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER25,9004) 
{ 
	apu.Update(); 

	const UINT swap = data & 0x2;

	if (pRomSwap != swap)
	{
		PDX::Swap( pRomSelect[0], pRomSelect[1] );
		pRom.SwapBanks<n8k,0x0000>( pRomSelect[0] ); 
		pRom.SwapBanks<n8k,0x4000>( pRomSelect[1] ); 
	}

	pRomSwap = swap;
}

NES_NAMESPACE_END