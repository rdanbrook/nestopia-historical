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

#ifndef NST_MAPPER_85_H
#define NST_MAPPER_85_H

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
// Mapper 85
////////////////////////////////////////////////////////////////////////////////////////

class MAPPER85 : public MAPPER
{
public:

	MAPPER85(CONTEXT& c)
	: MAPPER(c,&IrqState,&counter+1) {}

protected:

	VOID Reset();
	VOID IrqSync(const UINT);

	NES_DECL_POKE( 8000 );
	NES_DECL_POKE( 8008 );
	NES_DECL_POKE( 9000 );
	NES_DECL_POKE( 9010 );
	NES_DECL_POKE( A000 );
	NES_DECL_POKE( A008 );
	NES_DECL_POKE( B000 );
	NES_DECL_POKE( B008 );
	NES_DECL_POKE( C000 );
	NES_DECL_POKE( C008 );
	NES_DECL_POKE( D000 );
	NES_DECL_POKE( D008 );
	NES_DECL_POKE( E000 );
	NES_DECL_POKE( E008 );
	NES_DECL_POKE( F000 );
	NES_DECL_POKE( F008 );

	UINT IrqState;
	UINT counter;
};

NES_NAMESPACE_END

#endif