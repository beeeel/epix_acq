/*
 *
 *	xclibex4.cpp	External	23-Nov-2018
 *
 *	Copyright (C)  1998-2018  EPIX, Inc.  All rights reserved.
 *
 *	Example program for the XCLIB 'C' Library.
 *	Example assumes Windows 95/98/ME/NT/2000/XP/Vista/7/8/10 32/64-Bit.
 *
 *	Demonstrates XCLIB and/or PXIPL functions for capture and
 *	display of sequences as well as saving of an image sequence.
 *	This Windows program must, of course, also make use of
 *	various Windows GDI API functions; however, this is not
 *	intended to serve as a Windows tutorial.
 *
 *	Also optionally demonstrates use of multiple PIXCI(R)
 *	frame grabbers operating simultaneously.
 *
 *	Many parts similar to the XCLIBEX2.CPP example; that
 *	example provides PIXCI SV2, SV3, SV4, SV5, SV5A, SV5B, SV5L, SV6, SV7, and SV8
 *	specific controls, and demonstrates more display options including
 *	graphic overlays. It also demonstrates access to numeric
 *	pixel values and use of Events rather than a timer.
 *	For simplicity this example concentrates on sequence
 *	capture and display features and does not illustrate
 *	as many display options nor duplicate the other features
 *	demonstrated in that example.
 *
 */

/*
 *  INSTRUCTIONS:
 *
 *  1)	Set 'define' options below according to the intended camera
 *	and video format.
 *
 *	For PIXCI(R) SV2, SV3, SV4, SV5, SV5A, SV5B, SV5L, and SV6 frame grabbers
 *	common choices are RS-170, NSTC, NTSC/YC, CCIR, PAL, or PAL/YC.
 *	(The SV5A and SV5B do not support NTSC/YC or PAL/YC).
 *	For PIXCI(R) SV7 frame grabbers
 *	common choices are RS-170, NSTC, CCIR, or PAL.
 *	For PIXCI(R) SV8 frame grabbers
 *	common choices are RS-170, NSTC, NTSC/YC, CCIR, PAL, PAL/YC.
 *
 *	For PIXCI(R) A, CL1, CL2, CL3SD, D, D24, D32, D2X, D3X, D3XE,
 *	E1, E1DB, E4, E4DB, E4G2, E4TX2, E8, E8CAM, E8DB, e104x4, EB1, EB1G2, EB1-POCL, EB1G2-POCL,
 *	EB1mini, miniH2b, miniH2, EC1, ECB1, ECB1-34, ECB2, EL1, EL1DB, ELS2, SI, SI1, SI2, and SI4
 *	frame grabbers, use "default" to select the default format for
 *	the camera for which the PIXCI(R) frame grabber is intended.
 *	For non default formats, use XCAP to save the video setup to
 *	a file, and set FORMATFILE_LOAD to the saved file's path name.
 *	For camera's with RS-232 control, note that the saved
 *	video setup only resets the PIXCI(R) frame grabber's
 *	settings, but XCLIB does not reset the camera's settings.
 *	For selected Camera Link cameras w. serial controls,
 *	the video setup file may include serial commands which are
 *	automatically sent by XCLIB to the camera.
 *
 *	Alternately, this could be modified to use getenv("PIXCI"),
 *	GetPrivateProfileString(...), RegQueryValueEx(...), or any
 *	other convention chosen by the programmer to allow run time
 *	selection of the video format and resolution.
 *
 */

#if !defined(FORMAT) && !defined(FORMATFILE_LOAD) && !defined(FORMATFILE_COMP)
				  // For PIXCI(R) SV2, SV3, SV4, SV5, SV5A, SV5B, SV5L, SV6
  //#define FORMAT  "RS-170"	  // RS-170 on input 2
  //#define FORMAT  "NTSC"	  // NTSC on input 2
  //#define FORMAT  "NTSC/YC"	  // NSTC S-Video on input 1		  (N/A on SV5A,SV5B)
  //#define FORMAT  "CCIR"	  // CCIR on input 2
  //#define FORMAT  "PAL"	  // PAL (B,D,G,H,I) on input 2
  //#define FORMAT  "PAL/YC"	  // PAL (B,D,G,H,I) S-Video on input 1   (N/A on SV5A,SV5B)
  //#define FORMAT  "default"	  // NSTC S-Video on input 1

				  // For PIXCI(R) SV7
  //#define FORMAT  "RS-170"	  // RS-170
  //#define FORMAT  "NTSC"	  // NTSC
  //#define FORMAT  "CCIR"	  // CCIR
  //#define FORMAT  "PAL"	  // PAL
  //#define FORMAT  "default"	  // NSTC

				  // For PIXCI(R) SV8
  //#define FORMAT  "RS-170"	  // RS-170 on BNC 0
  //#define FORMAT  "NTSC"	  // NTSC on BNC 0
  //#define FORMAT  "NTSC/YC"	  // NSTC S-Video
  //#define FORMAT  "CCIR"	  // CCIR on BNC 0
  //#define FORMAT  "PAL"	  // PAL on BNC 0
  //#define FORMAT  "PAL/YC"	  // PAL (B,D,G,H,I) S-Video
  //#define FORMAT  "default"	  // NSTC on BNC 0

				  //
				  // For PIXCI(R) A, CL1, CL2, CL3SD, D, D24, D32,
				  // D2X, D3X, D3XE, E1, E1DB, E4, E4DB, E4G2, E4TX2,
				  // E8, E8CAM, E8DB, e104x4, EB1, EB1G2, EB1-POCL, EB1G2-POCL, EB1mini, miniH2b, miniH2,
				  // EC1, ECB1, ECB1-34, ECB2, EL1, EL1DB,
				  // ELS2, SI, SI1, SI2, SI4
    #define FORMAT  "default"	  // as per board's intended camera

				  // For any PIXCI(R) frame grabber
				  // using a format file saved by XCAP
				  // which can either be:
  //#define FORMATFILE_LOAD   "xcvidset.fmt"  // loaded from file during execution
  //#define FORMATFILE_COMP   "xcvidset.fmt"  // or compiled into this application
#endif


/*
 *  2.1) Set number of expected PIXCI(R) frame grabbers, from 1 to 4.
 *  The XCLIB Simple 'C' Functions expect that the boards are
 *  identical and operated at the same resolution.
 *
 *  For PIXCI(R) frame grabbers with multiple, functional units,
 *  the XCLIB presents the two halves of the PIXCI(R) E1DB, E4DB, E4G2-2F, E4TX2-2F,
 *  E8CAM, E8DB, e104x4-2f, ECB2, EL1DB, ELS2, miniH2b, SI2, or SV7 frame grabbers,
 *  or the three parts of the PIXCI(R) E4G2-F2B, E4TX2-F2B, e104x4-f2b frame grabber,
 *  or the four quarters of the PIXCI(R) E4G2-4B, E4TX2-4B, e104x4-4b or SI4 frame grabbers,
 *  as two, three, or four independent PIXCI(R) frame grabbers, respectively.
 *
 */
#if !defined(UNITS)
    #define UNITS	1
#endif
#define UNITSMAP    ((1<<UNITS)-1)  /* shorthand - bitmap of all units */
#if !defined(UNITSOPENMAP)
    #define UNITSOPENMAP UNITSMAP
#endif


/*
 *  2.2) Optionally, set driver configuration parameters.
 *  These are normally left to the default, "".
 *  The actual driver configuration parameters include the
 *  desired PIXCI(R) frame grabbers, but to make configuation easier,
 *  code, below, will automatically add board selection to this.
 */
#if !defined(DRIVERPARMS)
  //#define DRIVERPARMS "-QU 0"   // don't use interrupts
    #define DRIVERPARMS ""	  // default
#endif


/*
 *  3.1) Choose which form of image display is to be demonstrated.
 *  Some of these  options expect that the optional PXIPL library is present.
 *  Others may expect that the Windows DirectDraw SDK is present
 *  (available from Microsoft) and that the S/VGA supports DirectDraw.
 *
 *  Only one of these choices should have value 1, the others should be 0.
 *
 */
#if !defined(SHOWIM_STRETCHDIBITS) && !defined(SHOWIM_DRAWDIBDRAW) && !defined(SHOWIM_DRAWDIBDISPLAY) \
 && !defined(SHOWIM_GDIDISPLAY)    && !defined(SHOWIM_DIRECTXDISPLAY)

    #define SHOWIM_STRETCHDIBITS    1	// use XCLIB or XCLIB-Lite and GDI
    #define SHOWIM_DRAWDIBDRAW	    0	// use XCLIB or XCLIB-Lite and Video for Windows
    #define SHOWIM_DRAWDIBDISPLAY   0	// use XCLIB and PXIPL and Video for Windows
    #define SHOWIM_GDIDISPLAY	    0	// use XCLIB and PXIPL
    #define SHOWIM_DIRECTXDISPLAY   0	// use XCLIB and PXIPL and DirectDraw
#endif


/*
 *  3.2)  Choose whether the PXIPL Image Processing Library
 *	  is available for saving multiple tiff images in one file.
 *	  And whether images are to be saved in tiff or simple binary format.
 */
#if !defined(USE_PXIPL)
    #define USE_PXIPL	0
    #define SAVE_TIFF	1
    #define SAVE_BINARY 0
    #define SAVE_AVI	0
#endif


/*
 *  4a) Select whether the start or end of sequence capture
 *	should be triggered by a general pupose input.
 *
 *	This is independent from the triggering of each image,
 *	of which some cameras are capable. The option below
 *	demonstrates a 'once per sequence' trigger, regardless
 *	of the camera's mode.
 */
#define GPIN_START_SEQUENCE   0     // 0: don't use triggered sequence capture
#define GPIN_START_GPIN       0x01  // which G.P. input, as a bitmap, to start upon
#define GPIN_START_DELAY      1000  // number of fields to delay following
				    // trigger before starting sequence capture
#define GPIN_END_SEQUENCE     0     // 0: don't use triggered sequence capture
#define GPIN_END_GPIN	      0x02  // which G.P. input, as a bitmap, to end upon
#define GPIN_END_DELAY	      1000  // number of fields to delay following
				    // trigger before ending sequence capture

/*
 *  4b) Or, select whether the start or end of sequence capture
 *	should be triggered by a general pupose trigger.
 *
 *	This is independent from the triggering of each image,
 *	of which some cameras are capable. The option below
 *	demonstrates a 'once per sequence' trigger, regardless
 *	of the camera's mode.
 */
