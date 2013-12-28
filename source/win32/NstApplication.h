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

#ifndef NST_APPLICATION_H
#define NST_APPLICATION_H

#define application APPLICATION::GetSingleton()

#define NST_WM_OPEN_FILE 1
#define NST_WM_OPEN_ZIPPED_FILE 2

#define NST_CLASS_NAME "Nestopia Window"
#define NST_WINDOW_NAME "Nestopia"

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

#include "../paradox/PdxSingleton.h"
#include "../NstNes.h"
#include "NstStatusBar.h"
#include "NstMenu.h"

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

class TIMERMANAGER;
class GRAPHICMANAGER;
class SOUNDMANAGER;
class INPUTMANAGER;
class FILEMANAGER;
class FDSMANAGER;  
class GAMEGENIEMANAGER;
class SAVESTATEMANAGER;
class MOVIEMANAGER;
class VSDIPSWITCHMANAGER;
class PREFERENCES;
class LOGFILE;
class ROMINFO;
class HELPMANAGER; 
class USERINPUTMANAGER;
class CONFIGFILE;
class LAUNCHER;
class NETPLAYMANAGER;

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

#define NST_IDM_POS_FILE                 0
#define NST_IDM_POS_FILE_LOAD            3
#define NST_IDM_POS_FILE_SAVE            4
#define NST_IDM_POS_FILE_QUICKLOADSTATE	 6
#define NST_IDM_POS_FILE_QUICKSAVESTATE	 7
#define NST_IDM_POS_FILE_SAVE_SCREENSHOT 11
#define NST_IDM_POS_FILE_SOUND_RECORDER  12
#define NST_IDM_POS_FILE_MOVIE_PLAYER    13
#define NST_IDM_POS_FILE_RECENT          17
#define NST_IDM_POS_MACHINE              1
#define NST_IDM_POS_MACHINE_POWER        0
#define NST_IDM_POS_MACHINE_RESET        1
#define NST_IDM_POS_MACHINE_PORT_1	     5
#define NST_IDM_POS_MACHINE_PORT_2	     6
#define NST_IDM_POS_MACHINE_PORT_3	     7
#define NST_IDM_POS_MACHINE_PORT_4	     8
#define NST_IDM_POS_MACHINE_PORT_5	     9
#define NST_IDM_POS_MACHINE_MODE         11
#define NST_IDM_POS_MACHINE_OPTIONS	     13
#define NST_IDM_POS_FDS                  2
#define NST_IDM_POS_FDS_INSERTDISK       0
#define NST_IDM_POS_FDS_EJECTDISK        1
#define NST_IDM_POS_FDS_DISKSIDE         2
#define NST_IDM_POS_FDS_SEPARATOR        3
#define NST_IDM_POS_FDS_OPTIONS          4
#define NST_IDM_POS_VIEW                 4
#define NST_IDM_POS_VIEW_SCREENSIZE      3
#define NST_IDM_POS_VIEW_SHOW            4

////////////////////////////////////////////////////////////////////////////////////////
// window class
////////////////////////////////////////////////////////////////////////////////////////

class APPLICATION : public PDXSINGLETON<APPLICATION>
{
public:

	APPLICATION(HINSTANCE,const CHAR* const,const INT);
	~APPLICATION();

	INT Run();

	HWND GetHWnd() const;
	HINSTANCE GetInstance() const;
	HACCEL GetHAccel() const;

	BOOL IsActive()     const;
	BOOL IsWindowed()   const;
	BOOL IsMenuSet()    const;
	BOOL IsRunning()    const;
	BOOL IsPassive()    const;
	BOOL HasStatusBar() const;
						
	NES::MACHINE& GetNes();

	PDX_NO_INLINE VOID RefreshCursor(const BOOL=FALSE);
	PDX_NO_INLINE VOID UpdateWindowSizes(const UINT,const UINT);

