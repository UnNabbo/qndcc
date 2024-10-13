@echo off

set quiet=1

set project=Compiler
set output_dir=out

if %quiet%==0 (
	echo Starting compilation at %TIME%
	echo:
)

del /f /q "%output_dir%"
if not exist "%output_dir%" mkdir "%output_dir%"

set nec_flags= /nologo /FC /Fo:%output_dir%/ /Fd:%output_dir%/ /Fe:%output_dir%/%project% /std:c++20

if "%1"=="release" (
	set flags= /O2
) 

if "%1"=="debug" (
	set flags= /Zi
	set defines=%defines% /DDEBUG
) 

set defines=/DOS_WINDOWS
set libs=kernel32.lib user32.lib gdi32.lib Ole32.lib Dwmapi.lib Shcore.lib

set compilation_string=%nec_flags% src/main.c %libs% %flags% %defines%  /link /NODEFAULTLIB:library /LIBPATH:third_party/lib 

if %quiet%==0 echo cl %compilation_string%

cl %compilation_string%

if %errorlevel% == 0 (
    pushd "%output_dir%"
	echo:
	%Project%.exe
	echo:
	popd
)

if %quiet%==0 (
	echo Finishing compilation at %TIME%
	echo:
)