#define TRIG_START_SEQUENCE   0     // 0: don't use triggered sequence capture
#define TRIG_START_GPIN       0x100 // which trigger input, as a bitmap, to start upon
#define TRIG_START_DELAY      10    // number of fields to delay following
				    // trigger before starting sequence capture
#define TRIG_END_SEQUENCE     0     // 0: don't use triggered sequence capture
#define TRIG_END_GPIN	      0x100 // which trigger input, as a bitmap, to end upon
#define TRIG_END_DELAY	      10    // number of fields to delay following
				    // trigger before ending sequence capture



/*
 *  5)	Compile with Microsoft Visual Studio VS2010 as Win32 Platform
 *	application using command line tools:
 *
 *	    cl -I "\Program Files\EPIX\XCLIB\INC" -c xclibex4.cpp
 *	    rc.exe /l 0x409 /fo tt.res	xclibex4.rc
 *	    echo -subsystem:windows,4.0 -entry:WinMainCRTStartup comdlg32.lib ole32.lib >t1
 *	    echo /NODEFAULTLIB libc.lib oldnames.lib user32.lib gdi32.lib kernel32.lib vfw32.lib largeint.lib winmm.lib >t2
 *
 *	    Either (XCLIB w/out PXIPL for Windows NT/2000/XP/Vista/7/8/10 32-Bit)
 *		link xclibex4.obj "\Program Files\EPIX\XCLIB\LIB\XCLIBWNT.LIB" tt.res @t1 @t2
 *	    or	(XCLIB+PXIPL for Windows NT/2000/XP/Vista/7/8/10 32-Bit)
 *		link xclibex4.obj "\Program Files\EPIX\XCLIB\LIB\XCLIBWNT.LIB" "\Program Files\EPIX\XCLIB\LIB\PXIPLWNT.LIB" tt.res @t1 @t2
 *	    or (XCLIB-Lite for Windows NT/2000/XP/Vista/7/8/10 32-Bit)
 *		link xclibex4.obj "\Program Files\EPIX\XCLIB\LIB\XCLYBWNT.LIB" tt.res @t1 @t2
 *
 *	    del t1
 *	    del t2
 *	    del tt.res
 *	    del xclibex4.obj
 *
 *	Or, compile with Microsoft Visual Studio VS2010 as x64 Platform
 *	application using command line tools:
 *
 *	    cl -I "\Program Files\EPIX\XCLIB\INC" -c xclibex4.cpp
 *	    rc.exe /l 0x409 /fo tt.res xclibex4.rc
 *	    echo -subsystem:windows -machine:X64 -entry:WinMainCRTStartup comdlg32.lib ole32.lib >t1
 *	    echo /NODEFAULTLIB libcmt.lib oldnames.lib user32.lib gdi32.lib kernel32.lib vfw32.lib winmm.lib >t2
 *
 *	Either (XCLIB w/out PXIPL for Windows XP/Vista/7/8/10 64-Bit
 *	    link xclibex4.obj "\Program Files\EPIX\XCLIB\LIB\XCLIBW64.LIB" tt.res @t1 @t2
 *	or  (XCLIB+PXIPL for Windows XP/Vista/7/8/10 64-Bit
 *	    link xclibex4.obj "\Program Files\EPIX\XCLIB\LIB\XCLIBW64.LIB" "\Program Files\EPIX\XCLIB\LIB\PXIPLW64.LIB" tt.res @t1 @t2
 *
 *	    del t1
 *	    del t2
 *	    del tt.res
 *	    del xclibex4.obj
 *
 *    Or use the Microsoft Visual Studio VS2010 IDE, compiling xclibex4.cpp
 *    as a Windows NT/2000/XP/Vista/7/8/10 Win32 Platform or Win64/x64 Platform app,
 *    using the XCLIBEX4.RC resource, linking to library(s):
 *
 *	  for XCLIB:	    XCLIBWNT.LIB (Win32) or XCLIBW64.LIB (Win64/x64)
 *	  for XCLIB+PXIPL:  XCLIBWNT.LIB PXIPLWNT.LIB (Win32) or XCLIBW64.LIB PXIPLW64.LIB (Win64/x64)
 *	  for XCLIB-Lite:   XCLYBWNT.LIB (Win32) or XCLYBW64.LIB (Win64/x64)
 *
 *    For example, in the IDE for Visual Studio 2010:
 *         1. Launch Microsoft Visual Studio 2010.
 *         2. Select 'File' -> 'New' -> 'Project'.
 *         3. Under 'Project types:', Select 'Visual C++->'Win32' and 'Win32 Project' under 'Templates',
 *            do NOT select 'Win32 Console Application'.
 *         4. Enter your Project Name and press the 'OK' button, and then the 'Finish' button.
 *         6. A window with generic example code will be displayed, replace with code from this file.
 *	   7. Copy XCLIBEX4.RC to your project directory.
 *	   8. Select 'Project' -> 'Add Existing Item' and select XCLIBEX4.RC.
 *         9. Select 'Project' and '"Project Name" Properties' or 'Properties'.
 *        10. Select 'Configuration Properties', Change 'Configuration:'
 *            selection from 'Active(debug)' to 'All Configurations' and Platform to 'All Platforms'.
 *        11. Under 'Configuration Properties', 'General' select 'Character Set' and specify 'Not Set'.
 *	  12. Under 'Configuration Properties', 'VC++ Directories' select 'Additional Include Directories' and specify '\Program Files\EPIX\XCLIB\INC'.
 *	  13. Under C/C++-> General select 'Additional Include Directories' and specify '\Program Files\EPIX\XCLIB\INC'.
 *	  14. Under Resources->General select Additional Include Directories specify '\Program Files\EPIX\XCLIB\INC'.
 *        15. Under C/C++-> Precompiled Headers select 'Create/Use Precompiled Header' and
 *	      specify 'Not Using Precompiled Headers'.
 *	  16. Under Linker-> General select 'Additional Library Directories' and specify '\Program Files\EPIX\XCLIB\LIB'.
 *        17. For Win32 target platforms, Select 'Configuration Properties', Change 'Configuration:'
 *            selection to 'All Configurations' and Platform to 'Win32'.
 *	      Under Linker-> Input-> Additional Dependencies add
 *		for XCLIB:	  XCLIBWNT.LIB
 *              for XCLIB+PXIPL:  XCLIBWNT.LIB PXIPLWNT.LIB
 *              for XCLIB-Lite:   XCLYBWNT.LIB XCLYBWNT.LIB
 *            to the list of modules already there. Be certain to enter
 *            a space between your entry and the adjacent name.
 *        18. For x64 target platforms, Select 'Configuration Properties', Change 'Configuration:'
 *            selection to 'All Configurations' and Platform to 'x64'. If x64 is not selectable,
 *	      use Configuration Manager to create a new 'Active solution platform'
 *	      and select x64 as the target and set 'Copy settings from:' to 'Win32'.
 *	      Under Linker-> Input-> Additional Dependencies add
 *		for XCLIB:	  XCLIBW64.LIB
 *              for XCLIB+PXIPL:  XCLIBW64.LIB PXIPLW64.LIB
 *              for XCLIB-Lite:   XCLYBW64.LIB
 *            to the list of modules already there and remove any XCLIB Win32 entries. Be certain to enter
 *            a space between your entry and the adjacent name.
 *        19. Click on OK.
 *        20. Copy the file(s)
 *	      for XCLIB:	XCLIBWNT.DLL or XCLIBW64.DLL
 *	      for XCLIB+PXIPL:	XCLIBWNT.DLL PXIPLWNT.DLL or XCLIBW64.DLL PXIPLW64.DLL
 *	      for XCLIB-Lite:	XCLYBWNT.DLL or XCLYBW64.DLL
 *	      to this project's directory.
 *	  21. Press F5 to build and run program.
 *
 *
 *  For those using with Borland, Watcom, Delphi, or older Microsoft compilers:
 *  The XCLIBEX2.CPP example provides additional compilation instructions
 *  and hints which can be also applied to this example, replacing
 *  'XCLIBEX2.CPP' and XCLIBEX2.RC with 'XCLIBEX4.CPP' and 'XCLIBEX4.RC'.
 *
 */


/*
 *  NECESSARY INCLUDES:
 */
#define _CRT_SECURE_NO_DEPRECATE    1

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#if SHOWIM_DRAWDIBDRAW || SHOWIM_DRAWDIBDISPLAY
    #include <vfw.h>
#endif
#if SHOWIM_DIRECTXDISPLAY
    #include <compobj.h>
    #include <ddraw.h>
#endif
// Will's growing includes
#include<iostream>
#ifndef _INC_SHLWAPI
#include <shlwapi.h> // For PathFileExists function
#pragma comment(lib, "Shlwapi.lib")
#endif
// For the threaded image processing
#include <thread>
#include <atomic>
// Honestly I think these could be arrays.
#include <vector>
#include <memory> // For dynamic memory allocation
#include <fstream>
#include <string>
#include "tiffio.h"  // Assumes libtiff is used for TIFF file handling
#include <direct.h> // For _mkdir on Windows
#include <shlobj.h> // For SHGetFolderPath

extern "C" {
    #include "xcliball.h"
    #include "xclibex2.h"
    #if SHOWIM_GDIDISPLAY || SHOWIM_DIRECTXDISPLAY || SHOWIM_DRAWDIBDISPLAY || USE_PXIPL
	#include "pxipl.h"         
	#include "pximages.h"           
    #endif
}

/*
 * Global variables.
 */
static	HWND	hWnd;	    /* the main window */
static	HWND	hDlg;	    /* the main dialog */
#if SHOWIM_DRAWDIBDRAW || SHOWIM_DRAWDIBDISPLAY
    static HDRAWDIB	hDrawDib = NULL;    /* VFW handle */
#endif
#if SHOWIM_DIRECTXDISPLAY
    static LPDIRECTDRAW lpDD = NULL;
    static HINSTANCE	hDDLibrary = NULL;  /* DDraw handles */
#endif

// Will's lazy coding
int save_int = 5000;
int n_frames = 100000;
double exp_ms = 1;
pxcoord_t aoi_x = 1;
pxcoord_t aoi_y = 1;
pxcoord_t aoi_w = 640;
pxcoord_t aoi_h = 480;
bool qpdacq = FALSE;
bool processingNotFinished = TRUE;
bool notInterrupted = true;
char savefilename[100];
char settfilename[100];
int imageWidth, imageHeight;
// Declare a global flag to indicate if the processing thread is running
std::atomic<bool> processingThreadRunning(false);


/*
 * Display specified buffer from specified unit,
 * in specified AOI of specified HWND,
 * using a compile-time selected method.
 */