	SAVESTATEMANAGER& GetSaveStateManager  ();
	FILEMANAGER&      GetFileManager       ();     
	GAMEGENIEMANAGER& GetGameGenieManager  ();
	GRAPHICMANAGER&   GetGraphicManager    ();  
	USERINPUTMANAGER& GetUserInputManager  ();
	SOUNDMANAGER&     GetSoundManager      ();
	PREFERENCES&      GetPreferences       ();
	MOVIEMANAGER&     GetMovieManager      ();
	TIMERMANAGER&     GetTimerManager      ();
	LAUNCHER&         GetLauncher          ();
	NETPLAYMANAGER&   GetNetplayManager    ();
	NSTMENU           GetMenu              ();

	NES::MODE GetNesMode() const;

	PDX_NO_INLINE VOID OnLoadStateSlot (const UINT);
	PDX_NO_INLINE VOID OnSaveStateSlot (const UINT);
	PDX_NO_INLINE VOID OnToggleMenu();

	enum
	{
		TIMER_ID_SCREEN_MSG = 1,
		TIMER_ID_AUTO_SAVE
	};

	template<class T>                         
	VOID StartScreenMsg(const UINT,const T&);

	template<class T,class U>
	VOID StartScreenMsg(const UINT,const T&,const U&);

	template<class T,class U,class V>
	VOID StartScreenMsg(const UINT,const T&,const U&,const V&);

	template<class T,class U,class V,class W> 
	VOID StartScreenMsg(const UINT,const T&,const U&,const V&,const W&);

	template<class T,class U,class V,class W,class X> 
	VOID StartScreenMsg(const UINT,const T&,const U&,const V&,const W&,const X&);

	template<class T,class U,class V,class W,class X,class Y> 
	VOID StartScreenMsg(const UINT,const T&,const U&,const V&,const W&,const X&,const Y&);

	VOID ExecuteImage();

	LRESULT MsgProc(const HWND,const UINT,const WPARAM,const LPARAM);

	NSTMENU SwitchMenu(NSTMENU);

	PDX_NO_INLINE VOID OnReset(const BOOL);
	PDX_NO_INLINE VOID OnFdsInsertDisk(const UINT);
	PDX_NO_INLINE VOID OnFdsEjectDisk();
	PDX_NO_INLINE VOID OnFdsSide(const UINT);

private:

	enum FILETYPE
	{
		FILE_ALL,
		FILE_NSP,
		FILE_INPUT,
		FILE_ZIPPED
	};

	static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

	PDX_NO_INLINE VOID ExecuteNsf();
	PDX_NO_INLINE VOID SwitchScreen();
	PDX_NO_INLINE VOID PushWindow();
	PDX_NO_INLINE VOID PopWindow();
	PDX_NO_INLINE VOID UpdateRecentFiles();
	PDX_NO_INLINE VOID UpdateWindowItems();
	PDX_NO_INLINE VOID UpdateSoundRecorderMenu();
	PDX_NO_INLINE VOID UpdateFdsMenu();
	PDX_NO_INLINE VOID DisplayMsg();
	PDX_NO_INLINE VOID OutputNsfInfo();
	PDX_NO_INLINE VOID DisplayFPS(BOOL=TRUE);
	PDX_NO_INLINE VOID SetScreenMsg(const UINT,const BOOL);
	
	enum CFG_OP
	{
		CFG_LOAD,
		CFG_SAVE
	};

	PDX_NO_INLINE BOOL InitConfigFile(CONFIGFILE&,const CFG_OP);

	VOID UpdateControllerPorts();
	VOID UpdateDynamicMenuItems();
	VOID UpdateMovieMenu();
	VOID UpdateNsf();
	VOID ResetSaveSlots(const BOOL=FALSE);
	VOID GetScreenRect(RECT&) const;

