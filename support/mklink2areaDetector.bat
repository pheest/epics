rmdir areaDetector

SET A_V=%1%
if "%A_V%"=="" SET A_V=R2-4
mklink /D areaDetector areaDetector%A_V%
