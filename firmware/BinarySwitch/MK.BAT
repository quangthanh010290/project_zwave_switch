@echo off
rem *******************************  mk.bat  *******************************
rem            ####### 
rem            ##  ## 
rem            #  ##    ####   #####    #####  ##  ##   ##### 
rem              ##    ##  ##  ##  ##  ##      ##  ##  ##     
rem             ##  #  ######  ##  ##   ####   ##  ##   ####  
rem            ##  ##  ##      ##  ##      ##   #####      ## 
rem           #######   ####   ##  ##  #####       ##  #####  
rem                                            #####
rem           Z-Wave, the wireless language.
rem                                            
rem               Copyright (c) 2001
rem               Zensys A/S
rem               Denmark
rem 
rem               All Rights Reserved
rem 
rem     This source file is subject to the terms and conditions of the
rem     Zensys Software License Agreement which restricts the manner
rem     in which it may be used.
rem
rem ---------------------------------------------------------------------------
rem
rem  Description: Make bat file for building LED dimmer
rem
rem  Author:   Peter Shorty
rem 
rem  Last Changed By:  $Author: psh $
rem  Revision:         $Revision: 8640 $
rem  Last Changed:     $Date: 2007-02-09 14:25:01 +0100 (fr, 09 feb 2007) $
rem 
rem ****************************************************************************

if "%KEILPATH%"==""  goto usage_keil
if "%TOOLSDIR%"=="" goto usage_tools

if not exist %KEILPATH%\bin\c51.exe goto usage_keil
if not exist %KEILPATH%\bin\cx51.exe goto no_ext

set oldpath=%path%
set path=%KEILPATH%\bin;%TOOLSDIR%\Python;%path%

%TOOLSDIR%\Make\make %1 %2 %3

set path=%oldpath%
set oldpath=
goto exit

:usage_keil
@echo Set KEILPATH to point to the location of the Keil Compiler
@echo e.g c:\keil\c51
goto exit

:usage_tools
@echo Set TOOLSDIR to point to the location of the Z-Wave tools
@echo e.g c:\projects\zensys\devkit\tools
goto exit

:no_ext
@echo This developers kit requires the Keil PK51 Professional Developer's Kit

:exit