	PDX_NO_INLINE VOID OnOpen(const FILETYPE,const VOID* const=NULL);
	PDX_NO_INLINE VOID OnPort(const UINT);
	PDX_NO_INLINE VOID OnAutoSelectController();
	PDX_NO_INLINE VOID OnActive();
	PDX_NO_INLINE VOID OnDropFiles(WPARAM);
	PDX_NO_INLINE VOID OnInactive(const BOOL=FALSE);
	PDX_NO_INLINE BOOL OnCommand(const WPARAM);
	PDX_NO_INLINE VOID OnPower(const BOOL);
	PDX_NO_INLINE UINT GetAspectRatio() const;
	PDX_NO_INLINE VOID OnNsfCommand(const NES::IO::NSF::OP);
	PDX_NO_INLINE VOID OnWindowSize(const UINT,const BOOL=FALSE);
	PDX_NO_INLINE VOID OnToggleStatusBar();
	PDX_NO_INLINE VOID OnToggleFPS();
	PDX_NO_INLINE VOID OnHideMenu();
	PDX_NO_INLINE VOID OnShowMenu();
	PDX_NO_INLINE BOOL OnCopyData(const LPARAM);
	PDX_NO_INLINE VOID OnTop();
	PDX_NO_INLINE VOID SetThreadPriority(INT);
	PDX_NO_INLINE VOID OnPaint(const BOOL=FALSE);

	VOID OnMode(const UINT);
	VOID OnRecent(const UINT);
	VOID OnLauncher();
	VOID OnLoadNsp();
	VOID OnSaveNsp();
	VOID OnLoadState();
	VOID OnSaveState();
	VOID OnNetplay();
	VOID OnSoundRecorder(const UINT);
	BOOL OnSysCommand(const WPARAM);
	VOID OnMouseMove(const LPARAM);
	VOID OnLeftMouseButtonDown(const LPARAM);
	VOID OnLeftMouseButtonUp();
	VOID OnUnlimitedSprites();
	VOID OnNsfInBackground();
	VOID OnSizeMove(const LPARAM);
	VOID OnActivate(const WPARAM);
	VOID OnClose();
	VOID OnCloseWindow();
	VOID OnSaveScreenShot();
	VOID OnPause();
	VOID OnHelp(const UINT);

	INT GetDesiredPriority() const;

	INT ThreadPriority;

	BOOL AcceleratorEnabled;
	BOOL active;
	BOOL windowed;
	BOOL AutoSelectController;
	
	NSTMENU      menu;
	HCURSOR      hCursor;
	HACCEL const hAccel;
	BOOL         UseZapper;
	BOOL         ShowFPS;
	BOOL         InBackground;
	BOOL         WindowVisible;
	BOOL         ExitSuccess;
	
	RECT rcWindow;
	RECT rcScreen;
	RECT rcDesktop;
	RECT rcDesktopClient;

	HWND hWnd;
	HINSTANCE const hInstance;

	STATUSBAR*          StatusBar;
	TIMERMANAGER*       TimerManager;
	GRAPHICMANAGER*     GraphicManager;
	SOUNDMANAGER*       SoundManager;
	INPUTMANAGER*       InputManager;
	FILEMANAGER*        FileManager;
	FDSMANAGER*         FdsManager;
	GAMEGENIEMANAGER*   GameGenieManager;
	SAVESTATEMANAGER*   SaveStateManager;
	MOVIEMANAGER*       MovieManager;
	VSDIPSWITCHMANAGER* VsDipSwitchManager;
	PREFERENCES*        preferences;
	LOGFILE*            log;
	ROMINFO*            RomInfo;
	HELPMANAGER*        HelpManager;
	USERINPUTMANAGER*   UserInputManager;
	LAUNCHER*           launcher;
	NETPLAYMANAGER*     NetplayManager;

	PDXSTRING ScreenMsg;

	NES::MODE NesMode;
	UINT SelectPort[5];

	NES::MACHINE nes;

	struct NSFINFO
	{
		VOID Clear()
		{
			name.Clear();
			artist.Clear();
			copyright.Clear();
			song.Clear();
		}

		PDXSTRING name;
		PDXSTRING artist;
		PDXSTRING copyright;
		PDXSTRING song;
	};

	NSFINFO NsfInfo;

	static VOID CALLBACK OnScreenMsgEnd(HWND,UINT,UINT_PTR,DWORD);
	static BOOL CALLBACK HelpAboutDlgProc(HWND,UINT,WPARAM,LPARAM);
};

#include "NstApplication.inl"

#endif
