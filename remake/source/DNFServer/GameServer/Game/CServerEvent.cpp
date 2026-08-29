// df_game_r 还原 —— CServerEvent（G2-4 批次，服务端事件状态）。
// 全部方法按 ORIG 逐指令转写（GetExpRate 0x0814a6e0 / AddExpRate 0x084e9248 /
// SubExpRate 0x0863bc70 / IsEventing 0x084e927f / TurnOnEvent 0x084e925f /
// TurnOffEvent 0x0863bc87 / GetEventInterval 0x084e9294）。
// ORIG 为静态成员 + 3 个静态数据（B 符号），见 CServerEvent.h。

#include "CServerEvent.h"

int CServerEvent::m_nExpRate = 0;              // 0x943c138
int CServerEvent::m_nCurEvent[0x100] = {0};    // 0x943c13c
int CServerEvent::m_nCurEventInterval[0x100] = {0};  // 0x943c144

int CServerEvent::GetExpRate()
{
    return m_nExpRate;
}

int CServerEvent::AddExpRate(int rate)
{
    m_nExpRate += rate;
    return m_nExpRate;
}

int CServerEvent::SubExpRate(int rate)
{
    m_nExpRate -= rate;
    return m_nExpRate;
}

bool CServerEvent::IsEventing(int idx)
{
    return m_nCurEvent[idx] == 1;
}

void CServerEvent::TurnOnEvent(int idx, int durationSecond)
{
    m_nCurEvent[idx] = 1;
    m_nCurEventInterval[idx] = durationSecond;
}

void CServerEvent::TurnOffEvent(int idx)
{
    m_nCurEvent[idx] = 0;
    m_nCurEventInterval[idx] = 0;
}

int CServerEvent::GetEventInterval(int idx)
{
    return m_nCurEventInterval[idx];
}
