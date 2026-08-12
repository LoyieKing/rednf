#ifndef DBMW_DNFTICKHANDLER_H_
#define DBMW_DNFTICKHANDLER_H_

#include "DBMWCommon.h"


class CFrameCountHandler
{
public:
    CFrameCountHandler();
    void InitFrameCountInfo(CApplication* app, unsigned int a, unsigned short b);
    void SaveProcess();
    void SaveProcess(int n);
    void* GetFrameCountInfo();
    char m_field0[4];   // +0
    unsigned int m_field4;   // +4
    unsigned int m_field8;   // +8
    unsigned int m_fieldC;   // +0xc
    int m_field10;      // +0x10
    unsigned int m_field14;  // +0x14
    unsigned int m_field18;  // +0x18
    char m_pad1C[4];    // +0x1c
    unsigned int m_field20;  // +0x20
    unsigned char m_field24; // +0x24
    unsigned char m_field25; // +0x25
    unsigned char m_field26; // +0x26
    char m_pad27;       // +0x27
    unsigned char m_field28; // +0x28
    char m_pad29[3];    // +0x29
    CApplication* m_app;  // +0x2c
};

#endif  // DBMW_DNFTICKHANDLER_H_