void DisplayBuffer(int unit, pxbuffer_t buf, HWND hWndImage, struct pxywindow windImage[])
{
    HDC     hDC;
    int     err = 0;

    hDC = GetDC(hWndImage);

    //
    // Show image using pxd_renderStretchDIBits.
    //
    #if SHOWIM_STRETCHDIBITS
	SetStretchBltMode(hDC, STRETCH_DELETESCANS);
	err = pxd_renderStretchDIBits(1<<unit, buf, 0, 0, -1, -1, 0,
			 hDC, windImage[unit].nw.x, windImage[unit].nw.y,
			 windImage[unit].se.x-windImage[unit].nw.x,
			 windImage[unit].se.y-windImage[unit].nw.y, 0);
	if (err < 0)
	    MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_renderStretchDIBits", MB_OK|MB_TASKMODAL);
    #endif

    //
    // Show image using PXIPL pxio8_GDIDisplay.
    // Altho not used in this example, pxio8_GDIDisplay also allows drawing
    // a full screen cross hair cursor over the image.
    //
    #if SHOWIM_GDIDISPLAY
	err = pxio8_GDIDisplay(NULL, pxd_defineImage(1<<unit, buf, 0, 0, -1, -1, "Display"),
			 NULL, 0, 'n', 0, 0,hDC,&windImage[unit], NULL, NULL);
	if (err < 0)
	    MessageBox(NULL, pxd_mesgErrorCode(err), "pxio8_GDIDisplay", MB_OK|MB_TASKMODAL);
    #endif

    //
    // Show image using DirectDraw.
    // Note that this only supports S/VGA in
    // 24 or 32 bits per pixel mode.
    // This might be slightly quicker if the primary surface
    // were acquired once, and kept till the program exits.
    //
    // This can be extended to draw graphics in
    // the same manner as for pxio8_GDIDisplay, above.
    //
    // The pixel format of the S/VGA may not be acceptable!
    // Error reporting should be added!
    //
    #if SHOWIM_DIRECTXDISPLAY
    {
	DDSURFACEDESC	surfacedesc;
	LPDIRECTDRAWSURFACE ddrs = NULL;
	HRESULT     h;
	POINT	    pt;
	if (lpDD) {
	    //
	    // Get primary surface of full screen.
	    //
	    surfacedesc.dwFlags = DDSD_CAPS;
	    surfacedesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_VIDEOMEMORY;
	    surfacedesc.dwSize = sizeof(surfacedesc);
	    h = lpDD->CreateSurface(&surfacedesc, &ddrs, NULL);
	    if (ddrs) {
		//
		// Adjust for position of dialog on screen.
		//
		pt.x = pt.y = 0;
		ClientToScreen(hWndImage, &pt);
		windImage[unit].nw.x += pt.x;
		windImage[unit].nw.y += pt.y;
		windImage[unit].se.x += pt.x;
		windImage[unit].se.y += pt.y;
		err = pxio8_DirectXDisplay(NULL, pxd_defineImage(1<<unit, buf, 0, 0, -1, -1, "Display"),
				     NULL, 0, 'n', 0, 0, ddrs, &windImage[unit], NULL, hDC, NULL, NULL);
		windImage[unit].nw.x -= pt.x;
		windImage[unit].nw.y -= pt.y;
		windImage[unit].se.x -= pt.x;
		windImage[unit].se.y -= pt.y;
		((LPDIRECTDRAWSURFACE)ddrs)->Release();
	    }
	}
	if (err < 0)
	    MessageBox(NULL, pxd_mesgErrorCode(err), "pxio8_DirectXDisplay", MB_OK|MB_TASKMODAL);
    }
    #endif

    //
    // Show image using pxd_renderDIBCreate to create a
    // standard Windows DIB, and display with Video for Windows.
    //
    #if SHOWIM_DRAWDIBDRAW
	BITMAPINFOHEADER FAR *dib;
	HGLOBAL hDIB;

	hDIB = pxd_renderDIBCreate(1<<unit, buf, 0, 0, -1, -1, 0, 0);
	if (hDIB) {
	    if (dib = (BITMAPINFOHEADER FAR *)GlobalLock(hDIB)) {
		DrawDibDraw(hDrawDib, hDC, windImage[unit].nw.x, windImage[unit].nw.y,
					   windImage[unit].se.x-windImage[unit].nw.x, windImage[unit].se.y-windImage[unit].nw.y,
			    (BITMAPINFOHEADER *)dib,
			    (uchar FAR*)dib+dib->biSize+dib->biClrUsed*sizeof(RGBQUAD),
			    0, 0, pxd_imageXdim(), pxd_imageYdim(), 0);
		GlobalUnlock(hDIB);
	    }
	    pxd_renderDIBFree(hDIB);
	} else
	    MessageBox(NULL, "Error", "pxd_renderDIBCreate", MB_OK|MB_TASKMODAL);
    #endif

    //
    // Show image using PXIPL and Video for Windows.
    // Altho not used in this example, pxio8_DrawDibDisplay also allows drawing
    // a full screen cross hair cursor over the image.
    //
    #if SHOWIM_DRAWDIBDISPLAY
	err = pxio8_DrawDibDisplay(NULL, pxd_defineImage(1<<unit, buf, 0, 0, -1, -1, "Display"),
			 NULL, 0, 'n', 0, 0,hDrawDib,hDC,&windImage[unit], NULL, NULL);
	if (err < 0)
	    MessageBox(NULL, pxd_mesgErrorCode(err), "pxio8_DrawDibDisplay", MB_OK|MB_TASKMODAL);
    #endif

    ReleaseDC(hWndImage, hDC);
}

/*
 * Save all frame buffers in tiff format,
 * using one file per unit with multiple images per file.
 * This requires PXIPL.
 */
void SaveTiff1()
{
    #if USE_PXIPL
	int	err;
	for (int u = 0; u < UNITS; u++) {
	    OPENFILENAME ofn;
	    char    pathname[_MAX_PATH] = "";
	    int     r;
	    char    title[80];
	    memset(&ofn, 0, sizeof(ofn));
	    ofn.lStructSize = sizeof(ofn);
	    ofn.hwndOwner   = hWnd;
	    ofn.lpstrFilter = "TIFF Files (*.tif)\0*.tif\0\0";
	    ofn.lpstrFile   = pathname;
	    ofn.nMaxFile    = _MAX_PATH;
	    if (UNITS == 1)
		ofn.lpstrTitle	= "Save Sequence";
	    else {
		title[sizeof(title)-1] = 0; // this & snprintf: overly conservative - avoids warning messages
		_snprintf(title, sizeof(title)-1, "Save Sequence Unit %d", u);
		ofn.lpstrTitle	= title;
	    }
	    ofn.Flags	|= OFN_EXPLORER|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
	    r = GetSaveFileName(&ofn);
	    if (r != 0) {
		err = pxio8_tifwriteseq(NULL,
				      pxd_defineImage3(1<<u, 1, -1, 0, 0, -1, -1, "Default"),
				      NULL, pathname, pxd_imageBdim(), 0, 0, NULL);
		if (err < 0)
		    MessageBox(NULL, pxd_mesgErrorCode(err), "pxio8_tifwriteseq", MB_OK|MB_TASKMODAL);
	    }
	}
    #endif
}

/*
 * Save all frame buffers in tiff format,
 * using one file per unit per inage.
 * We won't prompt for each file name, but we do
 * check that none of the names to be used already exists.
 */
