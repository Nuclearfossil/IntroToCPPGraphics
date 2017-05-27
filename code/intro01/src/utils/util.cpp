#include <direct.h>
#include <io.h>
#include <windows.h>
#include "utils\assert.h"

const int kMaxPath = 1024;
static char sWorkingPath[kMaxPath];

bool Exists(const char* filename)
{
    bool result = false;
    ASSERT(filename != nullptr);

    if (_access(filename, 0) == 0)
        result = true;

    return result;
}

const char* Pwd()
{
    _getcwd(sWorkingPath, kMaxPath);

    return sWorkingPath;
}
