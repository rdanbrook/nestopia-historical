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

#pragma once

#ifndef NST_MAPPER_68_H
#define NST_MAPPER_68_H

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
// mapper 68
////////////////////////////////////////////////////////////////////////////////////////

class MAPPER68 : public MAPPER
{
public:

	MAPPER68(CONTEXT&);

	PDXRESULT LoadState(PDXFILE&);
	PDXRESULT SaveState(PDXFILE&) const;

private:

	VOID Reset();

	NES_DECL_POKE( 8000 );
	NES_DECL_POKE( 9000 );
	NES_DECL_POKE( A000 );
	NES_DECL_POKE( B000 );
	NES_DECL_POKE( C000 );
	NES_DECL_POKE( D000 );
	NES_DECL_POKE( E000 );
	NES_DECL_POKE( F000 );

	NES_DECL_PEEK( CiRam );
	NES_DECL_POKE( CiRam );

	VOID UpdateMirroring();

	enum
	{
		SELECT_CIROM     = b00010000,
		SELECT_MIRRORING = b00000011,
		BANK_OFFSET      = b10000000
	};


	typedef CHIP<n4k,4> CIROM;
	typedef CHIP<n4k,4> CIRAM;

	CIRAM CiRam;
	CIROM CiRom;

	UINT status;
	UINT CiRomBanks[2];
};

NES_NAMESPACE_END

#endif