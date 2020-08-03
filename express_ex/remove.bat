
for /d %%i in (*) do (
  if not "%%i"=="sources" (if not "%%i"=="include"  if not "%%i"==".vs"  RMDIR /q  /s  %%i )
)

for  %%i in (*) do (
  if not "%%i"=="CMakeLists.txt" (if not "%%i"=="cmake_.bat" if not "%%i"=="remove.bat" erase /Q %%i )
)



