#pragma once
#include <intrin.h>

#if defined( _DEBUG )

    extern bool AssertFunction( bool, char*, int, char*);

    #define ASSERT( exp ) \
        if ( AssertFunction( (int)(exp), #exp, __LINE__, __FILE__ ) ) \
        { __debugbreak(); }

    #define ASSERTD( exp, description ) \
        if ( AssertFunction( (int)(exp), description, __LINE__, __FILE__ ) )\
        { __debugbreak(); }

#else

    #define ASSERT( exp )
    #define ASSERTD( exp, description )

#endif // _DEBUG