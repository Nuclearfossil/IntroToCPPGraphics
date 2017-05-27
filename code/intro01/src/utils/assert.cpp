#include "windows.h"
#include "stdio.h"

bool AssertFunction( bool test, char* desc, int line, char* file )
{
    static char outputbuffer[2048];
    if (!test)
    {
        sprintf(outputbuffer, "%s line:[%d] file: %s", desc, line, file);
        OutputDebugStringA(outputbuffer);
    }

    return !test;
}