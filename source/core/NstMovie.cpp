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

#include <new>
#include "NstState.hpp"
#include "api/NstApiEmulator.hpp"
#include "api/NstApiMovie.hpp"
#include "api/NstApiUser.hpp"
#include "NstMovie.hpp"

namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif

		class Movie::Recorder
		{
		public:

			void Start(ulong,dword);
			void BeginFrame(dword,Api::Emulator&,SaveCallback);
			uint WritePort(uint,uint);
			void MachineReset(bool);
			void EndFrame();
			void Stop();

		private:

			void Flush();

			typedef Vector<u8> Buffer;

			struct Port
			{
				bool Unlock();
				NST_FORCE_INLINE void Flush(State::Saver&,uint);
				NST_FORCE_INLINE void Sync(uint);

				dword lock;
				Buffer input;
				Buffer recent;
				Buffer output;

				Port()
				: lock(0) {}
			};

			ibool good;
			dword frame;
			Port port[2];
			State::Saver state;
			const ulong streamPos;

		public:

			Recorder(StdStream stream)
			: good(true), frame(0), state(stream,true), streamPos(state.GetStream().GetPos()) {}

			bool operator == (StdStream stream)
			{
				return state.GetStream().GetStdStream() == stream;
			}
		};

		class Movie::Player
		{
		public:

			void Start(dword);
			bool BeginFrame(dword,Api::Emulator&,LoadCallback,ResetCallback);
			inline uint ReadPort(uint);
			void EndFrame();
			void Stop();

		private:

			typedef Vector<u8> Buffer;

			struct Port
			{
				bool Load(State::Loader&);
				NST_FORCE_INLINE void Sync(uint);

				dword lock;
				dword next;
				dword offset;
				dword pos;
				Buffer output;
			};

			struct Frame
			{
				uint port;
				dword clip;
				dword reset;
				dword wait;
			};

			ibool good;
			Frame frame;
			Port port[2];
			State::Loader state;
			const ulong streamPos;

		public:

			Player(StdStream stream)
			: state(stream), streamPos(state.GetStream().GetPos()) {}

			bool operator == (StdStream stream)
			{
				return state.GetStream().GetStdStream() == stream;
			}
		};

		Movie* Movie::Create(Cpu& cpu,dword crc)
		{
			return new Movie( cpu, crc );
		}

		void Movie::Destroy(Movie*& movie)
		{
			delete movie;
			movie = NULL;
		}

		Movie::Movie(Cpu& c,dword crc)
		:			
		cpu      (c),
		status   (STOPPED),
		player   (NULL),
		recorder (NULL),
		prgCrc   (crc)
		{
		}

		Movie::~Movie()
		{
			Close();
		}

		void Movie::Close()
		{
			Stop();

			delete player;
			player = NULL;

			delete recorder;
			recorder = NULL;
		}

		Result Movie::Record(StdStream const stream,const bool end)
		{
			NST_ASSERT( stream );

			if (status != PLAYING)
			{
				if (player)
				{
					Close();
				}
				else if (recorder)
				{
					if (*recorder == stream)
					{
						if (status == RECORDING)
							return RESULT_NOP;
					}
					else
					{
						Close();
					}
				}
				
				if (recorder == NULL)
					recorder = new Recorder( stream );

				try
				{
					recorder->Start( end, prgCrc );
				}
				catch (...)
				{
					delete recorder;
					recorder = NULL;
					throw;
				}

				status = RECORDING;
				SaveCpuPorts();

				Api::Movie::stateCallback( Api::Movie::RECORDING );

				return RESULT_OK;
			}

			return RESULT_ERR_NOT_READY;
		}
	
		Result Movie::Play(StdStream const stream)
		{
			NST_ASSERT( stream );

			if (status != RECORDING)
			{
				if (recorder)
				{
					Close();
				}
				else if (player)
				{
					if (*player == stream)
					{
						if (status == PLAYING)
							return RESULT_NOP;
					}
					else
					{
						Close();
					}
				}

				if (player == NULL)
					player = new Player( stream );

				try
				{
					player->Start( prgCrc );
				}
				catch (...)
				{
					delete player;
					player = NULL;
					throw;
				}

				status = PLAYING;
				SaveCpuPorts();

				Api::Movie::stateCallback( Api::Movie::PLAYING );

				return RESULT_OK;
			}

			return RESULT_ERR_NOT_READY;
		}

		bool Movie::Stop(Result result)
		{
			if (status != STOPPED)
			{
				status = STOPPED;

				NST_ASSERT( cpu.Map(0x4016).SameComponent(this) && cpu.Map(0x4017).SameComponent(this) );

				for (uint i=0; i < 2; ++i)
					cpu.Map( 0x4016 + i ) = ports[i];

				if (NES_SUCCEEDED(result))
				{
					try
					{
						if (recorder)
						{
							recorder->Stop();
						}
						else if (player)
						{
							player->Stop();
						}
					}
					catch (Result r)
					{
						result = r;
					}
					catch (std::bad_alloc&)
					{
						result = RESULT_ERR_OUT_OF_MEMORY;
					}
					catch (...)
					{
						result = RESULT_ERR_GENERIC;
					}

					if (NES_SUCCEEDED(result))
						Api::Movie::stateCallback( Api::Movie::STOPPED ); 
				}
			}

			if (NES_SUCCEEDED(result))
				return true;

			delete recorder;
			recorder = NULL;

			delete player;
			player = NULL;

			Api::Movie::State state;

			switch (result)
			{
				case RESULT_ERR_OUT_OF_MEMORY:

					state = Api::Movie::ERR_OUT_OF_MEMORY;
					break;

				case RESULT_ERR_INVALID_FILE:
				case RESULT_ERR_CORRUPT_FILE:	  
					
					state = Api::Movie::ERR_CORRUPT_FILE;
					break;

				case RESULT_ERR_UNSUPPORTED_GAME: 
					
					state = Api::Movie::ERR_UNSUPPORTED_IMAGE; 
					break;

    			default:
					
					state = Api::Movie::ERR_GENERIC;           
					break;
			}

			Api::Movie::stateCallback( state );
			
			return false;
		}

		void Movie::Stop()
		{
			Stop( RESULT_OK );
		}

		bool Movie::BeginFrame
		(
	     	const dword frame,
			Api::Emulator& emulator,
			SaveCallback saver,
			LoadCallback loader,
			ResetCallback reseter
		)
		{
			try
			{
				if (status == RECORDING)
				{
					recorder->BeginFrame( frame, emulator, saver );
				}
				else if (status == PLAYING)
				{
					if (!player->BeginFrame( frame, emulator, loader, reseter ))
						Stop();
				}

				return true;
			}
			catch (Result result)
			{
				return Stop( result );
			}
			catch (std::bad_alloc&)
			{
				return Stop( RESULT_ERR_OUT_OF_MEMORY );
			}
			catch (...)
			{
				return Stop( RESULT_ERR_GENERIC );
			}
		}
	
		bool Movie::EndFrame()
		{
			try
			{
				if (status == RECORDING)
				{
					recorder->EndFrame();
				}
				else if (status == PLAYING)
				{
					player->EndFrame();
				}

				return true;
			}
			catch (Result result)
			{
				return Stop( result );
			}
			catch (std::bad_alloc&)
			{
				return Stop( RESULT_ERR_OUT_OF_MEMORY );
			}
			catch (...)
			{
				return Stop( RESULT_ERR_GENERIC );
			}
		}

		bool Movie::MachineReset(const bool hard)
		{
			if (status != STOPPED)
			{
				SaveCpuPorts();

				if (status == RECORDING)
				{
					try
					{
						recorder->MachineReset( hard );
					}
					catch (Result result)
					{
						return Stop( result );
					}
					catch (std::bad_alloc&)
					{
						return Stop( RESULT_ERR_OUT_OF_MEMORY );
					}
					catch (...)
					{
						return Stop( RESULT_ERR_GENERIC );
					}
				}
			}

			return true;
		}
	
		void Movie::SaveCpuPorts()
		{
			NST_ASSERT
			( 
		       	status != STOPPED && 
				!cpu.Map( 0x4016 ).SameComponent( this ) && 
				!cpu.Map( 0x4017 ).SameComponent( this ) 
			);

			ports[0] = cpu.Map( 0x4016 );
			ports[1] = cpu.Map( 0x4017 );

			const bool recording = (status == RECORDING);

			cpu.Map( 0x4016 ).Set( this, recording ? &Movie::Peek_4016_Record : &Movie::Peek_4016_Play, &Movie::Poke_4016 );
			cpu.Map( 0x4017 ).Set( this, recording ? &Movie::Peek_4017_Record : &Movie::Peek_4017_Play, &Movie::Poke_4017 );
		}

		NST_FORCE_INLINE void Movie::Recorder::Port::Flush(State::Saver& state,const uint index)
		{
			Unlock();
			recent.Clear();

			if (output.Size())
			{
				state.Begin('P','T','0'+index,'\0').Write32( output.Size() - 1 ).Compress( output.Begin(), output.Size() ).End();
				output.Clear();
			}
		}

		void Movie::Recorder::Flush()
		{
			for (uint i=0; i < 2; ++i)
				port[i].Flush( state, i );
		}

		void Movie::Recorder::Start(ulong end,const dword prgCrc)
		{
			static const u8 header[5] =
			{
				'N','S','V', 0x1A, VERSION
			};

			static const u8 reserved[7] = 
			{
				0,0,0,0,0,0,0
			};

			state.GetStream().SetPos( streamPos );

			if (end && 0 < (end = state.GetStream().Length()))
			{
				frame = ~dword(0);
				
				if (end >= sizeof(header) + 4 + sizeof(reserved))
					state.GetStream().Seek( end );
				else
					throw RESULT_ERR_CORRUPT_FILE;
			}
			else
			{
				frame = 0;

				state.GetStream().Write( header, sizeof(header) );
				state.GetStream().Write32( prgCrc );
				state.GetStream().Write( reserved, sizeof(reserved) );
			}
		}

		void Movie::Recorder::Stop()
		{
			NST_VERIFY( bool(frame) >= bool(port[0].output.Size() || port[1].output.Size()) );

			Flush();

			if (frame && frame != ~dword(0))
				state.Begin('W','A','I','\0').Write32( frame ).End();

			state.GetStream().SetPos( streamPos );
		}

		void Movie::Recorder::MachineReset(const bool hard)
		{
			if (frame)
			{
				Flush();
				state.Begin('R','E','S','\0').Write32( frame != ~dword(0) ? frame : 0 ).Write8( hard ).End();
				frame = 0;
			}
		}

		void Movie::Player::Start(const dword prgCrc)
		{
			good = false;
			
			frame.port = 0;
			frame.clip = ~dword(0);
			frame.reset = ~dword(0);
			frame.wait = ~dword(0);

			for (uint i=0; i < 2; ++i)
			{
				port[i].pos = 0;
				port[i].offset = 0;
				port[i].lock = 0;
				port[i].next = 0;
				port[i].output.Clear();
			}

			state.GetStream().SetPos( streamPos );

			if (state.GetStream().Read32() != NES_STATE_CHUNK_ID('N','S','V',0x1A))
				throw RESULT_ERR_INVALID_FILE;

			if (state.GetStream().Read8() != VERSION)
				throw RESULT_ERR_UNSUPPORTED_FILE_VERSION;

			const dword crc = state.GetStream().Read32();

			if 
			(
				crc && crc != prgCrc &&
				Api::User::questionCallback( Api::User::QUESTION_NSV_PRG_CRC_FAIL_CONTINUE ) == Api::User::ANSWER_NO
			)
			    throw RESULT_ERR_INVALID_CRC;

			state.GetStream().Seek( 7 );

			good = true;
		}

		void Movie::Player::Stop()
		{
			state.GetStream().SetPos( streamPos );
		}

		bool Movie::Player::Port::Load(State::Loader& state)
		{
			if (pos != output.Size())
				return false;
			
			pos = 0;
			offset = 0;

			const dword size = state.Read32() + 1;

			if (size <= 0x02000000UL)
			{
				output.Resize( size );
				state.Uncompress( output.Begin(), size );
				return true;
			}
			else
			{
				throw RESULT_ERR_CORRUPT_FILE;
			}
		}

		bool Movie::Recorder::Port::Unlock()
		{
			if (dword count = lock)
			{
				lock = 0;

				output.Expand( --count < LOCK_SIZE_BIT ? 1 : 4 );

				if (count < LOCK_SIZE_BIT)
				{
					output.Back() = (LOCK_BIT|LOCK_SIZE_BIT) | count;
				}
				else
				{
					u8* const ptr = output.End() - 4;

					ptr[0] = (count >> 24) | LOCK_BIT;
					ptr[1] = (count >> 16) & 0xFF;
					ptr[2] = (count >>  8) & 0xFF;
					ptr[3] = (count >>  0) & 0xFF;
				}

				return true;
			}

			return false;
		}

		NST_FORCE_INLINE void Movie::Recorder::Port::Sync(const uint index)
		{
			NST_ASSERT( index <= 1 );

			if (input.Size())
			{
				if (input.Size() <= MAX_FRAME_READS)
				{
					if (recent == input)
					{
						++lock;
					}
					else
					{
						if (!Unlock() && index && recent.Size())
							output << 0x00; // $4016 can afford one bit while $4017 can't

						recent = input;
						output += input;
					}
				}
				else
				{
					throw RESULT_ERR_UNSUPPORTED_GAME;
				}
			}
		}
	
		NST_FORCE_INLINE void Movie::Player::Port::Sync(uint index)
		{
			NST_ASSERT( index <= 1 );

			if (pos > offset)
			{
				if (lock)
				{
					if (--lock)
					{
						pos = offset;
					}
					else
					{
						pos = offset = next;
					}
				}
				else if (pos < output.Size())
				{
					const uint ctrl = output[pos];
			
					if (ctrl & LOCK_BIT)
					{
						next = pos;
						pos = offset;
			
						if (ctrl & LOCK_SIZE_BIT)
						{
							next += 1;
							lock = (ctrl & ~uint(LOCK_BIT|LOCK_SIZE_BIT)) + 1;
						}
						else 
						{
							next += 4;
			
							if (next <= output.Size())
							{
								lock = 1 +
								(
									( (ctrl & ~uint(LOCK_BIT)) << 24) | 
									( output[next-3]           << 16) | 
									( output[next-2]           <<  8) | 
									( output[next-1]           <<  0)
								);
							}					
							else
							{
								throw RESULT_ERR_CORRUPT_FILE;
							}
						}
					}
					else
					{
						pos += index; // $4016 can afford one bit while $4017 can't
						offset = pos;
					}
				}
			}
		}

		void Movie::Recorder::BeginFrame(const dword emuFrame,Api::Emulator& emulator,SaveCallback saver)
		{
			NST_VERIFY( frame <= emuFrame || frame == ~dword(0) );

			for (uint i=0; i < 2; ++i)
				port[i].input.Clear();

			if (frame == emuFrame)
				return;
			
			Flush();

			if (emuFrame)
			{
				state.Begin('C','L','P','\0').Write32( frame != ~dword(0) ? frame : 0 );

				const Result result = (emulator.*saver)( state.GetStream().GetStdStream(), true );

				if (NES_FAILED(result))
					throw result;

				state.End();
			}
			else
			{
				MachineReset( true );
			}

			frame = emuFrame;			
		}

		bool Movie::Player::BeginFrame
		(
	     	const dword emuFrame,
			Api::Emulator& emulator,
			LoadCallback loader,
			ResetCallback reseter
		)
		{
			for (;;)
			{
				if (frame.port)
				{
					NST_ASSERT( frame.port <= 2 );

					if (port[frame.port - 1].Load( state ))
					{
						frame.port = 0;
						state.DigOut();
						state.End();
					}
					else
					{
						break;
					}
				}
				else if (frame.clip != ~dword(0))
				{
					if (frame.clip <= emuFrame)
					{
						frame.clip = ~dword(0);

						const Result result = (emulator.*loader)( state.GetStream().GetStdStream(), false );

						if (NES_SUCCEEDED(result))
						{
							state.DigOut();
							state.End();
						}
						else
						{
							throw RESULT_ERR_CORRUPT_FILE;
						}
					}
					else
					{
						break;
					}
				}			
				else if (frame.reset != ~dword(0))
				{
					if (frame.reset <= emuFrame)
					{
						frame.reset = ~dword(0);
						
						const Result result = (emulator.*reseter)( state.Read8() & 0x1, false );

						if (NES_SUCCEEDED(result))
						{
							state.DigOut();
							state.End();
						}
						else
						{
							throw result;
						}
					}
					else
					{
						break;
					}
				}
				else if (frame.wait != ~dword(0))
				{
					if (frame.wait <= emuFrame)
					{
						frame.wait = ~dword(0);
						
						state.DigOut();
						state.End();
					}
					else
					{
						break;
					}
				}
	
				if (!state.GetStream().Eof())
				{
					switch (state.Begin())
					{
						case NES_STATE_CHUNK_ID('P','T','0','\0'):
					
							state.DigIn();
							frame.port = 1;
							break;
					
						case NES_STATE_CHUNK_ID('P','T','1','\0'):
					
							state.DigIn();
							frame.port = 2;
							break;
					
						case NES_STATE_CHUNK_ID('C','L','P','\0'):
					
							state.DigIn();
							frame.clip = state.Read32() & ~dword(1);
							break;
					
						case NES_STATE_CHUNK_ID('R','E','S','\0'):
					
							state.DigIn();
							frame.reset = state.Read32() & ~dword(1);
							break;
					
						case NES_STATE_CHUNK_ID('W','A','I','\0'):
					
							state.DigIn();
							frame.wait = state.Read32() & ~dword(1);
							break;
					
						default:
					
							return false;
					}
				}
				else
				{
					return port[0].pos < port[0].output.Size() || port[1].pos < port[1].output.Size();					
				}
			}

			return true;
		}

		void Movie::Recorder::EndFrame()
		{
			if (good)
			{
				++frame;

				for (uint i=0; i < 2; ++i)
					port[i].Sync( i );
			}
			else
			{
				throw RESULT_ERR_OUT_OF_MEMORY;
			}
		}

		void Movie::Player::EndFrame()
		{
			if (good)
			{
				for (uint i=0; i < 2; ++i)
					port[i].Sync( i );
			}
			else
			{
				throw RESULT_ERR_CORRUPT_FILE;
			}
		}

        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif

		uint Movie::Recorder::WritePort(const uint index,const uint data)	
		{
			NST_ASSERT( index || !(data & LOCK_BIT) );

			if (good)
			{
				try
				{
					port[index].input << data;
				}
				catch (...)
				{
					good = false;
				}
			}

			return data;
		}

		inline uint Movie::Player::ReadPort(const uint index)
		{
			if (good)
			{
				if (port[index].pos < port[index].output.Size())
					return port[index].output[port[index].pos++];

				good = false;
			}

			return OPEN_BUS;
		}

		NES_PEEK(Movie,4016_Record)
		{
			return recorder->WritePort( 0, ports[0].Peek( 0x4016 ) );
		}
	
		NES_PEEK(Movie,4016_Play)
		{
			return player->ReadPort( 0 );
		}
	
		NES_PEEK(Movie,4017_Record)
		{
			return recorder->WritePort( 1, ports[1].Peek( 0x4017 ) );
		}
	
		NES_PEEK(Movie,4017_Play)
		{
			return player->ReadPort( 1 );
		}
	
		NES_POKE(Movie,4016)
		{
			ports[0].Poke( address, data );
		}
	
		NES_POKE(Movie,4017)
		{
			ports[1].Poke( address, data );
		}
	}
}
