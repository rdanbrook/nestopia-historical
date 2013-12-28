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

#include "../NstCore.hpp"
#include "NstApiSound.hpp"
#include "NstApiEmulator.hpp"
#include "NstApiMovie.hpp"
#include "../NstRewinder.hpp"

namespace Nes
{
	namespace Core
	{
		namespace Sound
		{
			Output::Locker Output::lockCallback;
			Output::Unlocker Output::unlockCallback;
			Loader::Callbacker Loader::loadCallback;
		}
	}

	namespace Api
	{
		NST_COMPILE_ASSERT
		(
	       	Sound::CHANNEL_SQUARE1  == 1U << Core::Apu::CHANNEL_SQUARE1  &&
			Sound::CHANNEL_SQUARE2  == 1U << Core::Apu::CHANNEL_SQUARE2  &&
			Sound::CHANNEL_TRIANGLE == 1U << Core::Apu::CHANNEL_TRIANGLE &&
			Sound::CHANNEL_NOISE    == 1U << Core::Apu::CHANNEL_NOISE    &&
			Sound::CHANNEL_DPCM     == 1U << Core::Apu::CHANNEL_DPCM
		);

		Result Sound::SetSampleRate(ulong rate)
		{
			return emulator.cpu.GetApu().SetSampleRate( rate );
		}
	
		Result Sound::SetSampleBits(uint bits)
		{
			return emulator.cpu.GetApu().SetSampleBits( bits );
		}

		Result Sound::SetVolume(uint channels,uint volume)
		{
			return emulator.cpu.GetApu().SetVolume( channels, volume );
		}

		Result Sound::SetSpeed(uint speed)
		{
			return emulator.cpu.GetApu().SetSpeed( speed );
		}

		void Sound::SetAutoTranspose(bool enable)
		{
			emulator.cpu.GetApu().SetAutoTranspose( enable );
		}
	
		void Sound::SetSpeaker(Speaker speaker)
		{
			return emulator.cpu.GetApu().EnableStereo( speaker == SPEAKER_STEREO );
		}

		ulong Sound::GetSampleRate() const
		{
			return emulator.cpu.GetApu().GetSampleRate();
		}
	
		uint Sound::GetSampleBits() const
		{
			return emulator.cpu.GetApu().GetSampleBits();
		}

		uint Sound::GetVolume(uint channel) const
		{
			return emulator.cpu.GetApu().GetVolume( channel );
		}

		uint Sound::GetSpeed() const
		{
			return emulator.cpu.GetApu().GetSpeed();
		}
	
		bool Sound::IsAudible() const
		{
			return emulator.cpu.GetApu().IsAudible();
		}

		bool Sound::IsAutoTransposing() const
		{
			return emulator.cpu.GetApu().IsAutoTransposing();
		}

		Sound::Speaker Sound::GetSpeaker() const
		{
			return emulator.cpu.GetApu().InStereo() ? SPEAKER_STEREO : SPEAKER_MONO;
		}
	
		uint Sound::GetLatency() const
		{
			if (!emulator.rewinder || !emulator.rewinder->IsSoundRewinding())
				return emulator.cpu.GetApu().GetLatency();
			else
				return emulator.rewinder->GetSoundLatency();
		}
	
		void Sound::EmptyBuffer()
		{
			return emulator.cpu.GetApu().ClearBuffers();
		}
	}
}
