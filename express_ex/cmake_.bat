set path=%path%;"C:\Program Files\CMake\bin"


for /d %%i in (*) do (
  if not "%%i"=="sources" (if not "%%i"=="include" (if not "%%i"=="test"  (if not "%%i"=="x86"  (if not "%%i"=="x64"  if not "%%i"==".vs"  RMDIR /q  /s  %%i ))))
)

for  %%i in (*) do (
  if not "%%i"=="CMakeLists.txt"  (if not "%%i"=="CMakeMacro.txt" (if not "%%i"=="cmake_.bat" if not "%%i"=="remove.bat" erase /Q %%i ))
)

cd x64

for /d %%i in (*) do (
  if not "%%i"=="Release"  if not "%%i"=="Debug"  RMDIR /q  /s  %%i 
)

for  %%i in (*) do (
  erase /Q %%i 
)

cmake  -G "Visual Studio 16 2019"  ..

cd ..\x86

for /d %%i in (*) do (
  if not "%%i"=="Release"  if not "%%i"=="Debug"  RMDIR /q  /s  %%i 
)

for  %%i in (*) do (
  erase /Q %%i 
)

cmake  -G "Visual Studio 16 2019" -A Win32 ..