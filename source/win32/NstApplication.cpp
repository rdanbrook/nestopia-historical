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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#pragma comment(lib,"comctl32")

#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <ShellAPI.h>
#include "NstGraphicManager.h"
#include "NstFileManager.h"
#include "NstTimerManager.h"
#include "NstLogFileManager.h"
#include "NstSoundManager.h"
#include "NstInputManager.h"
#include "NstFdsManager.h"
#include "NstSaveStateManager.h"
#include "NstMovieManager.h"
#include "NstGameGenieManager.h"
#include "NstVsDipSwitchManager.h"
#include "NstPreferences.h"
#include "NstRomInfo.h"
#include "NstHelpManager.h"
#include "NstUserInputManager.h"
#include "NstApplication.h"
#include "NstCmdLine.h"
#include "NstLauncher.h"
#include "NstNetplayManager.h"

#define NST_DELETE(x) delete x; x=NULL

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDX_COMPILE_ASSERT
(
	( IDM_MACHINE_PORT1_PAD1		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 1  &&  
	( IDM_MACHINE_PORT1_PAD2		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 2  && 
	( IDM_MACHINE_PORT1_PAD3		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 3  && 
	( IDM_MACHINE_PORT1_PAD4	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 4  && 
	( IDM_MACHINE_PORT1_ZAPPER	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 5  && 
	( IDM_MACHINE_PORT1_PADDLE	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 6  && 
	( IDM_MACHINE_PORT1_POWERPAD                - IDM_MACHINE_PORT1_UNCONNECTED ) == 7  && 
	( IDM_MACHINE_PORT2_UNCONNECTED             - IDM_MACHINE_PORT1_UNCONNECTED ) == 8  && 
	( IDM_MACHINE_PORT2_PAD1		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 9	&&
	( IDM_MACHINE_PORT2_PAD2		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 10 && 
	( IDM_MACHINE_PORT2_PAD3	  	            - IDM_MACHINE_PORT1_UNCONNECTED ) == 11 &&  
	( IDM_MACHINE_PORT2_PAD4	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 12 && 
	( IDM_MACHINE_PORT2_ZAPPER	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 13 && 
	( IDM_MACHINE_PORT2_PADDLE	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 14 && 
	( IDM_MACHINE_PORT2_POWERPAD                - IDM_MACHINE_PORT1_UNCONNECTED ) == 15 && 
	( IDM_MACHINE_PORT3_UNCONNECTED             - IDM_MACHINE_PORT1_UNCONNECTED ) == 16 && 
	( IDM_MACHINE_PORT3_PAD1		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 17 && 
	( IDM_MACHINE_PORT3_PAD2		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 18 && 
	( IDM_MACHINE_PORT3_PAD3		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 19 &&
	( IDM_MACHINE_PORT3_PAD4	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 20 && 
	( IDM_MACHINE_PORT4_UNCONNECTED             - IDM_MACHINE_PORT1_UNCONNECTED ) == 21 && 
	( IDM_MACHINE_PORT4_PAD1		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 22 && 
	( IDM_MACHINE_PORT4_PAD2		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 23 && 
	( IDM_MACHINE_PORT4_PAD3		            - IDM_MACHINE_PORT1_UNCONNECTED ) == 24 && 
	( IDM_MACHINE_PORT4_PAD4	                - IDM_MACHINE_PORT1_UNCONNECTED ) == 25 && 
	( IDM_MACHINE_EXPANSION_UNCONNECTED         - IDM_MACHINE_PORT1_UNCONNECTED ) == 26 &&
	( IDM_MACHINE_EXPANSION_FAMILYBASICKEYBOARD - IDM_MACHINE_PORT1_UNCONNECTED ) == 27
);

PDX_COMPILE_ASSERT
(
	( IDM_FDS_INSERT_DISK_2  - IDM_FDS_INSERT_DISK_1 ) == 1  && 
	( IDM_FDS_INSERT_DISK_3  - IDM_FDS_INSERT_DISK_1 ) == 2  &&  
	( IDM_FDS_INSERT_DISK_4  - IDM_FDS_INSERT_DISK_1 ) == 3  && 
	( IDM_FDS_INSERT_DISK_5  - IDM_FDS_INSERT_DISK_1 ) == 4  && 
	( IDM_FDS_INSERT_DISK_6  - IDM_FDS_INSERT_DISK_1 ) == 5  && 
	( IDM_FDS_INSERT_DISK_7  - IDM_FDS_INSERT_DISK_1 ) == 6  && 
	( IDM_FDS_INSERT_DISK_8  - IDM_FDS_INSERT_DISK_1 ) == 7  && 
	( IDM_FDS_INSERT_DISK_9  - IDM_FDS_INSERT_DISK_1 ) == 8  && 
	( IDM_FDS_INSERT_DISK_10 - IDM_FDS_INSERT_DISK_1 ) == 9	 &&
	( IDM_FDS_INSERT_DISK_11 - IDM_FDS_INSERT_DISK_1 ) == 10 &&
	( IDM_FDS_INSERT_DISK_12 - IDM_FDS_INSERT_DISK_1 ) == 11 &&
	( IDM_FDS_INSERT_DISK_13 - IDM_FDS_INSERT_DISK_1 ) == 12 &&
	( IDM_FDS_INSERT_DISK_14 - IDM_FDS_INSERT_DISK_1 ) == 13 &&
	( IDM_FDS_INSERT_DISK_15 - IDM_FDS_INSERT_DISK_1 ) == 14 &&
	( IDM_FDS_INSERT_DISK_16 - IDM_FDS_INSERT_DISK_1 ) == 15
);

PDX_COMPILE_ASSERT
(
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_1 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 1 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_2 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 2 &&  
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_3 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 3 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_4 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 4 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_5 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 5 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_6 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 6 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_7 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 7 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_8 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 8 && 
	( IDM_FILE_QUICK_LOAD_STATE_SLOT_9 - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST ) == 9
);

PDX_COMPILE_ASSERT
(
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_1 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 1 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_2 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 2 &&  
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_3 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 3 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_4 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 4 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_5 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 5 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_6 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 6 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_7 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 7 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_8 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 8 && 
	( IDM_FILE_QUICK_SAVE_STATE_SLOT_9 - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT ) == 9
);

PDX_COMPILE_ASSERT
(
	( IDM_FILE_RECENT_1 - IDM_FILE_RECENT_0 ) == 1 && 
	( IDM_FILE_RECENT_2 - IDM_FILE_RECENT_0 ) == 2 &&  
	( IDM_FILE_RECENT_3 - IDM_FILE_RECENT_0 ) == 3 && 
	( IDM_FILE_RECENT_4 - IDM_FILE_RECENT_0 ) == 4 && 
	( IDM_FILE_RECENT_5 - IDM_FILE_RECENT_0 ) == 5 && 
	( IDM_FILE_RECENT_6 - IDM_FILE_RECENT_0 ) == 6 && 
	( IDM_FILE_RECENT_7 - IDM_FILE_RECENT_0 ) == 7 && 
	( IDM_FILE_RECENT_8 - IDM_FILE_RECENT_0 ) == 8 && 
	( IDM_FILE_RECENT_9 - IDM_FILE_RECENT_0 ) == 9
);

PDX_COMPILE_ASSERT
(
    ( IDM_VIEW_WINDOWSIZE_1X - IDM_VIEW_WINDOWSIZE_MAX ) == 1 &&
	( IDM_VIEW_WINDOWSIZE_2X - IDM_VIEW_WINDOWSIZE_MAX ) == 2 && 
	( IDM_VIEW_WINDOWSIZE_4X - IDM_VIEW_WINDOWSIZE_MAX ) == 3 &&
	( IDM_VIEW_WINDOWSIZE_8X - IDM_VIEW_WINDOWSIZE_MAX ) == 4
);

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

#define NST_WINDOWSTYLE    (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME)
#define NST_WINDOWSTYLE_EX (WS_EX_ACCEPTFILES|WS_EX_CLIENTEDGE)

////////////////////////////////////////////////////////////////////////////////////////
// constructor
////////////////////////////////////////////////////////////////////////////////////////

APPLICATION::APPLICATION(HINSTANCE h,const CHAR* const CmdLine,const INT iCmdShow)
: 
hInstance            (h),
hWnd                 (NULL),
ThreadPriority       (THREAD_PRIORITY_NORMAL),
hCursor              (::LoadCursor(NULL,IDC_ARROW)),
menu                 (::LoadMenu(h,MAKEINTRESOURCE(IDR_MENU))),
hAccel               (::LoadAccelerators(h,MAKEINTRESOURCE(IDR_ACCELERATOR))),
active               (FALSE),
UseZapper            (FALSE),
ShowFPS              (FALSE),
windowed             (TRUE),
InBackground         (FALSE),
ExitSuccess          (FALSE),
AutoSelectController (FALSE),
NesMode              (NES::MODE_AUTO),
WindowVisible        (TRUE),
StatusBar            (NULL),
TimerManager		 (NULL),
GraphicManager		 (NULL),
SoundManager		 (NULL),
InputManager		 (NULL),
FileManager			 (NULL),
FdsManager			 (NULL),
GameGenieManager	 (NULL),
SaveStateManager	 (NULL),
MovieManager		 (NULL),
VsDipSwitchManager	 (NULL),
preferences			 (NULL),
log					 (NULL),
RomInfo				 (NULL),
HelpManager			 (NULL),
UserInputManager	 (NULL),
launcher             (NULL),
NetplayManager       (NULL)
{
	::InitCommonControls();

	{
		WNDCLASSEX wndcls;
		PDXMemZero( wndcls );

		wndcls.cbSize        = sizeof(wndcls);
		wndcls.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
		wndcls.lpfnWndProc   = WndProc;
		wndcls.hInstance     = hInstance;
		wndcls.hCursor       = ::LoadCursor(NULL,IDC_ARROW);
		wndcls.hbrBackground = HBRUSH(::GetStockObject(NULL_BRUSH));
		wndcls.lpszClassName = NST_CLASS_NAME;
		wndcls.hIcon         = 
		wndcls.hIconSm       = ::LoadIcon(hInstance,MAKEINTRESOURCE(IDI_WINDOW));

		if (!RegisterClassEx(&wndcls))
			throw ("RegisterClassEx() failed!");
	}

	{
		RECT rect;
		::SetRect( &rect, 0, 0, 256*2, 224*2 );
		
		::AdjustWindowRectEx
		( 
	       	&rect, 
			NST_WINDOWSTYLE, 
			TRUE,
			NST_WINDOWSTYLE_EX
		);

		hWnd = CreateWindowEx
		(
	     	NST_WINDOWSTYLE_EX,
			NST_CLASS_NAME,
			NST_WINDOW_NAME,
			NST_WINDOWSTYLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			0,
			menu,
			hInstance,
			NULL
		);
	}

	if (!hWnd)
		throw ("CreateWindowEx() failed!");

	StatusBar          = new STATUSBAR;
	TimerManager	   = new TIMERMANAGER;
	GraphicManager	   = new GRAPHICMANAGER;
	SoundManager       = new SOUNDMANAGER;
	InputManager       = new INPUTMANAGER;
	FileManager        = new FILEMANAGER;
	FdsManager         = new FDSMANAGER;
	GameGenieManager   = new GAMEGENIEMANAGER;
	SaveStateManager   = new SAVESTATEMANAGER;
	MovieManager       = new MOVIEMANAGER;
	VsDipSwitchManager = new VSDIPSWITCHMANAGER;
	preferences        = new PREFERENCES;
	log                = new LOGFILE;
	RomInfo            = new ROMINFO;
	HelpManager        = new HELPMANAGER;
	UserInputManager   = new USERINPUTMANAGER;
	launcher           = new LAUNCHER;
	NetplayManager     = new NETPLAYMANAGER;

	AcceleratorEnabled = bool(hAccel);

	SelectPort[0] = IDM_MACHINE_PORT1_PAD1;
	SelectPort[1] = IDM_MACHINE_PORT2_PAD2;
	SelectPort[2] = IDM_MACHINE_PORT3_UNCONNECTED;
	SelectPort[3] = IDM_MACHINE_PORT4_UNCONNECTED;
	SelectPort[4] = IDM_MACHINE_EXPANSION_UNCONNECTED;

	PDXSTRING CmdLineFile;

	{
		CONFIGFILE file;
		CONFIGFILE* ConfigFile = InitConfigFile( file, CFG_LOAD ) ? &file : NULL;

		{
			CMDLINEPARSER CmdLineParser;

			const BOOL HaveCommands = CmdLineParser.Parse
			( 
		     	CmdLine,
				strlen(CmdLine),
				&file, 
				IDS_CMDLINE_PARSE_ERROR,
				TRUE
			);

			if (HaveCommands)
				ConfigFile = &file;

			if (CmdLineParser.GetStartupFile().Length())
				CmdLineFile = CmdLineParser.GetStartupFile();
		}

		{
			UINT factor = 1;
			BOOL MenuCheck = TRUE;
			BOOL BarCheck = TRUE;
			BOOL OnTopCheck = FALSE;
			BOOL SpritesCheck = FALSE;
			BOOL NsfInBackgroundCheck = TRUE;

			if (ConfigFile)
			{
				const PDXSTRING& string = file["window size"];
	
     				 if (string == "1x") factor = 0;
				else if (string == "3x") factor = 2;
				else if (string == "4x") factor = 3;

				if ( file[ "view show status bar"          ] == "no"  ) BarCheck = FALSE;
				if ( file[ "view show fps"                 ] == "yes" ) ShowFPS = TRUE;
				if ( file[ "view show menu"                ] == "no"  ) MenuCheck = FALSE;
				if ( file[ "view show on top"              ] == "yes" ) OnTopCheck = TRUE;
				if ( file[ "video unlimited sprites"       ] == "yes" ) SpritesCheck = TRUE;
				if ( file[ "preferences nsf in background" ] == "no"  ) NsfInBackgroundCheck = FALSE;
			}

			{
				NES::IO::GFX::CONTEXT context;
				nes.GetGraphicContext( context );
				context.InfiniteSprites = SpritesCheck;
				nes.SetGraphicContext( context );
			}

			if (BarCheck)
				StatusBar->Create( hInstance, hWnd );

			menu.Check( IDM_MACHINE_OPTIONS_UNLIMITEDSPRITES, SpritesCheck );
			menu.Check( IDM_NSF_OPTIONS_PLAYINBACKGROUND, NsfInBackgroundCheck );
			menu.Check( IDM_VIEW_STATUSBAR, BarCheck );
			menu.Check( IDM_VIEW_MENU, MenuCheck );
			menu.Check( IDM_VIEW_ON_TOP, OnTopCheck );
			menu.Check( IDM_VIEW_FPS, ShowFPS );

			menu.Enable( IDM_VIEW_FPS, BarCheck );
			menu.Enable( IDM_FILE_NETPLAY, NetplayManager->IsSupported() );

			if (ShowFPS) 
				ShowFPS = BarCheck;

			UpdateWindowSizes
			( 
		       	GraphicManager->GetDisplayWidth(), 
				GraphicManager->GetDisplayHeight() 
			);

			OnWindowSize( factor, TRUE );

			::GetWindowRect( hWnd, &rcWindow );
			rcDesktop = rcWindow;
			::GetClientRect( hWnd, &rcDesktopClient );
			GetScreenRect( rcScreen );
		}

		TimerManager->Create       ( ConfigFile );
		FileManager->Create        ( ConfigFile );
		GraphicManager->Create     ( ConfigFile );
		SoundManager->Create       ( ConfigFile );
		InputManager->Create       ( ConfigFile );
		FdsManager->Create         ( ConfigFile );
		preferences->Create        ( ConfigFile );
		GameGenieManager->Create   ( ConfigFile );
		SaveStateManager->Create   ( ConfigFile );
		NetplayManager->Create     ( ConfigFile );
		MovieManager->Create       ( ConfigFile );
		VsDipSwitchManager->Create ( ConfigFile );
		log->Create                ( ConfigFile );
		RomInfo->Create            ( ConfigFile );
		UserInputManager->Create   ( ConfigFile );
		launcher->Create           ( ConfigFile );
	}
	
	UpdateRecentFiles();
	OnAutoSelectController();
	UpdateWindowItems();
	GraphicManager->SwitchToWindowed( rcScreen );

	if (preferences->StartUpFullScreen())
	{
		SwitchScreen();
	}
	else
	{
		::ShowWindow( hWnd, iCmdShow );
	}

	if (CmdLineFile.Length())
		OnOpen( FILE_INPUT, PDX_CAST(const VOID*,CmdLineFile.String()) );
}

////////////////////////////////////////////////////////////////////////////////////////
// destructor
////////////////////////////////////////////////////////////////////////////////////////

APPLICATION::~APPLICATION()
{
	nes.Power( FALSE );
	
	{
		CONFIGFILE file;
		CONFIGFILE* ConfigFile = NULL;

		if (ExitSuccess && preferences && preferences->SaveSettings())
		{
			ConfigFile = &file;

			if (GraphicManager)
			{
				switch (GetAspectRatio())
				{				
		     		case 0:  file["window size"] = "1x"; break;
		       		case 2:  file["window size"] = "3x"; break;
		       		case 3:  file["window size"] = "4x"; break;
		    		default: file["window size"] = "2x"; break;
				}
			}

			file[ "view show on top"              ] = ( menu.IsChecked( IDM_VIEW_ON_TOP                      ) ? "yes" : "no" );
			file[ "view show menu"                ] = ( menu.IsChecked( IDM_VIEW_MENU                        ) ? "yes" : "no" );
			file[ "view show status bar"          ] = ( menu.IsChecked( IDM_VIEW_STATUSBAR                   ) ? "yes" : "no" );
			file[ "view show fps"                 ] = ( menu.IsChecked( IDM_VIEW_FPS                         ) ? "yes" : "no" );
			file[ "video unlimited sprites"       ] = ( menu.IsChecked( IDM_MACHINE_OPTIONS_UNLIMITEDSPRITES ) ? "yes" : "no" );
			file[ "preferences nsf in background" ] = ( menu.IsChecked( IDM_NSF_OPTIONS_PLAYINBACKGROUND     ) ? "yes" : "no" );
		}

		if ( TimerManager     ) TimerManager->Destroy     ( ConfigFile );
		if ( FileManager      ) FileManager->Destroy      ( ConfigFile );
		if ( GraphicManager   ) GraphicManager->Destroy   ( ConfigFile );
		if ( SoundManager     ) SoundManager->Destroy     ( ConfigFile );
		if ( InputManager     ) InputManager->Destroy     ( ConfigFile );
		if ( FdsManager       ) FdsManager->Destroy       ( ConfigFile );
		if ( GameGenieManager ) GameGenieManager->Destroy ( ConfigFile );
		if ( NetplayManager   ) NetplayManager->Destroy   ( ConfigFile );
		if ( preferences      ) preferences->Destroy      ( ConfigFile );
		if ( launcher         ) launcher->Destroy         ( ConfigFile );

		if (ConfigFile)
			InitConfigFile( file, CFG_SAVE );
	}

	if (log)
		log->Close( !ExitSuccess || preferences->SaveLogFile() );

	if (!hWnd || !::GetMenu( hWnd ))
		menu.Destroy();

	if (hWnd)
	{
		::DestroyWindow( hWnd );
		hWnd = NULL;
	}

	NST_DELETE( NetplayManager     );
	NST_DELETE( launcher           );
	NST_DELETE( StatusBar          );
	NST_DELETE( TimerManager       );
	NST_DELETE( GraphicManager     );
	NST_DELETE( SoundManager       );
	NST_DELETE( InputManager       );
	NST_DELETE( FileManager        );      
	NST_DELETE( FdsManager         );        
	NST_DELETE( GameGenieManager   );
	NST_DELETE( SaveStateManager   );
	NST_DELETE( MovieManager       );   
	NST_DELETE( VsDipSwitchManager );
	NST_DELETE( preferences        );
	NST_DELETE( log                );        
	NST_DELETE( RomInfo            );
	NST_DELETE( HelpManager        );
	NST_DELETE( UserInputManager   );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL APPLICATION::InitConfigFile(CONFIGFILE& cfg,const CFG_OP op)
{
	PDXSTRING filename;

	UTILITIES::GetExeDir( filename );
	filename << "Nestopia.cfg";

	BOOL result;

	switch (op)
	{
    	case CFG_LOAD: 
				
			result = cfg.Load( filename );

			log->Output
			( 
     			(result ? "APPLICATION: loading settings from \"" : "APPLICATION: configuration file \""), 
				filename, 
				(result ? "\"" : "\" not present. using default settings")
			);
			return result;

		case CFG_SAVE: 
			
			result = cfg.Save( filename );

			log->Output
			(
     			(result ? "APPLICATION: saved settings to \"" : "APPLICATION: failed to save settings to file \""),
				filename,
				"\""
			);
			return result;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
// main loop
////////////////////////////////////////////////////////////////////////////////////////

INT APPLICATION::Run()
{
	MSG msg;	
	msg.message = WM_NULL;
	::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );

	while (msg.message != WM_QUIT)
	{
		const BOOL running = IsRunning();

		if ((running ? ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) : ::GetMessage( &msg, NULL, 0, 0 )))
		{
			const BOOL translate =
			(
		     	!launcher->IsDlgMessage( msg ) &&
				(!AcceleratorEnabled || !::TranslateAccelerator( hWnd, hAccel, &msg ))
			);

			if (translate)
			{
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
			}
		}
		else if (running)
		{
			if (nes.IsImage())
			{
				ExecuteImage();
			}
			else
			{
				ExecuteNsf();
			}
		}
	}

	ExitSuccess = TRUE;

	return INT(msg.wParam);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::ExecuteImage()
{
	PDX_ASSERT( nes.IsImage() );		

	NES::IO::INPUT* input;

	const BOOL GfxOut = GraphicManager->TestCooperativeLevel() && WindowVisible;

	for (UINT i=TimerManager->NumFrameSkips(); i; --i)
	{
		InputManager->Poll();			
		input = InputManager->GetFormat();

		if (NetplayManager->IsConnected())
			NetplayManager->Update( *input );

		nes.Execute
		( 
			NULL, 
			SoundManager->GetFormat(), 
			input
		);
	}

	BOOL cleared;

	if (!windowed && GfxOut)
		cleared = GraphicManager->ClearScreen( FALSE );

	InputManager->Poll();
	input = InputManager->GetFormat();

	if (NetplayManager->IsConnected())
		NetplayManager->Update( *input );

	if (GfxOut)
	{
		if (!windowed && !cleared)
			GraphicManager->ClearScreen( TRUE );

		nes.Execute
		( 
			GraphicManager->GetFormat(), 
			SoundManager->GetFormat(), 
			input 
		);

		if (ScreenMsg.Length() && !windowed)
			GraphicManager->DisplayMsg( &ScreenMsg );

		if (ShowFPS)
			DisplayFPS( TRUE );

		TimerManager->Synchronize( TRUE, TRUE );
		GraphicManager->Present();
	}
	else
	{
		nes.Execute
		( 
			NULL, 
			SoundManager->GetFormat(), 
			input 
		);

		TimerManager->Synchronize( FALSE, FALSE );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::ExecuteNsf()
{
	PDX_ASSERT( nes.IsNsf() );		
		
	nes.Execute
	( 
       	NULL, 
		SoundManager->GetFormat(), 
		NULL 
	);

	TimerManager->Synchronize( FALSE, FALSE );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////
													 
VOID APPLICATION::SetThreadPriority(INT priority)
{
	if (preferences->PriorityControl())
	{
		if (windowed && TimerManager->NoSleeping() && (priority == THREAD_PRIORITY_HIGHEST || priority == THREAD_PRIORITY_ABOVE_NORMAL))
			priority = THREAD_PRIORITY_NORMAL;

		if (ThreadPriority != priority)
		{
			// NT can't be trusted.. or maybe it can but I don't want to gamble
			const DWORD dwVersion = (::GetVersion() & 0x800000FFUL);

			if (dwVersion != 3 && dwVersion != 4)
				::SetThreadPriority( ::GetCurrentThread(), (ThreadPriority = priority) );
		}
	}
	else
	{
		ThreadPriority = preferences->GetDefaultPriority();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// static window callback procedure
////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK APPLICATION::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return application.MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////////////
// window callback procedure
////////////////////////////////////////////////////////////////////////////////////////

LRESULT APPLICATION::MsgProc(const HWND hWnd,const UINT uMsg,const WPARAM wParam,const LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SETCURSOR:

			if (LOWORD(lParam) == HTCLIENT)
			{
				::SetCursor( hCursor );
				return TRUE;
			}
			break;

		case WM_NCHITTEST:
			break;

		case WM_MOUSEMOVE:

			OnMouseMove( lParam );
			return 0;

		case WM_LBUTTONDOWN:

			OnLeftMouseButtonDown( lParam );
			return 0;

		case WM_LBUTTONUP:

			OnLeftMouseButtonUp();
			return 0;

		case WM_RBUTTONDOWN:

			OnToggleMenu();
			return 0;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:

			OnPaint( TRUE );
			return 0;

		case WM_COMMAND:

			if (OnCommand( wParam ))
				return 0;

			break;

		case WM_WINDOWPOSCHANGED:

			OnSizeMove( lParam );
			break;

		case WM_SYSCOMMAND:

			if (OnSysCommand( wParam ))
				return 0;

			break;

		case WM_EXITSIZEMOVE:
		case WM_EXITMENULOOP:

			OnActive();
			return 0;

		case WM_ENTERSIZEMOVE:

			OnInactive( TRUE );
			return 0;

		case WM_ENTERMENULOOP:

			OnInactive( TRUE );
			UpdateDynamicMenuItems();
			return 0;

		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
			
			SoundManager->Clear();
			break;

		case WM_ACTIVATE:

			OnActivate( wParam );
			return 0;

		case WM_COPYDATA:

			if (OnCopyData( lParam ))
				return 0;

			break;

		case WM_DROPFILES:

			OnDropFiles( wParam );
			return 0;

		case WM_POWERBROADCAST:

			switch (wParam)
			{
     			case PBT_APMQUERYSUSPEND:  OnInactive( TRUE ); return TRUE;
				case PBT_APMRESUMESUSPEND: OnActive(); return TRUE;
			}
			break;
		
		case WM_CLOSE:

			OnCloseWindow();
			return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL APPLICATION::OnCommand(const WPARAM wParam)
{
	const UINT idm = LOWORD(wParam);

	switch (idm)
	{
		case IDM_FILE_OPEN:                        OnOpen( FILE_ALL );                 return TRUE;
		case IDM_FILE_CLOSE:                       OnClose();                          return TRUE;
		case IDM_FILE_LOAD_NSP:                    OnLoadNsp();                        return TRUE;
		case IDM_FILE_LOAD_NST:                    OnLoadState();                      return TRUE;
		case IDM_FILE_SAVE_NSP:                    OnSaveNsp();                        return TRUE;
		case IDM_FILE_SAVE_NST:                    OnSaveState();                      return TRUE;
		case IDM_FILE_NETPLAY:                     OnNetplay();                        return TRUE;
		case IDM_FILE_LAUNCHER:                    OnLauncher();                       return TRUE;
		case IDM_FILE_QUIT:			               OnCloseWindow();                    return TRUE;
		case IDM_FILE_SAVE_SCREENSHOT:             OnSaveScreenShot();                 return TRUE;
		case IDM_FILE_SOUND_CAPTURE_FILE:		  
		case IDM_FILE_SOUND_CAPTURE_RECORD:		  
		case IDM_FILE_SOUND_CAPTURE_STOP:		  
		case IDM_FILE_SOUND_CAPTURE_RESET:         OnSoundRecorder( idm );             return TRUE;
		case IDM_FILE_MOVIE_FILE:                  MovieManager->StartDialog();        return TRUE;
		case IDM_FILE_MOVIE_PLAY:                  MovieManager->Play();               return TRUE;
		case IDM_FILE_MOVIE_RECORD:                MovieManager->Record();             return TRUE;
		case IDM_FILE_MOVIE_STOP:                  MovieManager->Stop();               return TRUE;
		case IDM_FILE_MOVIE_REWIND:                MovieManager->Rewind();             return TRUE;
		case IDM_FILE_MOVIE_FORWARD:               MovieManager->Forward();            return TRUE;
		case IDM_MACHINE_POWER_ON:		           OnPower( TRUE  );                   return TRUE;
		case IDM_MACHINE_POWER_OFF:                OnPower( FALSE );                   return TRUE;
		case IDM_MACHINE_RESET_SOFT:               OnReset( FALSE );                   return TRUE;
		case IDM_MACHINE_RESET_HARD:               OnReset( TRUE  );                   return TRUE;
		case IDM_MACHINE_AUTOSELECTCONTROLLER:     OnAutoSelectController();           return TRUE;                                
		case IDM_MACHINE_MODE_AUTO:                                    
		case IDM_MACHINE_MODE_NTSC:		  
		case IDM_MACHINE_MODE_PAL:	               OnMode( idm );                      return TRUE;
		case IDM_MACHINE_PAUSE:	                   OnPause();                          return TRUE;
		case IDM_MACHINE_OPTIONS_UNLIMITEDSPRITES: OnUnlimitedSprites();               return TRUE;
		case IDM_MACHINE_OPTIONS_DIPSWITCHES:      VsDipSwitchManager->StartDialog();  return TRUE;
		case IDM_FDS_EJECT_DISK:                   OnFdsEjectDisk();                   return TRUE;
		case IDM_FDS_SIDE_A:                      
		case IDM_FDS_SIDE_B:                       OnFdsSide( idm );                   return TRUE;
		case IDM_FDS_OPTIONS:                      FdsManager->StartDialog();          return TRUE;
		case IDM_NSF_PLAY:                         OnNsfCommand( NES::IO::NSF::PLAY ); return TRUE;
		case IDM_NSF_STOP:                         OnNsfCommand( NES::IO::NSF::STOP ); return TRUE;
		case IDM_NSF_NEXT:                         OnNsfCommand( NES::IO::NSF::NEXT ); return TRUE;
		case IDM_NSF_PREV:                         OnNsfCommand( NES::IO::NSF::PREV ); return TRUE;
		case IDM_NSF_OPTIONS_PLAYINBACKGROUND:     OnNsfInBackground();                return TRUE;
		case IDM_VIEW_ROM_INFO:                    RomInfo->StartDialog();             return TRUE;
		case IDM_VIEW_LOGFILE:                     log->StartDialog();                 return TRUE;
		case IDM_VIEW_SWITCH_SCREEN:               SwitchScreen();                     return TRUE;
		case IDM_VIEW_WINDOWSIZE_MAX:              OnWindowSize( UINT_MAX );           return TRUE;
		case IDM_VIEW_WINDOWSIZE_1X:               OnWindowSize( 0 );                  return TRUE;
		case IDM_VIEW_WINDOWSIZE_2X:               OnWindowSize( 1 );                  return TRUE;
		case IDM_VIEW_WINDOWSIZE_4X:               OnWindowSize( 2 );                  return TRUE;
		case IDM_VIEW_WINDOWSIZE_8X:               OnWindowSize( 3 );                  return TRUE;
		case IDM_VIEW_STATUSBAR:                   OnToggleStatusBar();                return TRUE;
		case IDM_VIEW_MENU:                        OnToggleMenu();                     return TRUE;
		case IDM_VIEW_FPS:                         OnToggleFPS();                      return TRUE;
		case IDM_VIEW_ON_TOP:                      OnTop();                            return TRUE;
		case IDM_OPTIONS_PREFERENCES:              preferences->StartDialog();         return TRUE;
		case IDM_OPTIONS_VIDEO:                    GraphicManager->StartDialog();      return TRUE;
		case IDM_OPTIONS_SOUND:                    SoundManager->StartDialog();        return TRUE;
		case IDM_OPTIONS_INPUT:                    InputManager->StartDialog();        return TRUE;
		case IDM_OPTIONS_TIMING:                   TimerManager->StartDialog();        return TRUE;
		case IDM_OPTIONS_PATHS:                    FileManager->StartDialog();         return TRUE;
		case IDM_OPTIONS_GAME_GENIE:               GameGenieManager->StartDialog();    return TRUE;
		case IDM_OPTIONS_AUTO_SAVE:                SaveStateManager->StartDialog();    return TRUE;
		case IDM_HELP_ABOUT:                      
		case IDM_HELP_LICENCE:                     OnHelp( idm );                      return TRUE;
	}

	if (idm >= IDM_MACHINE_PORT1_UNCONNECTED && idm <= IDM_MACHINE_EXPANSION_FAMILYBASICKEYBOARD)
	{
		OnPort( idm );
		return TRUE;
	}

	if (idm >= IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST && idm <= IDM_FILE_QUICK_LOAD_STATE_SLOT_9)
	{
		OnLoadStateSlot( idm );
		return TRUE;
	}

	if (idm >= IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT && idm <= IDM_FILE_QUICK_SAVE_STATE_SLOT_9)
	{
		OnSaveStateSlot( idm );
		return TRUE;
	}

	if (idm >= IDM_FILE_RECENT_0 && idm <= IDM_FILE_RECENT_9)
	{
		OnRecent( idm );
		return TRUE;
	}

	if (idm >= IDM_FDS_INSERT_DISK_1 && idm <= IDM_FDS_INSERT_DISK_16)
	{
		OnFdsInsertDisk( idm );
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

#define NST_CASE_(i,j,k) case i: port = SelectPort + j; nes.ConnectController( j, k ); break

VOID APPLICATION::OnPort(const UINT wParam)
{
	PDX_ASSERT( wParam >= IDM_MACHINE_PORT1_UNCONNECTED && wParam <= IDM_MACHINE_EXPANSION_FAMILYBASICKEYBOARD );

	UINT* port = NULL;

	switch (wParam)
	{
	    NST_CASE_( IDM_MACHINE_PORT1_UNCONNECTED,             0, NES::CONTROLLER_UNCONNECTED );
       	NST_CASE_( IDM_MACHINE_PORT1_PAD1,		              0, NES::CONTROLLER_PAD1        );
     	NST_CASE_( IDM_MACHINE_PORT1_PAD2,		              0, NES::CONTROLLER_PAD2        );
     	NST_CASE_( IDM_MACHINE_PORT1_PAD3,		              0, NES::CONTROLLER_PAD3        );
       	NST_CASE_( IDM_MACHINE_PORT1_PAD4,	                  0, NES::CONTROLLER_PAD4        );
       	NST_CASE_( IDM_MACHINE_PORT1_ZAPPER,	              0, NES::CONTROLLER_ZAPPER      );
    	NST_CASE_( IDM_MACHINE_PORT1_PADDLE,	              0, NES::CONTROLLER_PADDLE      );
		NST_CASE_( IDM_MACHINE_PORT1_POWERPAD,	              0, NES::CONTROLLER_POWERPAD    );
       	NST_CASE_( IDM_MACHINE_PORT2_UNCONNECTED,             1, NES::CONTROLLER_UNCONNECTED );
     	NST_CASE_( IDM_MACHINE_PORT2_PAD1,		              1, NES::CONTROLLER_PAD1        );
     	NST_CASE_( IDM_MACHINE_PORT2_PAD2,		              1, NES::CONTROLLER_PAD2        );
     	NST_CASE_( IDM_MACHINE_PORT2_PAD3,	  	              1, NES::CONTROLLER_PAD3        );
     	NST_CASE_( IDM_MACHINE_PORT2_PAD4,	                  1, NES::CONTROLLER_PAD4        );
     	NST_CASE_( IDM_MACHINE_PORT2_ZAPPER,	              1, NES::CONTROLLER_ZAPPER      );
     	NST_CASE_( IDM_MACHINE_PORT2_PADDLE,	              1, NES::CONTROLLER_PADDLE      );
		NST_CASE_( IDM_MACHINE_PORT2_POWERPAD,	              1, NES::CONTROLLER_POWERPAD    );
     	NST_CASE_( IDM_MACHINE_PORT3_UNCONNECTED,             2, NES::CONTROLLER_UNCONNECTED );
     	NST_CASE_( IDM_MACHINE_PORT3_PAD1,		              2, NES::CONTROLLER_PAD1        );
     	NST_CASE_( IDM_MACHINE_PORT3_PAD2,		              2, NES::CONTROLLER_PAD2        );
     	NST_CASE_( IDM_MACHINE_PORT3_PAD3,		              2, NES::CONTROLLER_PAD3        );
    	NST_CASE_( IDM_MACHINE_PORT3_PAD4,	                  2, NES::CONTROLLER_PAD4        );
     	NST_CASE_( IDM_MACHINE_PORT4_UNCONNECTED,             3, NES::CONTROLLER_UNCONNECTED );
     	NST_CASE_( IDM_MACHINE_PORT4_PAD1,		              3, NES::CONTROLLER_PAD1        );
     	NST_CASE_( IDM_MACHINE_PORT4_PAD2,		              3, NES::CONTROLLER_PAD2        );
     	NST_CASE_( IDM_MACHINE_PORT4_PAD3,		              3, NES::CONTROLLER_PAD3        );
    	NST_CASE_( IDM_MACHINE_PORT4_PAD4,	                  3, NES::CONTROLLER_PAD4        );
		NST_CASE_( IDM_MACHINE_EXPANSION_UNCONNECTED,	      4, NES::CONTROLLER_UNCONNECTED );
     	NST_CASE_( IDM_MACHINE_EXPANSION_FAMILYBASICKEYBOARD, 4, NES::CONTROLLER_KEYBOARD    );
	}

	if (port)
	{
		menu.Uncheck( *port );
		menu.Check( *port = wParam );

		if (nes.IsAnyControllerConnected(NES::CONTROLLER_KEYBOARD))
		{
			AcceleratorEnabled = FALSE;
			UseZapper = FALSE;
		}
		else
		{
			AcceleratorEnabled = bool(hAccel);
			UseZapper = nes.IsAnyControllerConnected(NES::CONTROLLER_ZAPPER);
		}

		RefreshCursor();
	}
}														 

#undef NST_CASE_

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnAutoSelectController()
{
	AutoSelectController = !AutoSelectController;
	menu.Check( IDM_MACHINE_AUTOSELECTCONTROLLER, AutoSelectController );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK APPLICATION::OnScreenMsgEnd(HWND hWnd,UINT,UINT_PTR,DWORD)
{
	PDX_ASSERT( application.IsInstanced() );

	::KillTimer( hWnd, TIMER_ID_SCREEN_MSG );
	
	application.ScreenMsg.Clear();

	if (application.windowed)
	{
		if (application.StatusBar)
			application.StatusBar->DisplayMsg( NULL );
	}
	else
	{
		if (application.IsPassive())
			application.OnPaint();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

INT APPLICATION::GetDesiredPriority() const
{
	if (IsRunning())
		return (nes.IsNsf() ? THREAD_PRIORITY_ABOVE_NORMAL : THREAD_PRIORITY_HIGHEST);

	return THREAD_PRIORITY_NORMAL;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::DisplayFPS(BOOL update)
{
	DOUBLE fps;

	if (update)
	{
		update = TimerManager->CalculateFPS( fps );
	}
	else
	{
     	update = FALSE;
    	fps = TimerManager->GetFPS();
	}

	if (windowed)
	{
		PDX_ASSERT( StatusBar->IsEnabled() );

		if (update)
			StatusBar->DisplayFPS( TRUE, fps );
	}
	else if (!IsPassive())
	{
		GraphicManager->DisplayFPS( fps );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL APPLICATION::OnSysCommand(const WPARAM wParam)
{
	const WORD wCommand = LOWORD(wParam);

	if (wCommand == SC_SCREENSAVE || wCommand == SC_MONITORPOWER)
	{
		if (!windowed || IsRunning())
			return TRUE;
			
		OnInactive( TRUE );
	}
	else
	{
		SoundManager->Clear();

		if (!windowed)
		{
			switch (wCommand)
			{
       			case SC_MOVE:
				case SC_SIZE:
					return TRUE;
			}
		}
	}
  
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL APPLICATION::OnCopyData(const LPARAM lParam)
{
	PDX_ASSERT( lParam );

	if (lParam)
	{
		const COPYDATASTRUCT& cds = *PDX_CAST(const COPYDATASTRUCT*,lParam);

		switch (cds.dwData)
		{
			case NST_WM_OPEN_FILE:
     		
				if (cds.cbData && cds.lpData)
				{
					const PDXSTRING filename
					( 
				     	PDX_CAST(const CHAR*,cds.lpData), 
						PDX_CAST(const CHAR*,cds.lpData) + cds.cbData
					);

					OnOpen( FILE_INPUT, PDX_CAST(const VOID*,filename.String()) );
				}
				return TRUE;

			case NST_WM_OPEN_ZIPPED_FILE:

				if (cds.cbData == sizeof(PDXPAIR<PDXSTRING,PDXSTRING>) && cds.lpData)
					OnOpen( FILE_ZIPPED, cds.lpData );

				return TRUE;
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnDropFiles(WPARAM wParam)
{
	PDX_ASSERT( wParam );

	PDXSTRING filename;
	filename.Resize(MAX_PATH);
	filename.Front() = '\0';

	if (::DragQueryFile( HDROP(wParam), 0, filename.Begin(), MAX_PATH ))
	{
		filename.Validate();

		if (filename.Length())
			OnOpen( FILE_INPUT, PDX_CAST(const VOID*,filename.String()) );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnPause()
{
	const BOOL pause = !nes.IsPaused();
	nes.Pause( pause );

	if (pause)
	{
		SoundManager->Clear();
		OnPaint();
	}

	menu.Check( IDM_MACHINE_PAUSE, pause );
	StartScreenMsg( 1500, pause ? "Paused.." : "Resumed.." );
	SetThreadPriority( GetDesiredPriority() );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnUnlimitedSprites()
{
	NES::IO::GFX::CONTEXT context;
	nes.GetGraphicContext( context );

	context.InfiniteSprites = menu.IsUnchecked( IDM_MACHINE_OPTIONS_UNLIMITEDSPRITES );
	menu.Check( IDM_MACHINE_OPTIONS_UNLIMITEDSPRITES, context.InfiniteSprites );

	StartScreenMsg( 1500, "Unlimited sprites ", context.InfiniteSprites ? "enabled.." : "disabled.." );
	nes.SetGraphicContext( context );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnNsfInBackground()
{
	menu.ToggleCheck( IDM_NSF_OPTIONS_PLAYINBACKGROUND );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::RefreshCursor(const BOOL force)
{
	if (!force && UseZapper && nes.IsOn() && nes.IsImage())
	{
		hCursor = ::LoadCursor( NULL, IDC_CROSS );
		::SetCursor( hCursor );
		while (::ShowCursor( TRUE ) <= -1);
	}
	else if (force || windowed || ::GetMenu( hWnd ))
	{
		hCursor = ::LoadCursor( NULL, IDC_ARROW );
		::SetCursor( hCursor );
		while (::ShowCursor( TRUE ) <= -1);
	}
	else
	{
		while (::ShowCursor( FALSE ) >= 0);
		::SetCursor( NULL );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnNsfCommand(const NES::IO::NSF::OP op)
{
	NES::IO::NSF::CONTEXT context;
	context.op = op;

	if (PDX_SUCCEEDED(nes.SetNsfContext( context )))
	{
		if (op == NES::IO::NSF::NEXT || op == NES::IO::NSF::PREV)
		{
			if (PDX_SUCCEEDED(nes.GetNsfContext(context)))
			{
				NsfInfo.song = "Song: ";
				NsfInfo.song << (context.song+1) << "/" << context.NumSongs;
				OnPaint();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnHelp(const UINT wParam)
{
	SoundManager->Clear();

	switch (wParam)
	{
     	case IDM_HELP_ABOUT:   HelpManager->StartAboutDialog();   break;
     	case IDM_HELP_LICENCE: HelpManager->StartLicenceDialog(); break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnActive()
{
	active = TRUE;
	SetThreadPriority( GetDesiredPriority() );
	InBackground = FALSE;
	InputManager->AcquireDevices();
	TimerManager->Update();
	SoundManager->Start();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnInactive(const BOOL force)
{
	InBackground = TRUE;
	SoundManager->Clear();

	if (force || (!preferences->RunInBackground() && !(nes.IsNsf() && nes.IsOn() && menu.IsChecked( IDM_NSF_OPTIONS_PLAYINBACKGROUND ))))
	{
		active = FALSE;
		SoundManager->Stop();
		TimerManager->Update();
		InputManager->UnacquireDevices();
		SetThreadPriority( THREAD_PRIORITY_NORMAL );
	}
	else
	{
		SetThreadPriority( IsRunning() ? THREAD_PRIORITY_ABOVE_NORMAL : THREAD_PRIORITY_NORMAL );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnOpen(const FILETYPE FileType,const VOID* const param)
{
	SoundManager->Clear();

	if (::IsIconic( hWnd ))
		::ShowWindow( hWnd, SW_RESTORE );

	::SetForegroundWindow( hWnd );

	if (preferences->ConfirmReset() && nes.IsOn())
	{
		if (!UI::MsgQuestion( IDS_MACHINE_DETACH_IMAGE, IDS_MACHINE_CONFIRM_DETACH_IMAGE ))
			return;
	}

	const BOOL WasPAL = nes.IsPAL();
	PDXRESULT result = PDX_FAILURE;

	switch (FileType)
	{
   		case FILE_ALL:   
			
			result = FileManager->Load
			(
       			param ? FILEMANAGER::COMMAND_RECENT_FILE : FILEMANAGER::COMMAND_CHOOSE_FILE, 
				param,
				preferences->EmulateImmediately() 
			); 
			break;

   		case FILE_NSP:   
			
			result = FileManager->LoadNSP
			( 
       			param ? FILEMANAGER::COMMAND_RECENT_FILE : FILEMANAGER::COMMAND_CHOOSE_FILE, 
				param,
				preferences->EmulateImmediately() 
			); 
			break;
		
		
		case FILE_INPUT: 
			
			result = FileManager->Load
			( 
     			FILEMANAGER::COMMAND_INPUT_FILE, 
				param,
				preferences->EmulateImmediately() 
			); 
			break;

		case FILE_ZIPPED: 

			result = FileManager->Load
			( 
				FILEMANAGER::COMMAND_ZIPPED_FILE, 
				param,
				preferences->EmulateImmediately() 
			); 
			break;

		default: return;
	}

	if (PDX_SUCCEEDED(result))
	{
		GraphicManager->ClearNesScreen();

		const BOOL IsPAL = nes.IsPAL();

		if (NesMode == NES::MODE_AUTO)
		{
			TimerManager->EnablePAL( IsPAL );
			GraphicManager->UpdateDirectDraw();

			if (bool(IsPAL) != bool(WasPAL) && windowed)
			{
				const UINT factor = GetAspectRatio();

				if (factor != UINT_MAX)
					OnWindowSize( factor );
			}
		}

		if (AutoSelectController)
			UpdateControllerPorts();

		StartScreenMsg
		(
		    1500,
		    "Loaded ",
			FileManager->GetRecentFile().GetFileName(),
			", ",
			(IsPAL ? "PAL" : "NTSC")
		);

		const PDXSTRING* const name = nes.GetMovieFileName();

		if (name)
			MovieManager->SetFile( *name );
	}
	else if (nes.IsOff())
	{
		GraphicManager->ClearNesScreen();
	}

	SetThreadPriority( GetDesiredPriority() );
	UpdateWindowItems();

	if (FileManager->UpdatedRecentFiles())
		UpdateRecentFiles();

	::InvalidateRect( hWnd, NULL, FALSE );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnLauncher()
{
	const BOOL EnableMenu = !windowed && !::GetMenu( hWnd );

	if (EnableMenu)
		GraphicManager->EnableGDI( TRUE );

	launcher->Open();
  
	if (EnableMenu)
	{
		::LockWindowUpdate( hWnd );
		
		menu.Check( IDM_VIEW_MENU );
		menu.Attach( hWnd );
		RefreshCursor();

		::LockWindowUpdate( NULL );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnSaveScreenShot()
{
	SoundManager->Clear();
	GraphicManager->SaveScreenShot();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateControllerPorts()
{
	nes.AutoSelectController();
	
	switch (nes.ConnectedController(0))
	{
     	case NES::CONTROLLER_UNCONNECTED: OnPort( IDM_MACHINE_PORT1_UNCONNECTED ); break;
		case NES::CONTROLLER_PAD1:        OnPort( IDM_MACHINE_PORT1_PAD1        ); break;
		case NES::CONTROLLER_PAD2:        OnPort( IDM_MACHINE_PORT1_PAD2        ); break;
		case NES::CONTROLLER_PAD3:        OnPort( IDM_MACHINE_PORT1_PAD3        ); break;
		case NES::CONTROLLER_PAD4:        OnPort( IDM_MACHINE_PORT1_PAD4        ); break;
		case NES::CONTROLLER_ZAPPER:      OnPort( IDM_MACHINE_PORT1_ZAPPER      ); break;
		case NES::CONTROLLER_PADDLE:      OnPort( IDM_MACHINE_PORT1_PADDLE      ); break;
		case NES::CONTROLLER_POWERPAD:    OnPort( IDM_MACHINE_PORT1_POWERPAD    ); break;
	}

	switch (nes.ConnectedController(1))
	{
     	case NES::CONTROLLER_UNCONNECTED: OnPort( IDM_MACHINE_PORT2_UNCONNECTED ); break;
		case NES::CONTROLLER_PAD1:        OnPort( IDM_MACHINE_PORT2_PAD1        ); break;
		case NES::CONTROLLER_PAD2:        OnPort( IDM_MACHINE_PORT2_PAD2        ); break;
		case NES::CONTROLLER_PAD3:        OnPort( IDM_MACHINE_PORT2_PAD3        ); break;
		case NES::CONTROLLER_PAD4:        OnPort( IDM_MACHINE_PORT2_PAD4        ); break;
		case NES::CONTROLLER_ZAPPER:      OnPort( IDM_MACHINE_PORT2_ZAPPER      ); break;
		case NES::CONTROLLER_PADDLE:      OnPort( IDM_MACHINE_PORT2_PADDLE      ); break;
		case NES::CONTROLLER_POWERPAD:    OnPort( IDM_MACHINE_PORT2_POWERPAD    ); break;
	}

	switch (nes.ConnectedController(2))
	{
     	case NES::CONTROLLER_UNCONNECTED: OnPort( IDM_MACHINE_PORT3_UNCONNECTED ); break;
		case NES::CONTROLLER_PAD1:        OnPort( IDM_MACHINE_PORT3_PAD1        ); break;
		case NES::CONTROLLER_PAD2:        OnPort( IDM_MACHINE_PORT3_PAD2        ); break;
		case NES::CONTROLLER_PAD3:        OnPort( IDM_MACHINE_PORT3_PAD3        ); break;
		case NES::CONTROLLER_PAD4:        OnPort( IDM_MACHINE_PORT3_PAD4        ); break;
	}

	switch (nes.ConnectedController(3))
	{
     	case NES::CONTROLLER_UNCONNECTED: OnPort( IDM_MACHINE_PORT4_UNCONNECTED ); break;
		case NES::CONTROLLER_PAD1:        OnPort( IDM_MACHINE_PORT4_PAD1        ); break;
		case NES::CONTROLLER_PAD2:        OnPort( IDM_MACHINE_PORT4_PAD2        ); break;
		case NES::CONTROLLER_PAD3:        OnPort( IDM_MACHINE_PORT4_PAD3        ); break;
		case NES::CONTROLLER_PAD4:        OnPort( IDM_MACHINE_PORT4_PAD4        ); break;
	}

	switch (nes.ConnectedController(4))
	{
     	case NES::CONTROLLER_UNCONNECTED: OnPort( IDM_MACHINE_EXPANSION_UNCONNECTED         ); break;
		case NES::CONTROLLER_KEYBOARD:    OnPort( IDM_MACHINE_EXPANSION_FAMILYBASICKEYBOARD ); break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnLoadNsp()
{
	OnOpen( FILE_NSP );
	UpdateFdsMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnSaveNsp()
{
	SoundManager->Clear();
	FileManager->SaveNSP();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnLoadState()
{
	SoundManager->Clear();
	FileManager->LoadNST();
	UpdateFdsMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnSaveState()
{
	SoundManager->Clear();
	FileManager->SaveNST();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnNetplay()
{
	if (launcher->IsEnabled())
		launcher->Close();

	menu.Disable( IDM_FILE_NETPLAY );	
	NetplayManager->Start();	
	menu.Enable( IDM_FILE_NETPLAY );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnSoundRecorder(const UINT wParam)
{
	switch (wParam)
	{
     	case IDM_FILE_SOUND_CAPTURE_FILE:   SoundManager->StartSoundRecordDialog(); break;
     	case IDM_FILE_SOUND_CAPTURE_RECORD: SoundManager->StartSoundRecording();    break;
     	case IDM_FILE_SOUND_CAPTURE_STOP:   SoundManager->StopSoundRecording();     break;
     	case IDM_FILE_SOUND_CAPTURE_RESET:  SoundManager->ResetSoundRecording();    break;
	}
	
	UpdateSoundRecorderMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateDynamicMenuItems()
{
	UpdateMovieMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnFdsInsertDisk(const UINT wParam)
{
	PDX_ASSERT( wParam >= IDM_FDS_INSERT_DISK_1 && wParam <= IDM_FDS_INSERT_DISK_16 );

	NES::IO::FDS::CONTEXT context;

	if (PDX_SUCCEEDED(nes.GetFdsContext( context )))
	{
		const UINT disk = wParam - IDM_FDS_INSERT_DISK_1;

		if (!context.DiskInserted || context.CurrentDisk != disk)
		{
			context.DiskInserted = TRUE;
			context.CurrentDisk = disk;
			nes.SetFdsContext( context );
			UpdateFdsMenu();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnFdsEjectDisk()
{
	NES::IO::FDS::CONTEXT context;

	if (PDX_SUCCEEDED(nes.GetFdsContext( context )))
	{
		if (context.DiskInserted)
		{
			context.DiskInserted = FALSE;
			nes.SetFdsContext( context );
			UpdateFdsMenu();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnFdsSide(const UINT wParam)
{
	NES::IO::FDS::CONTEXT context;

	if (PDX_SUCCEEDED(nes.GetFdsContext( context )))
	{
		const UINT side = (wParam == IDM_FDS_SIDE_A ? 0 : 1);

		if (context.CurrentSide != side)
		{
			context.CurrentSide = side;
			nes.SetFdsContext( context );
			UpdateFdsMenu();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateFdsMenu()
{
	menu.Disable( IDM_FDS_EJECT_DISK );
	menu.Disable( IDM_FDS_SIDE_A     );
	menu.Disable( IDM_FDS_SIDE_B     );
	
	NSTMENU SubMenu( menu.GetSub( NST_IDM_POS_FDS ) );
	SubMenu.GetSub(NST_IDM_POS_FDS_INSERTDISK).Clear();

	SubMenu.Enable( NST_IDM_POS_FDS_INSERTDISK, FALSE, NSTMENU::POS );
	SubMenu.Enable( NST_IDM_POS_FDS_DISKSIDE, FALSE, NSTMENU::POS );

	if (!nes.IsFds())
		return;

	NES::IO::FDS::CONTEXT context;

	if (PDX_FAILED(nes.GetFdsContext( context )))
		return;
	
	SubMenu.Enable( NST_IDM_POS_FDS_DISKSIDE, TRUE, NSTMENU::POS );
	menu.Enable( IDM_FDS_SIDE_A, context.CurrentSide != 0 );
	menu.Enable( IDM_FDS_SIDE_B, context.CurrentSide != 1 );

	if (!context.DiskInserted || context.NumDisks > 1)
	{
		SubMenu.Enable( NST_IDM_POS_FDS_INSERTDISK, TRUE, NSTMENU::POS );
		SubMenu = SubMenu.GetSub( NST_IDM_POS_FDS_INSERTDISK );

		PDXSTRING disk( "&" );

		const UINT NumDisks = PDX_MIN(16,context.NumDisks);

		for (UINT i=0; i < NumDisks; ++i)
		{
			disk.Resize(1);
			disk << (i+1);

			SubMenu.Insert( IDM_FDS_INSERT_DISK_1 + i, disk.String() );
			SubMenu.Enable( IDM_FDS_INSERT_DISK_1 + i, context.CurrentDisk != i || !context.DiskInserted );
		}
	}

	if (context.DiskInserted)
		menu.Enable( IDM_FDS_EJECT_DISK, TRUE );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateWindowItems()
{
	const NES::IO::CARTRIDGE::INFO* const info = nes.GetCartridgeInfo();

	PDXSTRING name( NST_WINDOW_NAME );

	const BOOL IsOn        = nes.IsOn();
	const BOOL IsImage     = nes.IsImage();
	const BOOL IsImageOn   = IsOn && IsImage;
	const BOOL IsNsf       = nes.IsNsf();
	const BOOL IsNsfOn     = IsOn && IsNsf;
	const BOOL IsLoaded    = IsImage || IsNsf;
	const BOOL IsLoadedOn  = IsOn && IsLoaded;
	const BOOL IsCartridge = nes.IsCartridge();

	if (IsOn)
	{
		if (IsCartridge)
		{
			if (info && info->name != "unknown")
				name = info->name;
		}
		else if (IsNsf)
		{
			NES::IO::NSF::CONTEXT context;
			nes.GetNsfContext( context );
			name = context.name;
		}
	}

	UpdateNsf();
	ResetSaveSlots( IsImageOn );
	::SetWindowText( hWnd, name.String() ); 

	const BOOL ExportBitmaps = GraphicManager->CanExportBitmaps();

	menu.Enable( IDM_FILE_CLOSE,                  IsLoaded                        );
	menu.Enable( IDM_FILE_LOAD_NST,               IsImageOn                       );
	menu.Enable( IDM_FILE_SAVE_NST,               IsImageOn                       );
	menu.Enable( IDM_FILE_SAVE_NSP,               IsImage                         );
	menu.Enable( IDM_FILE_SAVE_SCREENSHOT,        IsImageOn && ExportBitmaps      );
	menu.Enable( IDM_MACHINE_RESET_SOFT,          IsOn                            );
	menu.Enable( IDM_MACHINE_RESET_HARD,          IsOn                            );
	menu.Enable( IDM_MACHINE_POWER_ON,            !IsOn && IsLoaded               );
	menu.Enable( IDM_MACHINE_POWER_OFF,           IsOn                            );
	menu.Enable( IDM_MACHINE_PAUSE,               IsLoadedOn                      );
	menu.Enable( IDM_NSF_PLAY,                    IsNsfOn                         );
	menu.Enable( IDM_NSF_STOP,                    IsNsfOn                         );
	menu.Enable( IDM_NSF_PREV,                    IsNsfOn                         );
	menu.Enable( IDM_NSF_NEXT,                    IsNsfOn                         );
	menu.Enable( IDM_VIEW_ROM_INFO,               nes.IsCartridge()               );
	menu.Enable( IDM_MACHINE_OPTIONS_DIPSWITCHES, nes.GetNumVsSystemDipSwitches() );

	menu.Check( IDM_MACHINE_PAUSE, nes.IsPaused() );

	{
		NSTMENU SubMenu;

		SubMenu = menu.GetSub( NST_IDM_POS_FILE );
		SubMenu.Enable( NST_IDM_POS_FILE_SAVE, IsImage, NSTMENU::POS );
		SubMenu.Enable( NST_IDM_POS_FILE_QUICKLOADSTATE, IsImageOn, NSTMENU::POS );
		SubMenu.Enable( NST_IDM_POS_FILE_QUICKSAVESTATE, IsImageOn, NSTMENU::POS );
		SubMenu.Enable( NST_IDM_POS_FILE_RECENT, FileManager->NumRecentFiles(), NSTMENU::POS );

		SubMenu = menu.GetSub( NST_IDM_POS_MACHINE );
		SubMenu.Enable( NST_IDM_POS_MACHINE_POWER, IsOn || (IsLoaded && !IsOn), NSTMENU::POS );
		SubMenu.Enable( NST_IDM_POS_MACHINE_RESET, IsOn, NSTMENU::POS );
	}

	UpdateSoundRecorderMenu();
	UpdateFdsMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateSoundRecorderMenu()
{
	const BOOL yeah = 
	(
     	SoundManager->IsSoundRecordingFilePresent() && 
		nes.IsOn() && (nes.IsImage() || nes.IsNsf())
	);

	menu.Enable( IDM_FILE_SOUND_CAPTURE_RECORD, yeah && !SoundManager->IsSoundRecorderRecording() );
	menu.Enable( IDM_FILE_SOUND_CAPTURE_STOP,   yeah &&  SoundManager->IsSoundRecorderRecording() );
	menu.Enable( IDM_FILE_SOUND_CAPTURE_RESET,  yeah );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateMovieMenu()
{
	menu.Enable( IDM_FILE_MOVIE_STOP,	 MovieManager->CanStop()    );
	menu.Enable( IDM_FILE_MOVIE_PLAY,    MovieManager->CanPlay()    );
	menu.Enable( IDM_FILE_MOVIE_RECORD,  MovieManager->CanRecord()  );
	menu.Enable( IDM_FILE_MOVIE_REWIND,  MovieManager->CanRewind()  );
	menu.Enable( IDM_FILE_MOVIE_FORWARD, MovieManager->CanForward() );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateNsf()
{
	NsfInfo.Clear();

	if (nes.IsNsf())
	{
		NES::IO::NSF::CONTEXT context;

		if (PDX_SUCCEEDED(nes.GetNsfContext(context)))
		{
			NsfInfo.name << context.name << (context.pal ? ", PAL" : ", NTSC");

			if (context.artist != "<?>" && context.artist != "< ? >" && context.artist != "?")
				NsfInfo.artist = context.artist;

			if (context.copyright != "<?>" && context.copyright != "< ? >" && context.copyright != "?")
				NsfInfo.copyright = context.copyright;

			if (context.chip.Length())
				NsfInfo.copyright << ", " << context.chip << " chip";

			NsfInfo.song << "Song: " << (context.song+1) << "/" << context.NumSongs;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnClose()
{
	if (menu.IsDisabled(IDM_FILE_CLOSE))
		return;

	SoundManager->Stop();

	if (preferences->ConfirmReset() && nes.IsOn())
	{
		if (!UI::MsgQuestion( IDS_MACHINE_DETACH_IMAGE, IDS_MACHINE_CONFIRM_DETACH_IMAGE ))
			return;
	}

	GameGenieManager->ClearCodes( TRUE );
	nes.Unload();
	UpdateWindowItems();
	GraphicManager->ClearNesScreen();
	SetThreadPriority( THREAD_PRIORITY_NORMAL );
	StartScreenMsg( 1500, "Closed ", FileManager->GetRecentFile().GetFileName() );
	OnPaint();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnCloseWindow()
{
	if (preferences->PowerOffOnClose() && nes.IsOn())
	{
		if (!preferences->ConfirmExit() || UI::MsgQuestion(IDS_APP_DETACH_IMAGE,IDS_APP_EXIT_CONFIRM))
			OnPower( FALSE );
	}
	else
	{
		if (!preferences->ConfirmExit() || UI::MsgQuestion(IDS_APP_EXIT,IDS_APP_EXIT_CONFIRM))
			PostQuitMessage(0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnRecent(const UINT idm)
{
	PDX_ASSERT( idm >= IDM_FILE_RECENT_0 && idm <= IDM_FILE_RECENT_9 );
	
	const UINT recent = idm - IDM_FILE_RECENT_0;
	OnOpen( FILE_ALL, PDX_CAST(const VOID*,&recent) );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnPower(const BOOL state)
{
	if (preferences->ConfirmReset() && !state && nes.IsOn())
	{
		if (!UI::MsgQuestion( IDS_MACHINE_POWER_OFF, IDS_MACHINE_CONFIRM_POWER_OFF ))
			return;
	}

	if (bool(nes.IsOn()) != bool(state))
	{
		SoundManager->Clear();		
		nes.Power( state );		
		UpdateWindowItems();
		SetThreadPriority( GetDesiredPriority() );
		GraphicManager->ClearNesScreen();				
		StartScreenMsg( 1500, "Power ", (state ? "On.." : "Off..") );
		OnPaint();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnReset(const BOOL state)
{
	if (preferences->ConfirmReset() && nes.IsOn())
	{
		if (!UI::MsgQuestion( IDS_MACHINE_RESET, IDS_MACHINE_CONFIRM_RESET ))
			return;
	}

	SoundManager->Clear();
	nes.Reset( state );
	StartScreenMsg( 1500, (state ? "Hard" : "Soft"), " reset.." );
	menu.Check( IDM_MACHINE_PAUSE, nes.IsPaused() );
	UpdateFdsMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnMode(const UINT NewIdm)
{
	NES::MODE NewMode;

	switch (NewIdm)
	{
    	case IDM_MACHINE_MODE_AUTO: NewMode = NES::MODE_AUTO; break;
		case IDM_MACHINE_MODE_NTSC: NewMode = NES::MODE_NTSC; break;
		default:                    NewMode = NES::MODE_PAL;  break;
	}

	if (NesMode != NewMode)
	{
		nes.SetMode( NewMode );
		GraphicManager->UpdateDirectDraw();

		const BOOL IsPAL = nes.IsPAL();
		TimerManager->EnablePAL( IsPAL );

		if (windowed)
		{
			const UINT factor = GetAspectRatio();

			if (factor != UINT_MAX)
				OnWindowSize( factor );
		}

		UINT OldIdm;

		switch (NesMode)
		{
       		case NES::MODE_AUTO: OldIdm = IDM_MACHINE_MODE_AUTO; break;
     		case NES::MODE_NTSC: OldIdm = IDM_MACHINE_MODE_NTSC; break;
    		default:             OldIdm = IDM_MACHINE_MODE_PAL;  break;
		}

		NesMode = NewMode;

		menu.Uncheck( OldIdm );
		menu.Check( NewIdm );

		StartScreenMsg( 1500, "Switched to ", (IsPAL ? "PAL.." : "NTSC..") );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnPaint(const BOOL From_WM_PAINT)
{
	if (From_WM_PAINT)
	{
    	PAINTSTRUCT ps;

    	::BeginPaint( hWnd, &ps );
    	::EndPaint( hWnd, &ps );

		if ((ps.rcPaint.right - ps.rcPaint.left) <= 0 || (ps.rcPaint.bottom - ps.rcPaint.top) <= 0)
			return;
	}

	if (GraphicManager->TestCooperativeLevel())
	{
		GraphicManager->ClearScreen();

		if (nes.IsOn())
		{
			if (nes.IsNsf())
			{
				GraphicManager->DisplayNsf
				(
					NsfInfo.name,
					NsfInfo.artist,
					NsfInfo.copyright,
					NsfInfo.song
				);
			}
			else if (nes.IsImage())
			{
				GraphicManager->RedrawNesScreen();
			}
		}

		if (!windowed)
		{
			if (ScreenMsg.Length())
				GraphicManager->DisplayMsg( &ScreenMsg );

			if (ShowFPS)
				DisplayFPS( FALSE );
		}

		GraphicManager->Repaint();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnMouseMove(const LPARAM lParam)
{
	if (nes.IsAnyControllerConnected( NES::CONTROLLER_PADDLE ))
	{
		POINT point =
		{
			GET_X_LPARAM( lParam ),
			GET_Y_LPARAM( lParam )
		};

		InputManager->OnMouseButtonDown( point );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnLeftMouseButtonDown(const LPARAM lParam)
{
	if (nes.IsAnyControllerConnected( NES::CONTROLLER_ZAPPER, NES::CONTROLLER_PADDLE ))
	{
		POINT point =
		{
			GET_X_LPARAM( lParam ),
			GET_Y_LPARAM( lParam )
		};

		InputManager->OnMouseButtonDown( point );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnLeftMouseButtonUp()
{															  
	if (nes.IsAnyControllerConnected( NES::CONTROLLER_ZAPPER, NES::CONTROLLER_PADDLE ))
		InputManager->OnMouseButtonUp();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnToggleMenu()
{
	if (::GetMenu( hWnd )) OnHideMenu();
	else                   OnShowMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnShowMenu()
{
	if (!::GetMenu( hWnd ))
	{
		::SetFocus( hWnd );

		menu.Check( IDM_VIEW_MENU );

		if (windowed)
		{
			const BOOL zoomed = ::IsZoomed( hWnd );

			menu.Attach( hWnd );

			::SetWindowPos
			( 
				hWnd, 
				NULL, 
				0, 
				0, 
				rcWindow.right - rcWindow.left, 
				(rcWindow.bottom - rcWindow.top) + menu.GetHeight( hWnd ), 
				SWP_NOMOVE|SWP_NOZORDER 
			);

			if (zoomed)
				::ShowWindow( hWnd, SW_MAXIMIZE );
		}
		else
		{
			::LockWindowUpdate( hWnd );

			GraphicManager->EnableGDI( TRUE );

			menu.Attach( hWnd );
			RefreshCursor();

			if (launcher->IsEnabled())
				launcher->Activate();

			::LockWindowUpdate( NULL );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnHideMenu()
{
	if (::GetMenu( hWnd ))
	{
		::SetFocus( hWnd );

		menu.Uncheck( IDM_VIEW_MENU );

		if (windowed)
		{
			const BOOL zoomed = ::IsZoomed( hWnd );
			const INT height = menu.GetHeight( hWnd );
			
			menu.Detach( hWnd );

			::SetWindowPos
			( 
				hWnd, 
				NULL, 
				0, 
				0, 
				rcWindow.right - rcWindow.left, 
				(rcWindow.bottom - rcWindow.top) - height, 
				SWP_NOMOVE|SWP_NOZORDER 
			);

			if (zoomed)
				::ShowWindow( hWnd, SW_MAXIMIZE );
		}
		else
		{
			if (launcher->IsEnabled())
				launcher->Inactivate();

			::LockWindowUpdate( hWnd );

			menu.Detach( hWnd );
			RefreshCursor();

			::LockWindowUpdate( NULL );
			GraphicManager->EnableGDI( FALSE );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NSTMENU APPLICATION::SwitchMenu(NSTMENU NewMenu)
{
	NSTMENU OldMenu( menu );
	menu = NewMenu;

	if (::GetMenu( hWnd ) == OldMenu)
		::SetMenu( hWnd, HMENU(NewMenu) );

	return OldMenu;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnToggleStatusBar()
{
	if (windowed)
	{
		ShowFPS = !StatusBar->IsEnabled();

		menu.Check( IDM_VIEW_STATUSBAR, ShowFPS );
		menu.Enable( IDM_VIEW_FPS, ShowFPS );

		INT bottom;

		if (ShowFPS)
		{
			ShowFPS = menu.IsChecked( IDM_VIEW_FPS );
			StatusBar->Create( hInstance, hWnd );
			bottom = rcWindow.bottom + StatusBar->GetHeight();
		}
		else
		{
			bottom = rcWindow.bottom - StatusBar->GetHeight();
			StatusBar->Destroy();
		}

		const BOOL zoomed = ::IsZoomed( hWnd );

		::SetWindowPos
		( 
			hWnd, 
			NULL, 
			0, 
			0, 
			rcWindow.right - rcWindow.left, 
			bottom - rcWindow.top, 
			SWP_NOMOVE|SWP_NOZORDER 
		);

		if (zoomed)
			::ShowWindow( hWnd, SW_MAXIMIZE );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnToggleFPS()
{
	ShowFPS = !menu.ToggleCheck( IDM_VIEW_FPS );

	if (!ShowFPS)
		StatusBar->DisplayFPS( FALSE );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnTop()
{
	if (windowed)
	{
		HWND hZPos = menu.ToggleCheck( IDM_VIEW_ON_TOP ) ? HWND_NOTOPMOST : HWND_TOPMOST;
		::SetWindowPos( hWnd, hZPos, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::GetScreenRect(RECT& rect) const
{
	::GetClientRect( hWnd, &rect );

	POINT point = {rect.right,rect.bottom};

	::ClientToScreen( hWnd, &point );

	::SetRect
	( 
    	&rect, 
		point.x - rect.right, 
		point.y - rect.bottom, 
		point.x, 
		point.y - StatusBar->GetHeight()
	);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnSizeMove(const LPARAM lParam)
{
	if (windowed)
	{
		const WINDOWPOS& wp = *PDX_CAST(const WINDOWPOS*,lParam);

		if ((wp.flags & (SWP_NOMOVE|SWP_NOSIZE)) != (SWP_NOMOVE|SWP_NOSIZE))
		{
			::SetRect( &rcWindow, wp.x, wp.y, wp.x + wp.cx, wp.y + wp.cy );
			GetScreenRect( rcScreen );
			StatusBar->Resize();
			GraphicManager->UpdateScreenRect( rcScreen );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnActivate(const WPARAM wParam)
{
	WindowVisible = !HIWORD(wParam);

	switch (LOWORD(wParam))
	{
   		case WA_INACTIVE: 

			OnInactive();
			return;

   		case WA_ACTIVE:
   		case WA_CLICKACTIVE: 

			OnActive();
			return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateWindowSizes(const UINT width,const UINT height)
{
	NSTMENU SubMenu( menu.GetSub(NST_IDM_POS_VIEW).GetSub(NST_IDM_POS_VIEW_SCREENSIZE) );

	SubMenu.Clear();
	SubMenu.Insert( IDM_VIEW_WINDOWSIZE_MAX, "&Max\tAlt+M" );

	PDXSTRING item( "&" );

	for (UINT j=0,i=1,x=NES::IO::GFX::WIDTH,y=NES::IO::GFX::HEIGHT; x <= width && y <= height && j < 4; ++j, i *= 2, x *= 2, y *= 2) 
	{
		item.Resize(1);
		item << i << "X\tAlt+" << (j+1);
		SubMenu.Insert( IDM_VIEW_WINDOWSIZE_1X + j, item.String() );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

UINT APPLICATION::GetAspectRatio() const
{
	if (windowed && ::IsZoomed( hWnd ))
		return UINT_MAX;

	RECT rcClient;

	if (windowed)
		::GetClientRect( hWnd, &rcClient );
	else
		rcClient = rcDesktopClient;

	const LONG width = rcClient.right - rcClient.left;
	const LONG height = rcClient.bottom - rcClient.top;

	for (UINT i=1; i < 4; ++i)
	{
		if (width < (256*i) + ((256*i)/2) || height < (240*i) + ((240*i)/2))
		{
			if (i == 3 && !windowed)
				return UINT_MAX;

			return (i-1);
		}
	}

	return UINT_MAX;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnWindowSize(const UINT factor,const BOOL UpdateZOrder)
{
	if (windowed)
	{
		if (factor == UINT_MAX)
		{
			::ShowWindow( hWnd, SW_MAXIMIZE );

			if (UpdateZOrder)
			{
				::SetWindowPos
				( 
			     	hWnd, 
					(menu.IsChecked(IDM_VIEW_ON_TOP) ? HWND_TOPMOST : HWND_NOTOPMOST), 
					0,0,0,0,
					SWP_NOMOVE|SWP_NOSIZE 
				);
			}
		}
		else
		{
			if (::IsZoomed( hWnd ))
				::SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0 );

			INT x = GraphicManager->GetNesRect().right - GraphicManager->GetNesRect().left;
			INT y = GraphicManager->GetNesRect().bottom - GraphicManager->GetNesRect().top;

			RECT rcExtra = {0,0,x,y};

			::AdjustWindowRectEx
			( 
		       	&rcExtra, 
				NST_WINDOWSTYLE, 
				::GetMenu( hWnd ) != NULL,
				NST_WINDOWSTYLE_EX
			);

			rcExtra.right = (rcExtra.right - rcExtra.left) - x;
			rcExtra.bottom = (rcExtra.bottom - rcExtra.top) - y;

			const INT width = ::GetSystemMetrics( SM_CXFULLSCREEN );
			const INT height = ::GetSystemMetrics( SM_CYFULLSCREEN );

			for (UINT i=0; i < factor; ++i)
			{
				if (((x*2) > width) || ((y*2) > height))
					break;

				x *= 2;
				y *= 2;
			}

			x += rcExtra.right;
			y += rcExtra.bottom + StatusBar->GetHeight();

			HWND hZPos = NULL;
			DWORD flags = SWP_NOMOVE|SWP_NOZORDER;

			if (UpdateZOrder)
			{
				flags = SWP_NOMOVE;
				hZPos = (menu.IsChecked( IDM_VIEW_ON_TOP ) ? HWND_TOPMOST : HWND_NOTOPMOST);
			}

			::SetWindowPos( hWnd, hZPos, 0, 0, x, y, flags );

			if (::GetMenu( hWnd ))
			{
				const INT MenuHeight = menu.GetHeight( hWnd );
				const INT MenuMetric = ::GetSystemMetrics( SM_CYMENU );

				if (MenuHeight > MenuMetric)
					::SetWindowPos( hWnd, hZPos, 0, 0, x, y + (MenuHeight - MenuMetric), flags );
			}
		}
	}
	else
	{
		GraphicManager->SetScreenSize
		(
       		factor == UINT_MAX ? GRAPHICMANAGER::SCREEN_STRETCHED : GRAPHICMANAGER::SCREENTYPE(factor)		    
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::ResetSaveSlots(const BOOL enabled)
{
	for (UINT i=IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST; i <= IDM_FILE_QUICK_LOAD_STATE_SLOT_9; ++i)
		menu.Enable( i, enabled );

	for (UINT i=IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT; i <= IDM_FILE_QUICK_SAVE_STATE_SLOT_9; ++i)
		menu.Enable( i, enabled );
}

////////////////////////////////////////////////////////////////////////////////////////
// switch to full screen / window
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::SwitchScreen()
{
	SoundManager->Clear();

	const BOOL LauncherEnabled = launcher->IsEnabled();

	if (windowed)					
	{
		if (LauncherEnabled)
			launcher->Close();

		PushWindow();
		GraphicManager->SwitchToFullScreen();

		if (menu.IsChecked( IDM_VIEW_MENU ) || nes.IsOff())
			OnShowMenu();
	}
	else
	{
		GraphicManager->SwitchToWindowed( rcScreen );
		PopWindow();

		if (LauncherEnabled)
			launcher->Activate();
	}

	UpdateWindowSizes
	( 
		GraphicManager->GetDisplayWidth(), 
		GraphicManager->GetDisplayHeight() 
	);
}

////////////////////////////////////////////////////////////////////////////////////////
// save the window size and style
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::PushWindow()
{
	rcDesktop = rcWindow;
	::GetClientRect( hWnd, &rcDesktopClient );

	windowed = FALSE;
	menu.Detach( hWnd );

	StatusBar->Destroy();

	::SetWindowLong( hWnd, GWL_STYLE, WS_POPUP|WS_VISIBLE );
	::SetWindowLong( hWnd, GWL_EXSTYLE, 0 );
	::SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED );

	menu.SetText( IDM_VIEW_SWITCH_SCREEN, "&Window\tAlt+Enter" );

	menu.Enable( IDM_VIEW_STATUSBAR, FALSE );
	menu.Enable( IDM_VIEW_ON_TOP,    FALSE );
	menu.Enable( IDM_VIEW_FPS,       TRUE  );

	ShowFPS = menu.IsChecked( IDM_VIEW_FPS );

	RefreshCursor();
}

////////////////////////////////////////////////////////////////////////////////////////
// restore the window size and style
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::PopWindow()
{
	if (menu.IsChecked( IDM_VIEW_MENU ))
		menu.Attach( hWnd );
	else
		menu.Detach( hWnd );

	windowed = TRUE;

	menu.SetText( IDM_VIEW_SWITCH_SCREEN, "&Fullscreen\tAlt+Enter" );

	menu.Enable( IDM_VIEW_STATUSBAR, TRUE  );
	menu.Enable( IDM_VIEW_ON_TOP,    TRUE  );

	RefreshCursor();

	::SetWindowLong( hWnd, GWL_STYLE, NST_WINDOWSTYLE );
	::SetWindowLong( hWnd, GWL_EXSTYLE, NST_WINDOWSTYLE_EX );

	ShowFPS = menu.IsChecked( IDM_VIEW_STATUSBAR );
	menu.Enable( IDM_VIEW_FPS, ShowFPS );

	if (ShowFPS)
	{
		ShowFPS = menu.IsChecked( IDM_VIEW_FPS );
		StatusBar->Create( hInstance, hWnd );
	}

	::SetWindowPos
	(
		hWnd,
		(menu.IsChecked(IDM_VIEW_ON_TOP) ? HWND_TOPMOST : HWND_NOTOPMOST),
		rcDesktop.left,
		rcDesktop.top,
		rcDesktop.right - rcDesktop.left,
		rcDesktop.bottom - rcDesktop.top,
		SWP_SHOWWINDOW
	);

	::InvalidateRect( NULL, NULL, FALSE );
	::Sleep( 500 );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::SetScreenMsg(const UINT duration,const BOOL PrevMsg)
{
	if (windowed)
	{
		StatusBar->DisplayMsg( ScreenMsg.String() );
	}
	else
	{
		const BOOL passive = IsPassive();

		if (PrevMsg && passive)
			GraphicManager->DisplayMsg( NULL );

		GraphicManager->DisplayMsg( &ScreenMsg );

		if (passive)
			GraphicManager->Repaint();
	}

	::SetTimer( hWnd, TIMER_ID_SCREEN_MSG, duration, OnScreenMsgEnd );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnLoadStateSlot(const UINT idm)
{
	if (!IsRunning() || nes.IsNsf())
		return;

	const UINT slot = idm - IDM_FILE_QUICK_LOAD_STATE_SLOT_LAST;

	if (PDX_SUCCEEDED(SaveStateManager->LoadState( slot )))
		StartScreenMsg( 1000, "Loaded from slot ", (slot ? slot : SaveStateManager->GetLastSlot()), ".." );
	else
		StartScreenMsg( 1000, "Failed to load from slot.." );

	UpdateFdsMenu();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::OnSaveStateSlot(const UINT idm)
{
	if (!IsRunning() || nes.IsNsf())
		return;

	if (PDX_SUCCEEDED(SaveStateManager->SaveState( idm - IDM_FILE_QUICK_SAVE_STATE_SLOT_NEXT )))
		StartScreenMsg( 1000, "Saved to slot ", SaveStateManager->GetLastSlot(), ".." );
	else
		StartScreenMsg( 1000, "Failed to save to slot.." );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID APPLICATION::UpdateRecentFiles()
{
	NSTMENU SubMenu( menu.GetSub(NST_IDM_POS_FILE).GetSub(NST_IDM_POS_FILE_RECENT) );

	SubMenu.Clear();

	PDXSTRING item;

	const UINT NumItems = PDX_MIN(10,FileManager->NumRecentFiles());

	for (UINT i=0; i < NumItems; ++i)
	{
		item = i;
		item << " " << FileManager->GetRecentFile(i);
		SubMenu.Insert( IDM_FILE_RECENT_0 + i, item.String() );
	}
}