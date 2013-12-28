////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
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

#ifndef NST_BOARDS_TAIWANMMC3_H
#define NST_BOARDS_TAIWANMMC3_H

#ifdef NST_PRAGMA_ONCE_SUPPORT
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		namespace Boards
		{
			class TaiwanMmc3 : public Boards::Mmc3
			{
			protected:

				TaiwanMmc3(Context&,uint=0);

				void SubReset(bool);   
				void UpdateChr() const;

			private:

				void BaseSave(State::Saver&) const;
				void BaseLoad(State::Loader&,dword);
				void SwapChr(uint,uint) const;
				
				virtual uint GetChrSource(uint) const = 0;

				NES_DECL_PEEK( Ram  )
				NES_DECL_POKE( Ram  )
				NES_DECL_POKE( A000 )

				u8 ram[0x1000];
			};
		}
	}
}

#endif