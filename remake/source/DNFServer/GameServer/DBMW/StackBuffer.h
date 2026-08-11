#ifndef DBMW_STACKBUFFER_H_
#define DBMW_STACKBUFFER_H_

#include "DBMWCommon.h"


class StackBuffer
{
public:
    StackBuffer();
    StackBuffer(const StackBuffer& other);
    ~StackBuffer();
    StackBuffer& operator=(const StackBuffer& other);
    static StackBuffer alloc(unsigned int size);
    void freeAll();
    char* getBuffer();
    char* m_buf;   // +0
    int m_end;     // +4
};

class StackBuffer_char : public StackBuffer
{
public:
    static StackBuffer_char alloc(unsigned int size);
    operator char*();
};

class StackBuffer_wchar : public StackBuffer
{
public:
    static StackBuffer_wchar alloc(unsigned int size);
    operator wchar_t*();
};

#endif  // DBMW_STACKBUFFER_H_
