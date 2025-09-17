rmdir base

if "%1"=="" goto blank
mklink /D base %1
goto end
:blank
mklink /D /J base base-7.0.9
REM mklink /D configure base-7.0.6\configure
:end
