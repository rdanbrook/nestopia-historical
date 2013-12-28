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
#include "NstMapper051.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER51::Reset()
{
	if (cRom.Size())
		EnableCartridgeCRam();

	cpu.SetPort( 0x6000, 0x7FFF, this, Peek_6000, Poke_6000 );
	cpu.SetPort( 0x8000, 0x9FFF, this, Peek_8000, Poke_8000 );
	cpu.SetPort( 0xA000, 0xBFFF, this, Peek_A000, Poke_8000 );
	cpu.SetPort( 0xC000, 0xDFFF, this, Peek_C000, Poke_C000 );
	cpu.SetPort( 0xE000, 0xFFFF, this, Peek_E000, Poke_8000 );

	bank = 0;
	mode = 1;

	UpdateBanks();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(MAPPER51,6000)
{
	return *pRom.Ram(offset + address - 0x6000);
}

NES_POKE(MAPPER51,6000) 
{
	mode = 
	(
     	((data & 0x10) >> 3) | 
		((data & 0x02) >> 1)
	);

	UpdateBanks();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER51,8000) 
{
	bank = (data & 0x0F) << 2;
	UpdateBanks();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER51,C000)
{
	bank = (data & 0x0F) << 2;
	mode = (mode & 0x01) | ((data & 0x10) >> 3);
	UpdateBanks();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER51::UpdateBanks()
{
	apu.Update();
	ppu.SetMirroring( (mode < 3) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL );

	switch (mode)
	{
		case 1:
		case 3:

			pRom.SwapBanks<n32k,0x0000>( (bank >> 2) );
			offset = bank | 0x23;
			break;

		default:

			pRom.SwapBanks<n16k,0x0000>( (bank >> 1) | (mode >> 1) );
			pRom.SwapBanks<n16k,0x4000>( (bank >> 1) | 0x7 );
			offset = bank | 0x2F;
			break;
	}

	offset *= n8k;
}

NES_NAMESPACE_END