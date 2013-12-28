////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003-2005 Martin Freij
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

#ifndef NST_DIRECTX_DIRECTSOUND_H
#define NST_DIRECTX_DIRECTSOUND_H

#pragma once

#include "NstObjectPod.hpp"
#include "NstDirectX.hpp"

#define NOMMIDS  
#define NONEWWAVE
#define NONEWRIFF
#define NOJPEGDIB
#define NONEWIC  
#define NOBITMAP 

#include <MMReg.h>
#include <dsound.h>

namespace Nestopia
{
	namespace DirectX
	{
		class DirectSound
		{
		public:

			explicit DirectSound(HWND);
			~DirectSound();

			typedef BaseAdapter Adapter;
			typedef std::vector<Adapter> Adapters;

			enum Channels
			{
				MONO = 1,
				STEREO = 2
			};

			enum
			{
				VOLUME_MAX = 100,
				LATENCY_MAX = 10
			};

			cstring Update(uint,uint,uint,Channels,uint,uint,uint);
			cstring UpdateSpeed(uint,uint);
			void    Destroy();

		private:

			static BOOL CALLBACK EnumAdapter(LPGUID,LPCSTR,LPCSTR,LPVOID);

			class Buffer
			{
			public:

				Buffer();
				~Buffer();

				cstring Update(IDirectSound8&,ibool,uint,uint,Channels,uint,uint,uint);
				cstring UpdateSpeed(IDirectSound8&,ibool,uint,uint);
				void    StartStream();
				ibool   LockStream(void*&,uint&);
				void    Release();

			private:

				static uint CalculateSize(uint,uint,uint,uint);

				cstring Create(IDirectSound8&,ibool);
				void UpdateVolume() const;

				enum
				{
					DC_OFFSET_8 = 0x80,
					DC_OFFSET_16 = 0x0000,
					DECAY_8 = 1,
					DECAY_16 = 64
				};

				struct Settings
				{
					Settings();

					uint size;
					uint volume;
				};

				ComInterface<IDirectSoundBuffer8> com;
				uint writeOffset;
				Object::Pod<WAVEFORMATEX> waveFormat;
				Settings settings;

			public:

				const WAVEFORMATEX& GetWaveFormat() const
				{
					return waveFormat;
				}

				uint NumSamples() const
				{
					NST_ASSERT( settings.size % waveFormat.nBlockAlign == 0 );
					return settings.size / waveFormat.nBlockAlign;
				}

				void UnlockStream(void* data) const
				{
					NST_ASSERT( data && com );
					com->Unlock( data, settings.size, NULL, NULL );
				}

				ibool IsStreaming() const
				{
					DWORD status;

					return 
					(
				     	com && SUCCEEDED(com->GetStatus( &status )) && 
						(status & (DSBSTATUS_BUFFERLOST|DSBSTATUS_PLAYING|DSBSTATUS_LOOPING)) == (DSBSTATUS_PLAYING|DSBSTATUS_LOOPING)
					);
				}

				void StopStream() const
				{
					if (com)
						com->Stop();
				}
			};

			struct Settings
			{
				Settings(HWND);

				HWND const hWnd;
				uint deviceId;
				ibool priority;
			};

			ComInterface<IDirectSound8> device;
			Buffer buffer;
			Settings settings;
			Adapters adapters;

		public:

			ibool IsStreaming() const
			{
				return buffer.IsStreaming();
			}

			void StopStream() const
			{
				buffer.StopStream();
			}

			void StartStream()
			{
				buffer.StartStream();
			}

			ibool LockStream(void*& data,uint& size)
			{
				return buffer.LockStream( data, size );
			}

			void UnlockStream(void* data) const
			{
				buffer.UnlockStream( data );
			}

			uint NumSamples() const
			{
				return buffer.NumSamples();
			}

			const Adapters& GetAdapters() const
			{
				return adapters;
			}

			const WAVEFORMATEX& GetWaveFormat() const
			{
				return buffer.GetWaveFormat();
			}
		};
	}
}

#endif
