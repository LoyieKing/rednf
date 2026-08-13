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
    void InitFrameCountInfo(CApplication* app, unsigned int frameCount, unsigned short tick);
    CFrameCountHandler* GetFrameCountInfo();
    void SaveProcess();
    void SaveProcess(int interval);
    int m_bInit;      // +0（首次调用标记 0/1）
    int m_frameCount; // +4（DFC，日志 FPS(%02d) / DFC(%02d)）
    int m_fpsInterval;// +8（100/frameCount）
    int m_startClock; // +0xc（times() 起始）
    int m_curClock;   // +0x10（times() 当前）
    int m_secondFrameCnt; // +0x14（每秒帧计数）
    int m_fps;        // +0x18（FPS 结果）
    int m_field1c;    // +0x1c
    int m_field20;    // +0x20
    int m_state;      // +0x24（状态 0-4；子字节 +1/+2 为秒/分计数）
    char m_logCnt;    // +0x28（日志节拍计数）
    int m_app;        // +0x2c（CApplication*）
};

#endif
