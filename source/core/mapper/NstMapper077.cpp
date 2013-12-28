////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003-2006 Martin Freij
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

#include "../NstMapper.hpp"
#include "NstMapper077.hpp"
			 
namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif
	
		void Mapper77::SubReset(const bool hard)
		{
			Map( 0x6000U, 0xFFFFU, &Mapper77::Poke_Prg );

			chr.Source(1).SwapBank<SIZE_2K,0x0800U>( 0 );
			chr.Source(1).SwapBank<SIZE_2K,0x1000U>( 1 );
			chr.Source(1).SwapBank<SIZE_2K,0x1800U>( 2 );
		}
	
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif
	
		NES_POKE(Mapper77,Prg) 
		{
			ppu.Update();
			prg.SwapBank<SIZE_32K,0x0000U>(data & 0x7);
			chr.SwapBank<SIZE_2K,0x0000U>(data >> 4);
		}
	}
}
