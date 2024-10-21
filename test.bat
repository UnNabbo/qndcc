@echo off
setlocal

set stage_number=%1
if [%1]==[] set stage_number=1

echo:
echo Valid Tests:
echo:

set out_path=asm_out\stage_%stage_number%\valid
if exist "%out_path%\" rd /s /q %out_path%
for /R %%i in (stage_%stage_number%\valid\*.c) do call :ValidCase %%~ni

echo:
echo Invalid Tests:
echo:
set counter=1

set out_path=asm_out\stage_%stage_number%\invalid
if exist "%out_path%\" rd /s /q %out_path%
for /R %%i in (stage_%stage_number%\invalid\*.c) do call :InvalidCase %%~ni 

exit /B %errorlevel%

:ValidCase
if not exist "%out_path%\%1\" mkdir %out_path%\%1\
echo %1 test:
out\cc.exe tests\stage_%stage_number%\valid\%1.c -Fo %out_path%\%1\
echo output: %errorlevel%
echo: 
%out_path%\%1\%1.exe
echo: 
echo exit_code: %errorlevel%
echo: 
exit /B 0


:InvalidCase
if not exist "%out_path%\%1\" mkdir %out_path%\%1\
echo %1 test:
out\cc.exe tests\stage_%stage_number%\invalid\%1.c -Fo %out_path%\%1\
echo: 
exit /B 0

