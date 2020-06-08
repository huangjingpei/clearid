# Microsoft Developer Studio Project File - Name="clearId" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=clearId - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "clearId.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "clearId.mak" CFG="clearId - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "clearId - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "clearId - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "clearId - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CLEARID_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CLEARID_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "clearId - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CLEARID_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\aee++" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CLEARID_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug\00001780.app" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "clearId - Win32 Release"
# Name "clearId - Win32 Debug"
# Begin Group "AEE++"

# PROP Default_Filter ""
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\AEE++\util\AEEBase64.cpp"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEBase64.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEDes.cpp"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEDes.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEHeap.cpp"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEHeap.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEList.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEMap.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEMd5.cpp"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEMd5.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEStack.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEStringRef.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEUtf8Stream.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEVector.h"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEZipPackage.cpp"
# End Source File
# Begin Source File

SOURCE="..\AEE++\util\AEEZipPackage.h"
# End Source File
# End Group
# Begin Source File

SOURCE="..\aee++\AEEBase.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEBase.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEBaseApp.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEBaseApp.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEBuffer.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEDelegate.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEDelegate.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFile.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFile.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFileFind.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFileFind.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFileMgr.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFileMgr.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEFixedBuffer.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEHttp.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEHttp.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEHttpDownload.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEHttpDownload.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEMultiMedia.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEMultiMedia.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEESocket.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEESocket.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEStackAlloc.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEStackAlloc.h"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEString.cpp"
# End Source File
# Begin Source File

SOURCE="..\aee++\AEEString.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\AudioPlayouter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AudioPlayouter.h
# End Source File
# Begin Source File

SOURCE=.\src\AudioRecorder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AudioRecorder.h
# End Source File
# Begin Source File

SOURCE=.\src\ClearIdApp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ClearIdApp.h
# End Source File
# Begin Source File

SOURCE=.\src\GlobalDef.h
# End Source File
# Begin Source File

SOURCE=.\src\PhoneChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PhoneChannel.h
# End Source File
# Begin Source File

SOURCE=.\src\SignalClient.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SignalClient.h
# End Source File
# Begin Source File

SOURCE=.\src\TcpTransport.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TcpTransport.h
# End Source File
# Begin Source File

SOURCE=.\src\VideoCapture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\VideoCapture.h
# End Source File
# Begin Source File

SOURCE=.\src\VideoRender.cpp
# End Source File
# Begin Source File

SOURCE=.\src\VideoRender.h
# End Source File
# End Target
# End Project