void SaveTiffN()
{
    int     err;
    OPENFILENAME ofn;
    char    pathname[_MAX_PATH] = "";
    char    pathname2[_MAX_PATH] = "";
    int     r;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hWnd;
    ofn.lpstrFilter = "TIFF Files (*.tif)\0*.tif\0\0";
    ofn.lpstrFile   = pathname;
    ofn.nMaxFile    = _MAX_PATH;
    ofn.lpstrTitle  = "Save Sequence Base Pathname";
    ofn.Flags	|= OFN_EXPLORER|OFN_HIDEREADONLY;
    r = GetSaveFileName(&ofn);
    if (r != 0) {
	char *p = strstr(pathname, ".tif");
	if (p)
	    *p = 0;
	p = strstr(pathname, ".TIF");
	if (p)
	    *p = 0;
	for (int u = 0; u < UNITS; u++) {
	    for (int z = 1; z <= pxd_imageZdim(); z++) {
		pathname2[sizeof(pathname2)-1] = 0; // this & snprintf: overly conservative - avoids warning messages
		_snprintf(pathname2, sizeof(pathname2)-1, "%s_unit%.2d_frame%.6d.tif", pathname, u, z);
		HANDLE h = CreateFile(pathname2, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (h != INVALID_HANDLE_VALUE) {
		    MessageBox(NULL, "File already exists", pathname2, MB_OK|MB_TASKMODAL);
		    CloseHandle(h);
		} else {
		    err = pxd_saveTiff(1<<u, pathname2, z, 0, 0, -1, -1, 0, 0);
		    if (err < 0)
			MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_saveTiff", MB_OK|MB_TASKMODAL);
		}
	    }
	}
    }
}

/*
 * Save all frame buffers in avi format,
 * using one file per unit with multiple images per file.
 * This requires PXIPL.
 */
void SaveAvi1()
{
    #if USE_PXIPL
	int	err;
	for (int u = 0; u < UNITS; u++) {
	    OPENFILENAME ofn;
	    char    pathname[_MAX_PATH] = "";
	    int     r;
	    char    title[80];
	    memset(&ofn, 0, sizeof(ofn));
	    ofn.lStructSize = sizeof(ofn);
	    ofn.hwndOwner   = hWnd;
	    ofn.lpstrFilter = "AVI Files (*.avi)\0*.avi\0\0";
	    ofn.lpstrFile   = pathname;
	    ofn.nMaxFile    = _MAX_PATH;
	    if (UNITS == 1)
		ofn.lpstrTitle	= "Save Sequence";
	    else {
		title[sizeof(title)-1] = 0; // this & snprintf: overly conservative - avoids warning messages
		_snprintf(title, sizeof(title)-1, "Save Sequence Unit %d", u);
		ofn.lpstrTitle	= title;
	    }
	    ofn.Flags	|= OFN_EXPLORER|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
	    r = GetSaveFileName(&ofn);
	    if (r != 0) {
		// Mark the .avi file with the desired playback frame rate.
		// For some cameras, this might be the actual frame rate
		// (altho this example doesn't show measuring the rate).
		// For others, such as high speed cameras we would
		// want to slow the rate, for triggered cameras
		// we might want to speed up the rate.
		pximfileinfo_s	parm;
		memset(&parm, 0, sizeof(parm));
		parm.fi.bmp.framecnt = 30;
		parm.fi.bmp.framesecs = 1;
		err = pxio8_aviwriteseq(NULL,
				      pxd_defineImage3(1<<u, 1, -1, 0, 0, -1, -1, "Default"),
				      NULL, pathname, 8, &parm);
		if (err < 0)
		    MessageBox(NULL, pxd_mesgErrorCode(err), "pxio8_aviwriteseq", MB_OK|MB_TASKMODAL);
	    }
	}
    #endif
}



/*
 * Save all frame buffers in simple binary format,
 * using one file per unit with multiple images per file,
 * without using PXIPL.
 * Checking for file I/O errors via the C/Windows library
 * is left as an exercise for the reader.
 */
void SaveBinary1()
{
    int     err;

    for (int u = 0; u < UNITS; u++) {
	OPENFILENAME ofn;
	char	pathname[_MAX_PATH] = "";
	int	r;
	char	title[80];
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner	= hWnd;
	ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0\0";
	ofn.lpstrFile	= pathname;
	ofn.nMaxFile	= _MAX_PATH;
	if (UNITS == 1)
	    ofn.lpstrTitle  = "Save Sequence";
	else {
	    title[sizeof(title)-1] = 0; // this & snprintf: overly conservative - avoids warning messages
	    _snprintf(title, sizeof(title)-1, "Save Sequence Unit %d", u);
	    ofn.lpstrTitle  = title;
	}
	ofn.Flags   |= OFN_EXPLORER|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
	r = GetSaveFileName(&ofn);
	if (r != 0) {
	    //
	    // We could read one entire image at a time into a large buffer,
	    // read one pixel at a time into a tiny buffer, or anything
	    // inbetween. Reading one line at a time is a common compromise.
	    //
	    void *buffer = malloc(pxd_imageXdim() * pxd_imageCdim() * (pxd_imageBdim()<=8? 1: 2));
	    FILE *fp = fopen(pathname, "wb");
	    if (!fp)
		MessageBox(NULL, "Can't create file", "fopen", MB_OK|MB_TASKMODAL);
	    if (!buffer)
		MessageBox(NULL, "Can't alloc memory", "malloc", MB_OK|MB_TASKMODAL);
	    if (fp && buffer) {
		for (int z = 1; z <= pxd_imageZdim(); z++) {
		    for (int y = 0; y < pxd_imageYdim(); y++) {
			if (pxd_imageBdim()<=8) {
			    err = pxd_readuchar(1<<u, z, 0, y, -1, y+1, (uchar*)buffer, pxd_imageXdim() * pxd_imageCdim(), pxd_imageCdim()==1?"Grey":"RGB");
			    if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_readuchar", MB_OK|MB_TASKMODAL);
			} else {
			    err = pxd_readushort(1<<u, z, 0, y, -1, y+1, (ushort*)buffer, pxd_imageXdim() * pxd_imageCdim(), pxd_imageCdim()==1?"Grey":"RGB");
			    if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_readushort", MB_OK|MB_TASKMODAL);
			}
			fwrite(buffer, pxd_imageXdim() * pxd_imageCdim(), (pxd_imageBdim()<=8? 1: 2), fp);
		    }
		}
	    }
	    if (fp)
		fclose(fp);
	    if (buffer)
		free(buffer);
	}
    }
}

int UpdateTextboxValue(HWND hDlg, WPARAM wParam, int* targetVar, int minVal, const char* successMessage, const char* errorMessage) {
	char buffer[32];

	// Retrieve the text from the textbox with the given ID in wParam
	GetDlgItemText(hDlg, LOWORD(wParam), buffer, sizeof(buffer));
	int newValue = atoi(buffer);

	// Validate based on minimum value requirement
	if (newValue >= minVal) {
		*targetVar = newValue; // Update the variable with the new value
		//MessageBox(NULL, successMessage, "Update", MB_OK | MB_TASKMODAL);
		return TRUE;
	}
	else {
		MessageBox(NULL, errorMessage, "Invalid Input", MB_OK | MB_TASKMODAL);
		return FALSE;
	}
}

int UpdateTextboxValue(HWND hDlg, WPARAM wParam, double* targetVar, double minVal, const char* successMessage, const char* errorMessage) {
	char buffer[32];

	// Retrieve the text from the textbox with the given ID in wParam
	GetDlgItemText(hDlg, LOWORD(wParam), buffer, sizeof(buffer));
	double newValue = atof(buffer);

	// Validate based on minimum value requirement
	if (newValue >= minVal) {
		*targetVar = newValue; // Update the variable with the new value
		//MessageBox(NULL, successMessage, "Update", MB_OK | MB_TASKMODAL);
		return TRUE;
	}
	else {
		MessageBox(NULL, errorMessage, "Invalid Input", MB_OK | MB_TASKMODAL);
		return FALSE;
	}
}


// Function to check and create a unique filename if the specified one already exists
int UpdateSavePath(HWND hDlg, WPARAM wParam, char* savePathVar, int bufferSize) {
	char buffer[100]; // Buffer to retrieve the text from the input field
	char uniquePath[100];
	COLORREF color = RGB(255, 255, 255); // Default background color (white)

	// Retrieve the text from the textbox
	GetDlgItemText(hDlg, LOWORD(wParam), buffer, sizeof(buffer));

	// Variables for the directory and filename parts
	char dirPath[100], fileName[50];

	// Check if the input is valid
	if (strlen(buffer) > 0) {
		// Split the path into directory and filename parts
		char dirPath[_MAX_DIR], fileName[_MAX_FNAME], ext[_MAX_EXT];
		_splitpath_s(buffer, NULL, 0, dirPath, sizeof(dirPath), fileName, sizeof(fileName), ext, sizeof(ext));

		// Construct the full directory path
		snprintf(dirPath, sizeof(dirPath), "%s", dirPath);

		// Check if the directory exists
		if (PathFileExists(dirPath)) {
			// Directory exists, check if file already exists to append a unique number if necessary
			snprintf(uniquePath, sizeof(uniquePath), "%s%s%s", dirPath, fileName, ext);

			// Append a number if the file exists
			int count = 1;
			while (PathFileExists(uniquePath) && count < 100) {
				snprintf(uniquePath, sizeof(uniquePath), "%s%s_%d%s", dirPath, fileName, count++, ext);
			}

			// Copy the unique path to the savePathVar
			strncpy(savePathVar, uniquePath, bufferSize - 1);
			savePathVar[bufferSize - 1] = '\0';

			// Update the textbox to show the new unique path
			SetDlgItemText(hDlg, LOWORD(wParam), uniquePath);

			// Reset to default color if everything is valid
			color = RGB(255, 255, 255);
			return TRUE;
		}
		else {
			// Directory doesn't exist, set error color and message
			color = RGB(255, 0, 0);  // Set background color to red
			MessageBox(NULL, "The specified directory path does not exist. Please enter a valid directory. (You may need to create the parent directory)", "Invalid Directory", MB_OK | MB_ICONERROR);
		}
	}
	else {
		MessageBox(NULL, "Please enter a valid file path.", "Invalid Path", MB_OK | MB_ICONERROR);
	}

	// Change the background color of the textbox to indicate an error if needed
	HBRUSH hBrush = CreateSolidBrush(color);
	SetClassLongPtr(GetDlgItem(hDlg, LOWORD(wParam)), GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
	InvalidateRect(GetDlgItem(hDlg, LOWORD(wParam)), NULL, TRUE); // Force a redraw

	return FALSE;
}

void SetDefaultSavePath(HWND hDlg, char* savePathVar, int bufferSize) {
	char documentsPath[MAX_PATH];
	
	// Retrieve the path to the user's Documents folder
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documentsPath))) {
		// Append the subdirectory and default filename
		std::string defaultPath = std::string(documentsPath) + "\\epix_data";

		// Ensure the directory exists (create if not present)
		_mkdir(defaultPath.c_str());

		// Add default filename
		defaultPath += "\\no_name";

		// Copy to char array (assuming `savePathVar` has sufficient buffer size)
		strncpy(savePathVar, defaultPath.c_str(), bufferSize);

		// Set the path in the UI textbox
		SetDlgItemText(hDlg, IDSAVEPATH, savePathVar);

		// Call UpdateSavePath to apply it as the save path
		UpdateSavePath(hDlg, IDSAVEPATH, savePathVar, bufferSize);
	}
	else {
		MessageBox(hDlg, "Could not retrieve Documents path.", "Error", MB_OK | MB_ICONERROR);
	}
}

// Function to check and create a unique filename if the specified one already exists
int UpdateSettingsPath(HWND hDlg, WPARAM wParam, char* savePathVar, int bufferSize) {
	char buffer[100]; // Buffer to retrieve the text from the input field
	char uniquePath[100];
	COLORREF color = RGB(255, 255, 255); // Default background color (white)
	bool ret = FALSE;

	// Retrieve the text from the textbox
	GetDlgItemText(hDlg, LOWORD(wParam), buffer, sizeof(buffer));

	// Variables for the directory and filename parts
	char dirPath[100], fileName[50];

	// Check if the directory portion of the path is valid
	if (strlen(buffer) > 0) {
		// Split the input into directory and filename parts
		_splitpath_s(buffer, NULL, 0, dirPath, sizeof(dirPath), fileName, sizeof(fileName), NULL, 0);

		// Construct the full directory path up to the filename level
		snprintf(dirPath, sizeof(dirPath), "%s\\", dirPath);

		if (PathFileExists(dirPath)) {
			// Directory exists, now check the filename
			snprintf(uniquePath, sizeof(uniquePath), "%s%s", dirPath, fileName);

			// Check if the file already exists
			if (PathFileExists(uniquePath)) {  // Check for existing files up to a limit
				color = RGB(255, 255, 255);  // Reset to default color
				ret = TRUE;
			}
			else {
				color = RGB(255, 100, 0);
			}
		}
		else {
			// Directory doesn't exist, set error color and message
			color = RGB(255, 0, 0);  // Set background color to red
			MessageBox(NULL, "The specified directory path does not exist. Please enter a valid directory.", "Invalid Directory", MB_OK | MB_ICONERROR);
		}
	}
	else {
		MessageBox(NULL, "Please enter a valid file path.", "Invalid Path", MB_OK | MB_ICONERROR);
		color = RGB(255, 0, 100);  // Set background color to reddish
	}

	// Change the background color of the textbox to indicate an error if needed
	HBRUSH hBrush = CreateSolidBrush(color);
	SetClassLongPtr(GetDlgItem(hDlg, LOWORD(wParam)), GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
	InvalidateRect(GetDlgItem(hDlg, LOWORD(wParam)), NULL, TRUE); // Force a redraw

	return ret;
}

int stopSeq(HWND hDlg, int* liveon, int* seqdisplayon) {

	//// Query camera running mode.
	//int mode;
	//mode = pxd_SILICONVIDEO_getVideoMode(UNITSMAP);
	//sprintf(str, "Video mode: %s\n", (char*)&mode);
	//MessageBox(NULL, (LPCSTR)str, "pxd_saveExp", MB_OK | MB_TASKMODAL);

	pxd_goUnLive(UNITSMAP);
	*liveon = FALSE;
	*seqdisplayon = FALSE;
	notInterrupted = false;

	EnableWindow(GetDlgItem(hDlg, IDLIVE), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSNAP), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSTOP), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDBUFFERSCROLL), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDQPDACQ), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEPATH), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSETTINGS), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEINT), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEEXP), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEAOIX), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEAOIY), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEAOIW), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDSAVEAOIH), TRUE);

	if (qpdacq) {
		//saveQpdq();
		qpdacq = FALSE;
	}

	return(TRUE);
}


