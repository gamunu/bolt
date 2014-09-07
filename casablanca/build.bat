@echo OFF
cd "%~dp0"
echo Available Arguments:
echo [64^|32] Bit Architecture (optional, 64 is default)
echo [2008^|2010^|2012] VS Version(optional, 2012 is default)
echo [Clean] optional


SETLOCAL
:SetDefaultArguments
set Project=Casablanca
set CleanCMake=OFF

:: Default Generator
set GeneratorVersion=Visual Studio 12
set GeneratorArchitecture=Win64
set CMake_Arch64Bit=ON
set Release_Type=Release 
:: Folder arguments
set CMake64_Folder_Suffix=_x64
set CMakeGenVersion_FolderSuffix=

:: Parse over argumets using loop and shift
:Loop
	if /I "%1"=="" GOTO EndLoop
	
	
	if /I "%1"=="Clean" (
	echo "Clean make specified"
	set CleanCMake=ON
	)

	:: Visual Studio Version
	if /I "%1"=="2010" (
	echo "Generator VS 2010 specified"
	set GeneratorVersion=Visual Studio 10
	set CMakeGenVersion_FolderSuffix=
	)

	if /I "%1" == "Release" (
		echo "Build type release"
		set Release_Type=Release
		)
	
	if /I "%1" == "Debug" (
		echo "Build type Debug"
		set Release_Type=Debug
		)

	if /I "%1"=="32" (
	set GeneratorArchitecture=Win32
	set CMake_Arch64Bit=OFF
	)

	if /I "%1"=="64" (
	echo 64 bit specified
	set CMake_Arch64Bit=ON
	)
	
	shift
	goto Loop

:EndLoop

if "%CMake_Arch64Bit%"=="ON" (
set CMake64_Folder_Suffix=_x64
) else (
set CMake64_Folder_Suffix=
)

:: CMake Folder specified:
set CMakeFolder="WIN%CMake64_Folder_Suffix%"
echo CMakeFolder: %CMakeFolder%

:: Create generator type from VS version and architecture
if "%GeneratorArchitecture%"=="Win32" (
::32 Bit has no generator argument
set GENERATOR=%GeneratorVersion%
) else (
:: Is ARM or Win64
set GENERATOR=%GeneratorVersion% %GeneratorArchitecture%
)
echo Generator used: %GENERATOR%

:: Delete old cmake folder on "clean"
if "%CleanCMake%"=="ON" (
echo deleting folder %CleanCMake%
if exist %CMakeFolder% rmdir /S /Q %CMakeFolder%
)

mkdir %CMakeFolder%
cd %CMakeFolder%
echo cmake ../. -G"%GENERATOR%" -DCMAKE_CL_64=%CMake_Arch64Bit% -DCMAKE_BUILD_TYPE=%Release_Type%
cmake ../. -G"%GENERATOR%" -DCMAKE_CL_64=%CMake_Arch64Bit% -DCMAKE_BUILD_TYPE=%Release_Type%
CMAKE --build . --config %Release_Type%

xcopy /E /V %Release_Type%  %WINDIR%\System

if errorlevel 1 goto error
goto ok

:error
cd ../..
echo Failed %Project% build cmake.
echo "Exiting build.bat"
pause
exit /B 1

:ok
echo Success %Project% build cmake.
cd ../..
echo "Exiting build.bat"
pause
exit /B 0
