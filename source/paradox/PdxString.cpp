//////////////////////////////////////////////////////////////////////////////////////////////
//
// Paradox Library - general purpose C++ utilities
//
// Copyright (C) 2003 Martin Freij
//
// This file is part of Paradox Library.
// 
// Paradox Library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// Paradox Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Paradox Library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "PdxString.h"

CHAR PDXSTRING::nill = '\0';
const PDXSTRING::CONVERTER PDXSTRING::converter;

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::InsertBack(const CHAR c)
{
	buffer.Grow( buffer.Size() ? 1 : 2 );

	buffer[buffer.Size()-2]	= c;
	buffer[buffer.Size()-1] = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::RemoveSpaces()
{
	if (buffer.Size() > 1)
	{
		TSIZE begin;

		for (begin=0; buffer[begin] == ' '; ++begin);

		TSIZE end = buffer.Size() - 1;

		if (begin < end)
		{
			while (buffer[end-1] == ' ')
				--end;

			if (begin)
			{
				end -= begin;
				memmove( buffer.Begin(), buffer.At(begin), end );
			}
		}
		else
		{
			end = 0;
		}

		buffer.Resize( end + 1 );
		buffer.Back() = '\0';
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::RemoveQuotes()
{
	if (buffer.Size() > 1)
	{
		if (buffer.Front() == '\"')
			buffer.EraseFront();

		if (buffer.Size() > 1 && buffer[buffer.Size() - 2] == '\"')
		{
			buffer.EraseBack();
			buffer.Back() = '\0';
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

ULONG PDXSTRING::ToUlong() const
{
	if (buffer.Size() < 2)
		return 0;

	return strtoul( buffer.Begin(), NULL, 10 );
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::Append(const UINT value,const UINT radix)
{
	if (value)
	{
		TSIZE pos = buffer.Size() ? buffer.Size() - 1: 0;

		buffer.Grow( PDX_SIZE_BITS(UINT) );
		buffer[pos] = '\0';

		sprintf( buffer.At(pos), radix == HEX ? "%#X" : "%u", value );

		if (radix == HEX)
			buffer[pos+1] = 'x';

		while (buffer[pos++] != '\0');

		buffer.Resize(pos);

		return *this;
	}

	return Append( radix == HEX ? "0x0" : "0" );
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::Append(const INT value,const UINT radix)
{
	if (radix == HEX)
		return Append( UINT(value), HEX );

	TSIZE pos = buffer.Size() ? buffer.Size() - 1: 0;

	buffer.Grow( PDX_SIZE_BITS(INT) );
	buffer[pos] = '\0';

	sprintf( buffer.At(pos), "%d", value );

	while (buffer[pos++] != '\0');

	buffer.Resize(pos);

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::Append(const FLOAT value,const UINT radix)
{
	TSIZE pos = buffer.Size() ? buffer.Size() - 1: 0;

	buffer.Grow( PDX_SIZE_BITS(DOUBLE) );
	buffer[pos] = '\0';

	sprintf( buffer.At(pos), radix == DIG ? "%f" : "%d", value );

	while (buffer[pos++] != '\0');

	buffer.Resize(pos);

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Copy
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Set(CONSTITERATOR begin,CONSTITERATOR end)
{
	PDX_ASSERT(begin && end);
	buffer.Resize((end-begin)+1);
	memcpy(buffer.Begin(),begin,buffer.Size()-1);
	buffer.Back() = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Append
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::Append(const PDXSTRING& string)
{
	if (string.Size())
	{
		if (buffer.Size())
			buffer.EraseBack();

		buffer += string.buffer;
		PDX_ASSERT(buffer.Back() == '\0');
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Append
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::Append(CONSTITERATOR begin,CONSTITERATOR end)
{
	PDX_ASSERT(begin && end);

	const TSIZE pos = buffer.Size() ? buffer.Size() - 1 : 0;
	const TSIZE length = end - begin;

	buffer.Resize(pos + length + 1);
	memcpy(buffer.At(pos),begin,length);
	buffer.Back() = '\0';

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Validate()
{
	for (TSIZE i=0; i < buffer.Size(); ++i)
	{
		if (buffer[i] == '\0')
		{
			buffer.Resize(i+1);
			buffer.Defrag();
			return;
		}
	}

	buffer.InsertBack('\0');
	buffer.Defrag();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Resize(const TSIZE length,const CHAR FillCharacter)
{
	const TSIZE pos = Length();
	buffer.Resize( length + 1 );

	if (length > pos)
		memset( buffer.At(pos), FillCharacter, length - pos );

	buffer.Back() = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::EraseBack()
{
	PDX_ASSERT( buffer.Size() >= 2 );
	buffer.EraseBack();
	buffer.Back() = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Grow(const TSIZE length,const CHAR FillCharacter)
{
	Resize( Length() + length, FillCharacter );
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Insert
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Insert(ITERATOR pos,const PDXSTRING& string)
{
	PDX_ASSERT(pos);

	if (string.Size())
	{
		buffer.Insert
		(
	     	pos,
			string.buffer.Begin(),
			string.buffer.At(string.buffer.Size()-1)
		);

		Terminate();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Insert
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Insert(ITERATOR pos,CONSTITERATOR begin,CONSTITERATOR end)
{
	PDX_ASSERT(pos && begin && end);
	buffer.Insert(pos,begin,end);
	Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Replace a string with another
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::Replace(ITERATOR begin,CONSTITERATOR end,CONSTITERATOR InsertBegin,CONSTITERATOR InsertEnd)
{
	const TSIZE pos = end - begin;
	buffer.Erase(begin,end);
	buffer.Insert(buffer.At(pos),InsertBegin,InsertEnd);
	Terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Compare, case-sensetive
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::ExactEqual(const PDXSTRING& string) const
{
	if (buffer.Size() != string.buffer.Size())
		return FALSE;

	for (TSIZE i=0; i < buffer.Size(); ++i)
		if (buffer[i] != string.buffer[i])
			return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Compare, case-sensetive
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::ExactEqual(CONSTITERATOR string) const
{
	PDX_ASSERT(string);

	if (buffer.IsEmpty())
		return *string == '\0';

	TSIZE i=0;

	for (; i < buffer.Size() && string[i] != '\0'; ++i)
		if (buffer[i] != string[i])
			return FALSE;

	return (i+1) == buffer.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Compare, NOT case-sensetive
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::operator == (CONSTITERATOR string) const
{
	PDX_ASSERT(string);

	if (buffer.IsEmpty())
		return *string == '\0';

	TSIZE i=0;

	for (; i < buffer.Size() && string[i] != '\0'; ++i)
		if (converter.lower[buffer[i]] != converter.lower[string[i]])
			return FALSE;

	return (i+1) == buffer.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Compare, NOT case-sensetive
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::operator == (const PDXSTRING& string) const 
{
	if (buffer.Size() != string.buffer.Size())
		return FALSE;

	for (TSIZE i=0; i < buffer.Size(); ++i)
		if (converter.lower[buffer[i]] != converter.lower[string.buffer[i]])
			return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Less than, NOT case-sensetive
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::operator < (CONSTITERATOR string) const
{
	PDX_ASSERT(string);

	if (buffer.IsEmpty())
		return *string != '\0';

	for (UINT i=0; ; ++i)
	{
		if (buffer[i] == '\0') return string[i] != '\0';
		if (string[i] == '\0') break;
		if (converter.lower[buffer[i]] < converter.lower[string[i]]) return TRUE;
		if (converter.lower[buffer[i]] > converter.lower[string[i]]) break;
	}

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Convert to lower case
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::SetLowerCase()
{
	for (TSIZE i=0; i < buffer.Size(); ++i)
		buffer[i] = converter.lower[buffer[i]];
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Convert to upper case
//////////////////////////////////////////////////////////////////////////////////////////////

VOID PDXSTRING::SetUpperCase()
{
	for (TSIZE i=0; i < buffer.Size(); ++i)
		buffer[i] = converter.upper[buffer[i]];
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Find first occurance of a substring
//////////////////////////////////////////////////////////////////////////////////////////////

TSIZE PDXSTRING::PosAtFirstOf(CONSTITERATOR SubBegin,CONSTITERATOR SubEnd,CONSTITERATOR begin,CONSTITERATOR end)
{
	PDX_ASSERT(begin && end && SubBegin && SubEnd);

	const TSIZE length = end - begin;
	const TSIZE SubLength = SubEnd - SubBegin; 

	for (TSIZE i=0; (i+SubLength) <= length; ++i)
	{
		CONSTITERATOR pos = begin+i;

		TSIZE j;

		for (j=0; j < SubLength; ++j)
			if (converter.lower[pos[j]] != converter.lower[SubBegin[j]])
				break;

		if (j == SubLength)
			return i;
	}

	return length;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Find last occurance of a substring
//////////////////////////////////////////////////////////////////////////////////////////////

TSIZE PDXSTRING::PosAtLastOf(CONSTITERATOR SubBegin,CONSTITERATOR SubEnd,CONSTITERATOR begin,CONSTITERATOR end)
{
	PDX_ASSERT(begin && end && SubBegin && SubEnd);

	const TSIZE length = end - begin;
	const TSIZE SubLength = SubEnd - SubBegin; 

	for (LONG i=length-SubLength; i >= 0; --i)
	{
		CONSTITERATOR pos = &begin[i];

		TSIZE j;

		for (j=0; j < SubLength; ++j)
			if (converter.lower[pos[j]] != converter.lower[SubBegin[j]])
				break;

		if (j == SubLength)
			return i;
	}

	return length;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING PDXSTRING::Quoted() const
{
	PDXSTRING quoted;

	const TSIZE length = Length();

	quoted.buffer.Resize( 3 + length );
	quoted.buffer.Front() = '\"';

	memcpy( quoted.buffer.At(1), buffer.Begin(), length );
	
	quoted.buffer[1 + length] = '\"';
	quoted.buffer[2 + length] = '\0';

	return quoted;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::IsFileExtension(const CHAR* ext,const TSIZE length) const
{
	if (buffer.Size() > 1)
	{
		for (const CHAR* c=buffer.At(buffer.Size()-2); c >= buffer.Begin(); --c)
		{
			if (*c == '.')
			{
				++c;

				for (const CHAR* end = ext + (length ? length : strlen(ext)); ext != end; ++ext)
				{
					if (*c++ != *ext)
						return FALSE;
				}

				while (*c != '\0')
				{
					if (*c != ' ' && *c != '\r' && *c != '\n' && *c != '\t' && *c != '\v')
						return FALSE;

					++c;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Return the directory path
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::GetFilePath(PDXSTRING& path) const
{
	CONSTITERATOR last = AtLastOf("\\");

	if (last++ != End()) 
	{
		const CHAR* const d = ".";

		if (AtFirstOf(d+0,d+1,last,End()) == End())
			last = End();

		path.Set(Begin(),last);

		if (path.Back() != '\\')
			path += "\\";
	}
	else
	{
		path.Set(".");
	}

	return path;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Return the file name
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::GetFileName(PDXSTRING& file) const
{
	CONSTITERATOR last = AtLastOf("\\");

	if (last != End()) file.Set(&last[1],End());
	else               file.Set(*this);

	return file;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Return the file extension (.xxx)
//////////////////////////////////////////////////////////////////////////////////////////////

PDXSTRING& PDXSTRING::GetFileExtension(PDXSTRING& ext) const
{
	CONSTITERATOR last = AtLastOf(".");

	if (last != End()) ext.Set(&last[1],End());
	else               ext.Clear();

	return ext;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Replace the file extension (.xxx)
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::ReplaceFileExtension(CONSTITERATOR ext)
{
	PDX_ASSERT(ext);

	BOOL result = FALSE;

	ITERATOR last = AtLastOf(".");
	CONSTITERATOR end = End();

	if (last != end)
	{
		buffer.Erase(&last[1],end);
		result = TRUE;
	}
	else
	{
		Append(".");
	}

	Append(ext);

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

BOOL PDXSTRING::ReplaceFilePath(const PDXSTRING& string)
{
	if (string.IsEmpty())
		return FALSE;

	BOOL result = FALSE;

	CONSTITERATOR last = AtLastOf("\\");

	if (last++ != End()) 
	{
		const CHAR* const d = ".";

		if (AtFirstOf(d+0,d+1,last,End()) == End())
			last = End();

		buffer.Erase(Begin(),last);
		result = TRUE;
	}

	PDXSTRING tmp;
	const PDXSTRING* ptr = &string;

	if (string.Back() != '\\')
	{
		tmp = string;
		tmp += "\\";
		ptr = &tmp;
	}

	buffer.Insert(Begin(),ptr->Begin(),ptr->End());

	return result;
}