int stopSeq(HWND hDlg, WPARAM wParam, int* liveon, int* seqdisplayon) {
	return stopSeq(hDlg, liveon, seqdisplayon);
}

int stopSeq(HWND hDlg) {
	int* dummy;
	return stopSeq(hDlg, dummy, dummy);
}

int updateResolution(HWND hDlg, double goalVal) {
	int unitmap = UNITSMAP;
	int subsample = pxd_SILICONVIDEO_getSubsample(unitmap);; // Initial value for subsample
	int scandirection = ('L' << 8) | 'T'; // Set scan direction
	int bitdepth = 10; // Set bit depth to 10
	double  pixelClkFreq = pxd_SILICONVIDEO_getPixelClock(unitmap);
	double  framePeriod = pxd_SILICONVIDEO_getFramePeriod(unitmap);

	// Define one-element arrays for AOI parameters, initialized with current global values
	int aoileft[1] = { aoi_x };
	int aoitop[1] = { aoi_y };
	int aoiwidth[1] = { aoi_w };
	int aoiheight[1] = { aoi_h };

	// Call the SDK function to correct values to the nearest valid settings
	int result = pxd_SILICONVIDEO_getMinMaxResolution(
		unitmap, &subsample, aoileft, aoitop, aoiwidth, aoiheight, &scandirection, &bitdepth, 0, 0	);

	// Check for errors in the SDK call
	if (result < 0) {
		MessageBox(hDlg, "Error correcting AOI resolution", "Resolution Update", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//
	// Set the AOI twice to get fastest video
	//
	result = pxd_SILICONVIDEO_setResolutionAndTiming(unitmap, 0, subsample, *aoileft, *aoitop, *aoiwidth,
		*aoiheight, scandirection, bitdepth, 0, 0, pixelClkFreq,
		framePeriod, 0, 0, 0);

	// Check for errors in the SDK call
	if (result < 0) {
		MessageBox(hDlg, "Error setting AOI resolution", "Resolution Update", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	framePeriod = pxd_SILICONVIDEO_getMinMaxFramePeriod(unitmap, goalVal);

	result = pxd_SILICONVIDEO_setResolutionAndTiming(unitmap, 0, subsample, *aoileft, *aoitop, *aoiwidth,
		*aoiheight, scandirection, bitdepth, 0, 0, pixelClkFreq,
		framePeriod, 0, 0, 0);

	// Check for errors in the SDK call
	if (result < 0) {
		MessageBox(hDlg, "Error setting AOI resolution", "Resolution Update", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Update global variables with validated values
	aoi_x = aoileft[0];
	aoi_y = aoitop[0];
	aoi_w = aoiwidth[0];
	aoi_h = aoiheight[0];

	// Format the validated AOI values as strings for display in the text boxes
	char buffer[32];

	snprintf(buffer, sizeof(buffer), "%d", aoi_x);
	SetDlgItemText(hDlg, IDSAVEAOIX, buffer);

	snprintf(buffer, sizeof(buffer), "%d", aoi_y);
	SetDlgItemText(hDlg, IDSAVEAOIY, buffer);

	snprintf(buffer, sizeof(buffer), "%d", aoi_w);
	SetDlgItemText(hDlg, IDSAVEAOIW, buffer);

	snprintf(buffer, sizeof(buffer), "%d", aoi_h);
	SetDlgItemText(hDlg, IDSAVEAOIH, buffer);

	// Calculate FPS based on framePeriod
	if (framePeriod > 0) {
		double fps = 1000.0 / framePeriod;
		snprintf(buffer, sizeof(buffer), "%.2f", fps);
	}
	else {
		snprintf(buffer, sizeof(buffer), "N/A");
	}

	// Display FPS in the non-editable text field
	SetDlgItemText(hDlg, IDFPS, buffer);

	pxd_goUnLive(UNITSMAP);
	pxd_goLive(UNITSMAP, 1L);

	// You can optionally display a success message or add further handling if needed
	return TRUE;
}

int updateResolution(HWND hDlg) {
	return updateResolution(hDlg, (double)0.0);
}

void processImage(unsigned short* frame, int width, int height, double* X, double* Y, double* Z) 
{
	// Ensure the frame is valid
	if (frame == nullptr || width <= 0 || height <= 0) {
		// Set output to NaN if input is invalid
		*X = std::nan("");
		*Y = std::nan("");
		*Z = std::nan("");
		return;
	}

	// Initialize sums for each quadrant
	double A = 0.0, B = 0.0, C = 0.0, D = 0.0;

	// Calculate the midpoint of the image
	int midX = width / 2;
	int midY = height / 2;

	// Iterate through each pixel to calculate A, B, C, and D
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			unsigned short pixelValue = frame[y * width + x];

			// Sum based on the quadrant
			if (x < midX && y < midY) {
				A += pixelValue; // Top-left
			}
			else if (x < midX && y >= midY) {
				B += pixelValue; // Bottom-left
			}
			else if (x >= midX && y < midY) {
				C += pixelValue; // Top-right
			}
			else {
				D += pixelValue; // Bottom-right
			}
		}
	}

	// Calculate Z, X, Y values
	Z[0] = A + B + C + D; // Total sum

	// Check to avoid division by zero
	if (Z[0] != 0) {
		X[0] = (A + B - C - D) / Z[0];
		Y[0] = (A + C - B - D) / Z[0];
	}
	else {
		// If Z is zero, set X and Y to NaN
		X[0] = std::nan("");
		Y[0] = std::nan("");
	}
}

// Create the save directory if it doesn't exist
void createSaveDirectory(const char* folderName) {
#ifdef _WIN32
	_mkdir(folderName);  // Windows
#else
	MessageBox(hDlg, "Could not make directory because I was compiled for windows. (ifdef _WIN32 evaluated false to get here, try manually making the directory you need)", "Error", MB_OK | MB_ICONERROR);
#endif
}

void saveData(const std::vector<std::unique_ptr<unsigned short[]>>& imageArray,
	int n_frames, int width, int height,
	const std::vector<double>& X,
	const std::vector<double>& Y,
	const std::vector<double>& Z)
{
	// Use this function at the start of your saveData function
	createSaveDirectory(savefilename);

	// Modify file paths to include the save directory
	std::string savePath(savefilename);

	// Set up file paths within the directory
	std::string tiffFilePath = savePath + "/image_data.tiff";
	std::string xFilePath = savePath + "/X_data.bin";
	std::string yFilePath = savePath + "/Y_data.bin";
	std::string zFilePath = savePath + "/Z_data.bin";
	std::string tFilePath = savePath + "/T_data.bin";

	// Open a single TIFF file to save all frames as a multipage TIFF
	TIFF* tiff = TIFFOpen(tiffFilePath.c_str(), "w");
	if (!tiff) {
		std::cerr << "Failed to create multipage TIFF file." << std::endl;
		return;
	}

	// Loop to write each frame as a separate page in the TIFF file
	for (int frameIdx = 0; frameIdx < n_frames; ++frameIdx) {
		// Set TIFF metadata for each frame (image directory/page)
		TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
		TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 16);
		TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

		// Write each row of the current frame to the TIFF file
		unsigned short* imageData = imageArray[frameIdx].get();
		for (int row = 0; row < height; ++row) {
			TIFFWriteScanline(tiff, &imageData[row * width], row, 0);
		}

		// Create a new directory (page) in the TIFF file for the next frame
		TIFFWriteDirectory(tiff);
	}

	// Close the TIFF file
	TIFFClose(tiff);
	std::cout << "Multipage TIFF file successfully saved." << std::endl;

	// 2. Create time stamps based on framePeriod
	double framePeriod = pxd_SILICONVIDEO_getFramePeriod(UNITSMAP) * 1e-3;
	std::vector<double> timeStamps(X.size());
	for (int i = 0; i < n_frames; ++i) {
		timeStamps[i] = i * framePeriod;
	}

	// 3. Save vectors X, Y, Z, and timeStamps as binary files
	auto saveVectorToBinaryFile = [](const std::vector<double>& vec, const std::string& filename) {
		std::ofstream outFile(filename, std::ios::binary);
		if (!outFile) {
			std::cerr << "Failed to open file for writing: " << filename << std::endl;
			return;
		}
		outFile.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(double));
		outFile.close();
		};

	saveVectorToBinaryFile(X, xFilePath);
	saveVectorToBinaryFile(Y, yFilePath);
	saveVectorToBinaryFile(Z, zFilePath);
	saveVectorToBinaryFile(timeStamps, tFilePath);

	std::cout << "Data and images successfully saved." << std::endl;
}

// Image processing function
void processImageThread(HWND hWndImage, struct pxywindow windImage[]) {

	// Retrieve the current path from the textbox
	char currentSavePath[100];
	GetDlgItemText(hDlg, IDSAVEPATH, currentSavePath, sizeof(currentSavePath));

	// Run UpdateSavePath to ensure a unique and valid path
	if (!UpdateSavePath(hDlg, MAKEWPARAM(IDSAVEPATH, EN_CHANGE), currentSavePath, sizeof(currentSavePath))) {
		MessageBox(NULL, "Error with save path. Please check the path.", "Save Path Error", MB_OK | MB_ICONERROR);
		return; // Abort acquisition if path is invalid
	}

	// Calculate total frames to save in memory and allocate
	int numSavedFrames = n_frames / save_int + 1;

	// Allocate memory for image buffer array (application memory for each saved frame)
	std::vector<std::unique_ptr<ushort[]>> imageArray(numSavedFrames);
	for (int i = 0; i < numSavedFrames; ++i) {
		imageArray[i] = std::make_unique<ushort[]>(aoi_w * aoi_h);
	}

	// Allocate memory for X, Y, Z arrays
	std::vector<double> X(n_frames);
	std::vector<double> Y(n_frames);
	std::vector<double> Z(n_frames);

	// Start processing frames
	int frameNumber = 0;
	int bufMax = pxd_imageZdim();
	
	DWORD lastFrameTime = GetTickCount();
	int initialFrameCount = pxd_videoFieldCount(UNITSMAP);
	pxbuffer_t buf = 0;
	processingNotFinished = TRUE;
	notInterrupted = true;

	while (frameNumber < n_frames && notInterrupted) {

		//
		// This is a gamble based on my understanding of C++, 
		// aided by chatGPT: the frame count on the card could
		// reach INT_MAX, in which case it will presumably loop
		// back to 0 if the live sequence continues. In which 
		// case chatGPT figured out how to handle it and I didn't
		// disagree with the logic.
		//
		const int MAX_FRAME_COUNT = INT_MAX; // Or another max if specified in your SDK

		// Check if there are new frames by calculating relative frame count
		int currentFrameCount = pxd_videoFieldCount(UNITSMAP);
		int relativeFrameCount;

		if (currentFrameCount >= initialFrameCount) {
			// No overflow case
			relativeFrameCount = currentFrameCount - initialFrameCount;
		}
		else {
			// Overflow case: the counter wrapped around
			relativeFrameCount = (MAX_FRAME_COUNT - initialFrameCount + 1) + currentFrameCount;
		}

		if (frameNumber >= relativeFrameCount) {
			// No new frames since the last check
			DWORD currentTime = GetTickCount();

			if (currentTime - lastFrameTime > 1000) {  // If over a second has passed with no new frames
				notInterrupted = false;  // Stop the acquisition
				MessageBox(NULL, "Acquisition error: No new frames received in over 1 second.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			else {
				Sleep(10);  // Wait a bit and then check again
				continue;
			}
		}

		// Update last frame time as a new frame has been detected
		lastFrameTime = GetTickCount();
		// Increment buffer count
		++buf;
		if (buf > bufMax)
			buf = 1;

		// Retrieve the next frame into the appropriate image buffer
		pxcoord_t ulx = aoi_x - 1;  // Upper-left x of AOI
		pxcoord_t uly = aoi_y - 1;  // Upper-left y of AOI
		pxcoord_t lrx = aoi_x + aoi_w;  // Lower-right x of AOI
		pxcoord_t lry = aoi_y + aoi_h;  // Lower-right y of AOI
		ushort* imageBuffer = imageArray[frameNumber / save_int].get();
		size_t cnt = (size_t)aoi_w * (size_t)aoi_h;

		int u = 0;

		int err = pxd_readushort(1 << u, buf, ulx, uly, -1, -1, imageBuffer, cnt, "Grey"); // There's good odds that this will be the line that errors if you use a colour camera.
		
		if (err < 0) {
			// Use a sentinel value to indicate an invalid frame
			for (int i = 0; i < aoi_w * aoi_h; ++i) {
				imageBuffer[i] = USHRT_MAX; // or any other sentinel value
			}

			// Fill the X, Y, Z vectors with NaN values
			X[frameNumber] = std::nan("");
			Y[frameNumber] = std::nan("");
			Z[frameNumber] = std::nan("");
		}
		else {
			// Process the image and store the result
			processImage(imageBuffer, aoi_w, aoi_h, &X[frameNumber], &Y[frameNumber], &Z[frameNumber]);
		}
		// Conditionally display the image every save_int frames
		if (frameNumber % save_int == 0) {
			DisplayBuffer(0, buf, hWndImage, windImage);  // Display the image on screen
			//
			// Let buffer scroll bar show sequence capture activity.
			// Especially useful in triggered sequence mode, as it
			// will show when the trigger has arrived and the delay
			// expired so as to let the sequence capture run.
			//
			SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, buf, TRUE);

		}

		frameNumber++;
	}

	// Call saveData function to handle saving the frame or analysis results
	saveData(imageArray, numSavedFrames - 1, aoi_w, aoi_h, X, Y, Z);

	// Signal completion of processing by resetting the thread flag
	processingThreadRunning = false;
	processingNotFinished = FALSE;
}

/*
 * The Dialog
 */
BOOL CALLBACK
PIXCIDialogProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT svgaBits;
	static int liveon = 0;
	static int seqdisplayon = 0;
	static pxbuffer_t seqdisplaybuf = 1;
	static DWORD seqdisplaytime;
	static pxvbtime_t lastcapttime[UNITS] = { 0 };
	static struct pxywindow windImage[max(4, UNITS)];
	static HWND hWndImage;
	int err = 0;
	char str[150];

	switch (wMsg) {
	case WM_INITDIALOG:
	{
		RECT	rectImage;

		//
		// Open the PIXCI(R) frame grabber.
		// If this program were to only support a single PIXCI(R)
		// frame grabber, the first parameter could be simplified to:
		//
		//	if (pxd_PIXCIopen("", FORMAT, NULL) < 0)
		//	    pxd__mesgFault(1);
		//
		// But, for the sake of multiple PIXCI(R) frame grabbers
		// specify which units are to be used.
		//
		char driverparms[80];
		driverparms[sizeof(driverparms) - 1] = 0; // this & snprintf: overly conservative - avoids warning messages
		_snprintf(driverparms, sizeof(driverparms) - 1, "-DM 0x%x %s", UNITSOPENMAP, DRIVERPARMS);
		//
		// Either FORMAT or FORMATFILE_LOAD or FORMATFILE_COMP
		// should have been selected above.
		//
#if defined(FORMAT)
		if (pxd_PIXCIopen(driverparms, FORMAT, "") < 0)
			pxd_mesgFault(UNITSMAP);
#elif defined(FORMATFILE_LOAD)
	//
	// The FORMATFILE can be read and loaded
	// during the pxd_PIXCIopen(), for convenience
	// of changing the format file without recompiling.
	//
		if (pxd_PIXCIopen(driverparms, "", FORMATFILE_LOAD) < 0)
			pxd_mesgFault(UNITSMAP);
#elif defined(FORMATFILE_COMP)
	//
	// Or the FORMATFILE can be compiled into this application,
	// reducing the number of files that must be distributed, or
	// possibly lost.
	//
	// Note: On MSVC 6.0, if the precompiled header option is used,
	// the compiler objects to this code (C2006) when FORMATFILE_COMP
	// is not defined, even though this shouldn't be compiled
	// when FORMATFILE_COMP is not defined.
	// Either turn off the 'Use Precompiled Headers' option,
	// remove this code, or choose to use the FORMATFILE_COMP option.
	//
		if (pxd_PIXCIopen(driverparms, "Default", "") < 0)
			pxd_mesgFault(UNITSMAP);
		{
#include FORMATFILE_COMP
			pxd_videoFormatAsIncludedInit(0);
			err = pxd_videoFormatAsIncluded(0);
			if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_videoFormatAsIncluded", MB_OK | MB_TASKMODAL);
		}
#endif

		//
		// Set our title.
		//
		SetWindowText(hDlg, "EPIX(R) PIXCI(R) Frame Grabber Example");

		//
		// Enable timer, for live video updates, checking for faults,
		// and timed display fo sequences.
		// See xclibex2.cpp for an alternate, using an Event
		// instead of a timer.
		//
		SetTimer(hDlg, 1, 5, NULL);

		//
		// Get handle to image display area of dialog,
		// then get its device context and size.
		//
		hWndImage = GetDlgItem(hDlg, IDIMAGE);
		{
			HDC  hDC = GetDC(hWndImage);
			GetClientRect(hWndImage, &rectImage);
			svgaBits = GetDeviceCaps(hDC, PLANES) * GetDeviceCaps(hDC, BITSPIXEL);
			ReleaseDC(hWndImage, hDC);
		}

		//
		// Determine displayed size.
		// We could simply fill up the hWndImage, but
		// much rather adjust the displayed image for
		// correct aspect ratio.
		//
		windImage[0].nw.x = windImage[0].nw.y = 0;
		windImage[0].se.x = rectImage.right + 1;		 // inclusive->exclusive
		windImage[0].se.y = rectImage.bottom + 1; 	 // inclusive->exclusive
		{
			double  scalex, scaley, aspect;
			aspect = pxd_imageAspectRatio();
			if (aspect == 0.0)
				aspect = 1.0;
			scalex = windImage[0].se.x / (double)pxd_imageXdim();
			scaley = windImage[0].se.y / ((double)pxd_imageYdim() * aspect);
			scalex = min(scalex, scaley);
			windImage[0].se.x = (int)(pxd_imageXdim() * scalex);
			windImage[0].se.y = (int)(pxd_imageYdim() * scalex * aspect);
		}

		//
		// Seems as good a place as any to save the image dimensions
		// so we know what the acceptable AOI range is.
		//
		imageWidth = pxd_imageXdim();
		imageHeight = pxd_imageYdim();

		//
		// Update textboxes with defaults
		//
		char buffer[32];

		// Set IDSAVEAOIW with the value of imageWidth
		snprintf(buffer, sizeof(buffer), "%d", imageWidth);
		SetDlgItemText(hDlg, IDSAVEAOIW, buffer);

		// Set IDSAVEAOIH with the value of imageHeight
		snprintf(buffer, sizeof(buffer), "%d", imageHeight);
		SetDlgItemText(hDlg, IDSAVEAOIH, buffer);

		// Set IDSAVEAOIX with the initial X position (0)
		snprintf(buffer, sizeof(buffer), "%d", 0);
		SetDlgItemText(hDlg, IDSAVEAOIX, buffer);

		// Set IDSAVEAOIY with the initial Y position (0)
		snprintf(buffer, sizeof(buffer), "%d", 0);
		SetDlgItemText(hDlg, IDSAVEAOIY, buffer);

		// Set IDSAVEEXP and IDSAVEINT too
		exp_ms = pxd_SILICONVIDEO_getExposure(UNITSMAP);
		snprintf(buffer, sizeof(buffer), "%f", exp_ms);
		SetDlgItemText(hDlg, IDSAVEEXP, buffer);

		snprintf(buffer, sizeof(buffer), "%d", save_int);
		SetDlgItemText(hDlg, IDSAVEINT, buffer);
		
		// Set IDNFRAMES with the initial number of frames
		snprintf(buffer, sizeof(buffer), "%d", n_frames);
		SetDlgItemText(hDlg, IDNFRAMES, buffer);

		// Set default savepath in dialog box.
		SetDefaultSavePath(hDlg, savefilename, sizeof(savefilename));

		//
		// If StrecthDIBits is to be used, some VGA card drivers
		// abhor horizontal dimensions which are not a multiple of 4.
		// This isn't needed for other rendering methods, but doesn't hurt.
		//
		windImage[0].se.x &= ~3;

		//
		// For multiple units, display each of four units
		// in quadrant of display area.
		//
		if (UNITS > 1) {
			windImage[0].se.x &= ~0xF;	 // See above StretchDIBits comment above
			windImage[1] = windImage[0];
			windImage[2] = windImage[0];
			windImage[3] = windImage[0];
			windImage[0].se.x /= 2;
			windImage[0].se.y /= 2;
			windImage[1].nw.x = windImage[1].se.x / 2;
			windImage[1].se.y /= 2;
			windImage[2].se.x /= 2;
			windImage[2].nw.y = windImage[2].se.y / 2;
			windImage[3].nw.x = windImage[3].se.x / 2;
			windImage[3].nw.y = windImage[3].se.y / 2;
		}

		//
		// Init dialog controls.
		//
		SetScrollRange(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, 1, pxd_imageZdim(), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDLIVE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSNAP), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSTOP), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDBUFFERSCROLL), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSEQSAVE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDQPDACQ), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEPATH), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSETTINGS), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEINT), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEEXP), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEAOIX), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEAOIY), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEAOIW), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDSAVEAOIH), TRUE);

		//
		// If using DirectDraw, initialize access to it.
		//
		// DirectDraw may not be available!
		// Error reporting should be added!
		//
