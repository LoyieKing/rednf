#ifndef DNFTICKHANDLER_H_
#define DNFTICKHANDLER_H_

#include <pthread.h>
#include <queue>

#include "PacketHeader.h"

class CApplication;
class CFrameCountHandler;

// from GuildThread.h
class CFrameCountHandler
{
public:
    CFrameCountHandler();
    ~CFrameCountHandler();
    void InitFrameCountInfo(CApplication* app, unsigned int frameCount, unsigned short tick);
    CFrameCountHandler* GetFrameCountInfo();
    void SaveProcess();
    void SaveProcess(int interval);
    int m_field0;     // +0
    int m_field4;     // +4
    int m_field8;     // +8
    int m_fieldc;     // +0xc
    int m_field10;    // +0x10
    int m_field14;    // +0x14
    int m_field18;    // +0x18
    int m_field1c;    // +0x1c
    int m_field20;    // +0x20
    int m_field24;    // +0x24
    int m_field28;    // +0x28
    int m_field2c;    // +0x2c
};

#endif
