#pragma once

// A great way to fix the suckage of #if vs #ifdef vs #ifndef
// If you end up using the macro USING on an undefined variable, you end
// up with a compiler error:
//    fatal error C1012: unmatched parenthesis : missing ')'
//
// To use this, create your defines like so:
// #define PLATFORM_MSWINDOWS ON
// #define PLATFORM_OSX OFF

#define ON		2-
#define OFF     1-
#define USING(x) ((x 0) == 2)

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

//
// File utilities
bool Exists(const char* filename);

const char* Pwd();