#if SHOWIM_DIRECTXDISPLAY
		{
			HRESULT	    h;
			hDDLibrary = LoadLibrary("DDRAW");
			if (hDDLibrary) {
				typedef HRESULT(WINAPI* OPEN)(void FAR*, LPDIRECTDRAW FAR*, void FAR*);
				OPEN	lpfnDM;
				lpfnDM = (OPEN)GetProcAddress(hDDLibrary, "DirectDrawCreate");
				if (lpfnDM) {
					h = (*lpfnDM)(NULL, &lpDD, NULL);
					if (lpDD) {
						h = lpDD->SetCooperativeLevel((HWND)hWnd, DDSCL_NORMAL);
					}
				}
			}
		}
#endif

		//
		// If using Video for Windows, initialize access to it.
		//
#if SHOWIM_DRAWDIBDRAW || SHOWIM_DRAWDIBDISPLAY
		hDrawDib = DrawDibOpen();
#endif

		return(TRUE);
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDSNAP:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			liveon = FALSE;
			seqdisplaybuf = FALSE;
			err = pxd_goSnap(UNITSMAP, 1);
			if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_goSnap", MB_OK | MB_TASKMODAL);
			EnableWindow(GetDlgItem(hDlg, IDLIVE), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDSNAP), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDSTOP), FALSE);
			SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, 1, TRUE);
			return(TRUE);

		case IDLIVE:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			liveon = TRUE;
			seqdisplaybuf = FALSE;
			err = pxd_goLive(UNITSMAP, 1L);
			if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_goLive", MB_OK | MB_TASKMODAL);
			EnableWindow(GetDlgItem(hDlg, IDLIVE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSNAP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSTOP), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDBUFFERSCROLL), FALSE);
			SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, 1, TRUE);
			return(TRUE);

		case IDSTOP:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			return stopSeq(hDlg, &liveon, &seqdisplayon);

		case IDSEQCAPTURE:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			//
			// Reminder: The pxd_goLiveSeq and pxd_goLiveSeqTrig
			// return immediately with the sequence capture running
			// in the background. In the context of this example program,
			// being 'user-event-driven', we prefer not waiting for completion
			// of the sequence capture.
			//
			#if TRIG_START_SEQUENCE | TRIG_END_SEQUENCE | GPIN_START_SEQUENCE | GPIN_END_SEQUENCE
						err = pxd_goLiveSeqTrig(UNITSMAP,
							1,			// Starting image frame buffer
							pxd_imageZdim(),	// Ending image frame buffer
							1,			// Image frame buffer number increment
							0,			// Number of captured images
							1,			// Period between captured images
							0, 0,
			#if TRIG_START_SEQUENCE
							TRIG_START_GPIN, 0, TRIG_START_DELAY,
			#elif GPIN_START_SEQUENCE
							GPIN_START_GPIN, 1, GPIN_START_DELAY,
			#else
							0, 0, 0,
			#endif
							0, 0, 0, 0, 0, 0, 0,
			#if TRIG_END_SEQUENCE
							TRIG_END_GPIN, 0, TRIG_END_DELAY,
			#elif GPIN_END_SEQUENCE
							GPIN_END_GPIN, 1, GPIN_END_DELAY,
			#else
							0, 0, 0,
			#endif
							0, 0, 0, 0, 0, 0);
			#else
						err = pxd_goLiveSeq(UNITSMAP, 1, pxd_imageZdim(), 1, 0, 1);
			#endif
			if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_goLiveSeq", MB_OK | MB_TASKMODAL);
			liveon = FALSE;
			seqdisplayon = FALSE;
			EnableWindow(GetDlgItem(hDlg, IDLIVE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSNAP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSTOP), TRUE);
			// Enable scroll so it can show capture status;
			// it doesn't allow changing the currently capture buffer.
			EnableWindow(GetDlgItem(hDlg, IDBUFFERSCROLL), TRUE);
			SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, 1, TRUE);

			return(TRUE);

		case IDSEQDISPLAY:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			seqdisplaybuf = 1;
			liveon = FALSE;
			seqdisplayon = TRUE;
			seqdisplaytime = GetTickCount();
			EnableWindow(GetDlgItem(hDlg, IDLIVE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSNAP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSTOP), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDBUFFERSCROLL), FALSE);
			SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, 1, TRUE);
			return(TRUE);

		case IDSEQSAVE:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			//
			// Save multiple images in one binary file?
			#if SAVE_BINARY
						SaveBinary1();
			#endif
						//
						// Save multiple images in one TIFF file?
			#if USE_PXIPL&SAVE_TIFF
						SaveTiff1();
			#endif
						//
						// Save multiple images in sequence of TIFF files?
			#if !USE_PXIPL&SAVE_TIFF
						SaveTiffN();
			#endif
						//
						// Save multiple images in one AVI file?
			#if USE_PXIPL&SAVE_AVI
						SaveAvi1();
			#endif
			return(TRUE);

		case IDSAVEINT:
			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);
			
			err = UpdateTextboxValue(hDlg, wParam, &save_int, 1, "Save Interval updated successfully!", "Please enter a valid positive number for save interval.");
			if (~err) 
				return(TRUE);
			else 
				return FALSE;


		case IDSAVEEXP:
			//
			// I don't know if I should disbelieve this or the FPS calculation, 
			// but one of them is wrong because apparently I can use 5 ms exposure 
			// time and acquire more than 1,000 fps. Hmmm...
			//

			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);
			
			err = UpdateTextboxValue(hDlg, wParam, &exp_ms, 0, "Exposure time updated successfully!", "Please enter a valid positive number for exposure time.");
			if (!err)
				return FALSE;

			double exp_act, exp2;
			int err;

			updateResolution(hDlg, (double)9E99);

			// Step 2: Get the allowable exposure closest to exp_ms based on the max frame period
			exp_act = pxd_SILICONVIDEO_getMinMaxExposure(UNITSMAP, exp_ms);
			err = pxd_SILICONVIDEO_setExposure(UNITSMAP, 0, exp_act);
			exp2 = pxd_SILICONVIDEO_getExposure(UNITSMAP);
			if (err < 0) {
				MessageBox(NULL, "Failed to set exposure", "Error", MB_OK | MB_ICONERROR);
				return FALSE;
			}

			updateResolution(hDlg);

			//// Step 4: Display the updated information
			//sprintf(str, "Current Z dimension: %i\n", pxd_imageZdim());
			//MessageBox(NULL, (LPCSTR)str, "pxd_saveExp", MB_OK | MB_TASKMODAL);

			// Step 5: Check and notify if the requested exposure differs from the allowable exposure
			if (exp_act != exp_ms) {
				double rat = exp_act / exp_ms, inrat = exp_ms / exp_act;
				if (!(rat < 1.1 && inrat < 1.1)) {
					sprintf(str, "Failed to set exposure to: %f ms, closest allowed: %f ms\n", exp_ms, exp_act);
					MessageBox(NULL, (LPCSTR)str, "pxd_saveExp", MB_OK | MB_TASKMODAL);
				}
			}
			exp_ms = exp2;
			sprintf(str, "%f", exp_ms);
			SetDlgItemText(hDlg, IDSAVEEXP, str);  // Update the textbox to show the actual exposure set
			
			if (err < 0) {
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_saveExp", MB_OK | MB_TASKMODAL);
				return(FALSE);
			}
			
			return(TRUE);

		case IDNFRAMES:
			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);
			UpdateTextboxValue(hDlg, wParam, &n_frames, 0, "N frames updated successfully!", "Please enter a valid non-negative number for N frames.");

			err = updateResolution(hDlg);
			if (err)
				return(TRUE);
			else
				return FALSE;

		case IDSAVEAOIX:
			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);
			UpdateTextboxValue(hDlg, wParam, &aoi_x, 0, "AOI X updated successfully!", "Please enter a valid non-negative number for AOI X.");

			err = updateResolution(hDlg);
			if (err)
				return(TRUE);
			else
				return FALSE;

		case IDSAVEAOIY:
			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);
			UpdateTextboxValue(hDlg, wParam, &aoi_y, 0, "AOI Y updated successfully!", "Please enter a valid non-negative number for AOI Y.");
			err = updateResolution(hDlg);
			if (err)
				return(TRUE);
			else
				return FALSE;

		case IDSAVEAOIW:
			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);

			UpdateTextboxValue(hDlg, wParam, &aoi_w, 1, "Save AOI Width updated successfully!", "Please enter a valid positive number for AOI Width.");
			err = updateResolution(hDlg);
			if (err)
				return(TRUE);
			else
				return FALSE;

		case IDSAVEAOIH:
			if (HIWORD(wParam) != EN_KILLFOCUS)
				return(FALSE);
			UpdateTextboxValue(hDlg, wParam, &aoi_h, 1, "Save AOI Height updated successfully!", "Please enter a valid positive number for AOI Height.");
			err = updateResolution(hDlg);
			if (err)
				return(TRUE);
			else
				return FALSE;

		case IDSAVEPATH:
			if (HIWORD(wParam) != EN_KILLFOCUS)  // Ensure it only updates on losing focus
				return FALSE; 

			err = UpdateSavePath(hDlg, wParam, savefilename, sizeof(savefilename));
			return err;

		case IDSETTINGS:
			if (HIWORD(wParam) != EN_KILLFOCUS)  // Ensure it only updates on losing focus
				return FALSE;
			err = UpdateSavePath(hDlg, wParam, settfilename, sizeof(settfilename)); 

			return err;

		case IDQPDACQ:
			if (HIWORD(wParam) != BN_CLICKED)
				return(FALSE);
			//
			// Reminder: The pxd_goLiveSeq and pxd_goLiveSeqTrig
			// return immediately with the sequence capture running
			// in the background. In the context of this example program,
			// being 'user-event-driven', we prefer not waiting for completion
			// of the sequence capture.
			//
			#if TRIG_START_SEQUENCE | TRIG_END_SEQUENCE | GPIN_START_SEQUENCE | GPIN_END_SEQUENCE
						err = pxd_goLiveSeqTrig(UNITSMAP,
							1,			// Starting image frame buffer
							pxd_imageZdim(),	// Ending image frame buffer
							1,			// Image frame buffer number increment
							0,			// Number of captured images
							1,			// Period between captured images
							0, 0,
			#if TRIG_START_SEQUENCE
							TRIG_START_GPIN, 0, TRIG_START_DELAY,
			#elif GPIN_START_SEQUENCE
							GPIN_START_GPIN, 1, GPIN_START_DELAY,
			#else
							0, 0, 0,
			#endif
							0, 0, 0, 0, 0, 0, 0,
			#if TRIG_END_SEQUENCE
							TRIG_END_GPIN, 0, TRIG_END_DELAY,
			#elif GPIN_END_SEQUENCE
							GPIN_END_GPIN, 1, GPIN_END_DELAY,
			#else
							0, 0, 0,
			#endif
							0, 0, 0, 0, 0, 0);
			#else
			err = pxd_goLiveSeq(UNITSMAP, 1, pxd_imageZdim(), 1, (pxbuffer_t) n_frames, 1);
			#endif
			if (err < 0)
				MessageBox(NULL, pxd_mesgErrorCode(err), "pxd_goLiveSeq", MB_OK | MB_TASKMODAL);
			liveon = FALSE;
			seqdisplayon = FALSE;
			processingNotFinished = TRUE;
			qpdacq = TRUE;
			EnableWindow(GetDlgItem(hDlg, IDLIVE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSNAP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQCAPTURE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSEQDISPLAY), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSTOP), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDQPDACQ), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEPATH), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSETTINGS), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEINT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEEXP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEAOIX), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEAOIY), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEAOIW), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDSAVEAOIH), FALSE);

			// Enable scroll so it can show capture status;
			// it doesn't allow changing the currently capture buffer.
			EnableWindow(GetDlgItem(hDlg, IDBUFFERSCROLL), TRUE);
			SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, 1, TRUE);
			return(TRUE);
		}
		//break;

	 case WM_HSCROLL:
	 {
		 HWND hCtrl = (HWND)lParam;
		 switch (GetWindowLong(hCtrl, GWL_ID)) {
		 case IDBUFFERSCROLL:
		 {
			 if (liveon) {
				 SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, pxd_capturedBuffer(1), TRUE);
			 }
			 else {
				 pxbuffer_t b = seqdisplaybuf;
				 switch (LOWORD(wParam)) {
				 case SB_PAGEDOWN:	b += 5; 		break;
				 case SB_LINEDOWN:	b += 1; 		break;
				 case SB_PAGEUP: 	b -= 5; 		break;
				 case SB_LINEUP: 	b -= 1; 		break;
				 case SB_TOP:		b = pxd_imageZdim();	break;
				 case SB_BOTTOM: 	b = 1;			break;
				 case SB_THUMBPOSITION:
				 case SB_THUMBTRACK:	b = HIWORD(wParam); break;
				 default:
					 return(FALSE);
				 }
				 b = max(1, min(pxd_imageZdim(), b));
				 SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, b, TRUE);
				 seqdisplaybuf = b;
				 if (!seqdisplayon)
					 for (int u = 0; u < UNITS; u++)
						 DisplayBuffer(u, b, hWndImage, windImage);
			 }
			 return(TRUE);
		 }
		 }
		 return(FALSE);
	 }

	 case WM_CLOSE:
		 pxd_PIXCIclose();
		 //DestroyWindow(GetParent(hDlg));
		#if SHOWIM_DIRECTXDISPLAY
			 if (lpDD)
				 lpDD->Release();
			 if (hDDLibrary)
				 FreeLibrary(hDDLibrary);
			 lpDD = NULL;
			 hDDLibrary = NULL;
		#endif
		#if SHOWIM_DRAWDIBDRAW || SHOWIM_DRAWDIBDISPLAY
			 if (hDrawDib)
				 DrawDibClose(hDrawDib);
			 hDrawDib = NULL;
		#endif

		 DestroyWindow(hWnd);
		 EndDialog(hDlg, 0);
		 return(TRUE);

	 case WM_TIMER:
		 //
		 // Monitor for asynchronous faults, such as video
		 // being disconnected while capturing. These faults
		 // can't be reported by functions such as pxd_goLive()
		 // which initiate capture and return immediately.
		 //
		 // Should there be a fault and pxd_mesgFault() pop up a dialog,
		 // the Windows TIMER will continue in a new thread. Thus the
		 // 'faulting' variable and logic to limit to one dialog at a time.
		 //
		 if (pxd_infoUnits()) {	 // implies whether library is open
			 static int faulting = 0;
			 if (!faulting) {
				 faulting++;
				 pxd_mesgFault(UNITSMAP);
				 faulting--;
			 }
		 }

		 //
		 // Has a new field or frame been captured
		 // since the last time we checked?
		 // Or, in sequence display mode, is it
		 // time to display the next image?
		 //
		 // In sequence capture, the PIXCI driver is handles
		 // switching from one capture buffer to the next -
		 // this need only monitor the result.
		 // During sequence display, this determines when,
		 // and it what order, each previously captured buffer
		 // should be displayed.
		 //
		 // In virtual QPD Acquisition, a thread is started
		 // which processes the images as they are being 
		 // acquired. This will then save the data when the 
		 // acquisition is finished or interrupted.
		 //
		 pxbuffer_t  buf = 1;
		 if (qpdacq) {
			 int u = 0;  // Assume single camera for fast acquisition

			 // Timeout check to allow acquisition start before attempting to get a frame
			 if (seqdisplaytime + 500 > GetTickCount()) {
				 break;  // No display yet
			 }

			 // Start image processing thread only if one isn’t already running
			 if (!processingThreadRunning.load() && processingNotFinished) {
				 processingThreadRunning = true;  // Set the flag to indicate processing has started
				 processingNotFinished = TRUE;
				 // Launch the image processing function in a separate thread
				 std::thread(processImageThread, hWndImage, windImage).detach();
			 }
			 else if (!processingNotFinished && !processingThreadRunning)
			 {
				 stopSeq(hDlg, wParam, &liveon, &seqdisplayon);
			 }

		 }
		 else {
			 for (int u = 0; u < UNITS; u++) {
				 if (seqdisplayon) {
					 if (u == 0) {
						 if (seqdisplaytime + 500 > GetTickCount())
							 break;	// no display yet, all units.
						 seqdisplaytime = GetTickCount();
						 buf = seqdisplaybuf++;
						 if (seqdisplaybuf > pxd_imageZdim())
							 seqdisplaybuf = 1;
					 }
				 }
				 else {
					 pxvbtime_t lasttime = pxd_capturedFieldCount(1 << u);
					 if (lastcapttime[u] == lasttime)
						 continue;
					 lastcapttime[u] = lasttime;
					 buf = pxd_capturedBuffer(1 << u);
				 }
				 DisplayBuffer(u, buf, hWndImage, windImage);
				 //
				 // Let buffer scroll bar show sequence capture activity.
				 // Especially useful in triggered sequence mode, as it
				 // will show when the trigger has arrived and the delay
				 // expired so as to let the sequence capture run.
				 //
				 SetScrollPos(GetDlgItem(hDlg, IDBUFFERSCROLL), SB_CTL, buf, TRUE);
			 }
		 }

		 return(TRUE);

	}
	return(FALSE);
}

