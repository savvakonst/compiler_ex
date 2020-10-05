win32 {

  Release:_CONFIG = Release
  Debug:_CONFIG = Debug
  Debug:LIBPOSTFIX = d

  contains(QMAKE_TARGET.arch, x86_64) {
    _PLATFORM = "x64"
  } else {
    _PLATFORM = "x86"
  }

}
