﻿/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        launch/Main.cpp
*  PURPOSE:     Unchanging .exe that doesn't change
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the launch/output diectory.

    The .exe in launch/output will be used by the installer and updater.

    (set flag.newexe on the build server to generate new exe)
*/

//
// Helper function(s)
//
SString GetLaunchPath ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, NUMELMS(szBuffer) - 1 );

    // Strip the module name out of the path.
    PathRemoveFileSpec ( szBuffer );

    return szBuffer;
}


///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    // Load the loader.dll and continue the load
#ifdef MTA_DEBUG
    SString strLoaderDllFilename = "loader_d.dll";
#else
    SString strLoaderDllFilename = "loader.dll";
#endif

    SString strMTASAPath = PathJoin ( GetLaunchPath (), "mta" );
    SString strLoaderDllPathFilename = PathJoin ( strMTASAPath, strLoaderDllFilename );

    bool bUnicodeCharacters = strMTASAPath.Contains( "?" );
    for( uint i = 0 ; i < strMTASAPath.size() ; i++ )
    {
        char c = strMTASAPath[i];
        if ( c < 0 )
            bUnicodeCharacters = true;
    }

    if ( bUnicodeCharacters )
    {
        SString strMessage = "WARNING: Install path contains unicode characters\n\n";
        strMessage += "If MTA fails to load, please reinstall with basic Latin characters.\n\n";
        AddReportLog ( 5712, strMessage );
        BrowseToSolution ( "unicode-mta-path", ASK_GO_ONLINE | TERMINATE_IF_YES, strMessage );
    }

    // Load loader dll
    HMODULE hModule = LoadLibrary ( strLoaderDllPathFilename );

    int iReturnCode = 0;
    if ( hModule )
    {
        // Find and call DoWinMain
        typedef int (*PFNDOWINMAIN) ( HINSTANCE, HINSTANCE, LPSTR, int );
        PFNDOWINMAIN pfnDoWinMain = static_cast < PFNDOWINMAIN > ( static_cast < PVOID > ( GetProcAddress ( hModule, "DoWinMain" ) ) );
        if ( pfnDoWinMain )
            iReturnCode = pfnDoWinMain ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );

        FreeLibrary ( hModule );
    }
    else
    {
        SString strError = GetSystemErrorMessage ( GetLastError () );
        SString strMessage ( "Failed to load: '%s'\n\n%s", *strLoaderDllPathFilename, *strError );
        AddReportLog ( 5711, strMessage );
        BrowseToSolution ( "loader-dll-missing", ASK_GO_ONLINE, strMessage );
        iReturnCode = 1;
    }

    return iReturnCode;
}
