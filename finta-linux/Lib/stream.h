#include <stddef.h>
#ifndef __finta_Stream__
#define __finta_Stream__

class Stream{
    public:
    virtual int read();
    virtual int peek();
    virtual int write(char* holas);
    virtual int write(char holas);
    virtual int print(char* holas);
    virtual int available();
};
#endif