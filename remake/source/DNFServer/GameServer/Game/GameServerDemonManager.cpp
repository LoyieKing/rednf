// GameServerDemonManager.cpp — ORIG 全套方法实现（LinkClosureD 批次）
//
// 报告：docs/class_func_reports/GameServerDemonManager/（README/SetUserProhibitFlag/
// SetNoticeMessage/Notice10MinShutDownMessage/~GameServerDemonManager 全文已读）。
// 报告缺失符号（C1/GetNoticeMessage/IsProhibitConnectState/GameserverDemonManagerInstance）
// 以 nm -S 地址 + objdump -d 逐指令转写（见 §3.2 兜底规则）。
// 布局（ORIG C1 0x080f6330 / SetNoticeMessage 0x080f6280 实证，sizeof 0x101）：
//   +0x00 unsigned char m_prohibitFlag（SetUserProhibitFlag 写 byte；IsProhibitConnectState 读 byte）
//   +0x01 unsigned char m_noticeLen（SetNoticeMessage 写 byte；C1 清零）
//   +0x02 char m_notice[0xff]（C1 memset 0xff；SetNoticeMessage memcpy len）
// 单例：GameserverDemonManagerInstance() 函数局部 static（guard 0x093faba0 /
// instance 0x093fabc0，__cxa_atexit(~D1 0x080f621c)——P20 形态）。

#include "GameServerDemonManager.h"
#include "TimerQueue.h"          // G_TimerQueue / TimerEntry::OBJ_TYPE / TIMER_MESSAGE

GameServerDemonManager::GameServerDemonManager()      // W 0x080f6330
{
    m_prohibitFlag = 0;
    m_noticeLen = 0;
    memset(m_notice, 0, 0xff);
}

GameServerDemonManager::~GameServerDemonManager()     // T 0x080f621c（空体）
{
}

void GameServerDemonManager::SetUserProhibitFlag(bool flag)   // T 0x080f6222
{
    m_prohibitFlag = static_cast<unsigned char>(flag);
}

unsigned char GameServerDemonManager::IsProhibitConnectState()  // W 0x084ed404
{
    return m_prohibitFlag;
}

void GameServerDemonManager::SetNoticeMessage(unsigned char len, char* msg)  // T 0x080f6280
{
    m_noticeLen = len;
    memset(m_notice, 0, 0xff);
    memcpy(m_notice, msg, static_cast<unsigned int>(len));
}

char const* GameServerDemonManager::GetNoticeMessage()        // W 0x0863bff6
{
    return m_notice;
}

void GameServerDemonManager::Notice10MinShutDownMessage()     // T 0x080f623a
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0xa2, 0x3c, 0, 0);
}

GameServerDemonManager* GameserverDemonManagerInstance()      // T 0x080f61b8
{
    static GameServerDemonManager instance;
    return &instance;
}
