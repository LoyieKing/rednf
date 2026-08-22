// ============================================================================
// CTimeGate（G2 时间之门批次）
// ----------------------------------------------------------------------------
// ORIG（均为弱符号）：
//   isOpen                        _ZNK9CTimeGate6isOpenEv   0x8234e1c
//     → movzbl +0x04; cmp $0x64; 即 return (m_openState == 0x64)
//   setBlindTimeGateStateQuest    _ZN9CTimeGate26setBlindTimeGateStateQuestEi 0x8234e46
//     → (getQuestIndex()==questIdx && isOpen())
//          || (getOpenedQuestIndex()==questIdx && !isOpen())
//   SetTimeGate                   _ZN9CTimeGate11SetTimeGateEiiiii 0x86d2788
//     → +0x08=a(village) +0x0c=b(area) +0x10=c(npc) +0x14=d(quest) +0x18=e(opened)
// 布局见 CTimeGate.h（GlobalData.cpp 提供该属于 ctor）。
// 注：Area.cpp 经 extern "C" asm 标签 _ZN9CTimeGate11SetTimeGateEiiiii 调用 SetTimeGate。
// ============================================================================

#include "CTimeGate.h"

void CTimeGate::SetTimeGate(int a, int b, int c, int d, int e)
{
    m_field8 = a;
    m_fieldc = b;
    m_npcIndex = c;
    m_questIndex = d;
    m_openedQuestIndex = e;
}

bool CTimeGate::setBlindTimeGateStateQuest(int questIdx)
{
    if (m_questIndex == questIdx && isOpen())
    {
        return true;
    }
    if (m_openedQuestIndex == questIdx && !isOpen())
    {
        return true;
    }
    return false;
}

char CTimeGate::isOpen() const
{
    return (m_openState == 0x64) ? 1 : 0;
}