LRESULT CALLBACK MainWndProc(
    HWND	hWnd,
    unsigned	message,
    WPARAM	wParam,
    LPARAM	lParam
){
    switch (message) {
	case WM_CLOSE:
	    DestroyWindow(hWnd);
	    return(0);
	case WM_DESTROY:
	    PostQuitMessage(0);
	    return(0);
    }
    return(DefWindowProc(hWnd, message, wParam, lParam));
}

/*
 * The Main
 */
int APIENTRY WinMain(
    HINSTANCE  hInstance,
    HINSTANCE  hPrevInstance,
    LPSTR      lpCmdLine,
    int        nCmdShow
){
    MSG       msg;
    WNDCLASS  wc;

    wc.style	     = CS_BYTEALIGNWINDOW;
    wc.lpfnWndProc   = MainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon	     = 0;
    wc.hCursor	     = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  =	NULL;
    wc.lpszClassName = "PXlibWClass";

    if (!RegisterClass(&wc))
	return (FALSE);

    if (hPrevInstance)
	return(FALSE);

    hWnd = CreateWindow("PXlibWClass",
			"XCLIBEX5 Windows Example",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			0, 0, hInstance, NULL );
    if (!hWnd)
	return (FALSE);

    //
    // This main window of this example does nothing useful.
    // Don't bother showing it.
    //
    //ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);
    hDlg = CreateDialogParam(hInstance, "PIXCIDIALOG", NULL, (DLGPROC)PIXCIDialogProc, NULL);
    if (!hDlg) {
	MessageBox(NULL, "Missing Dialog Resource - Compilation or Link Error!", "XCLIBEX4", MB_OK|MB_TASKMODAL);
	return(FALSE);
    }

    while (GetMessage(&msg, 0, 0, 0)) {
	if (!hDlg || !IsDialogMessage(hDlg, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }
    return ((int)msg.wParam);
}
