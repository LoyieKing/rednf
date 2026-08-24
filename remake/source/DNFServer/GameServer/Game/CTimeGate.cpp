// ============================================================================
// CTimeGate（G2 时间之门批次）
// ----------------------------------------------------------------------------
// ORIG（均为弱符号）：
//   ctor                            _ZN9CTimeGateC1Ev   0x8284512
//     → IObject::IObject() 基类 -> 置 CTimeGate vptr(0x8c0beb8) -> movb $0,+0x04
//   dtor                            _ZN9CTimeGateD1Ev   0x8284536
//     → 仅重设 vptr(0x8c0beb8)；基类 IObject 析构为空（不调用）
//   put_object_info                 _ZN9CTimeGate15put_object_infoER11PacketGuard 0x82844d8
//     → InterfacePacketBuf::put_byte(2);
//       put_byte((int)(char)+0x04 /*m_openState*/); return 1
//   reset（vtable slot0；ORIG 该槽符号名 include）      0x82845a0 → return 0（空操作）
//   isOpen                          _ZNK9CTimeGate6isOpenEv   0x8234e1c
//     → movzbl +0x04; cmp $0x64; 即 return (m_openState == 0x64)
//   setBlindTimeGateStateQuest      _ZN9CTimeGate26setBlindTimeGateStateQuestEi 0x8234e46
//     → (getQuestIndex()==questIdx && isOpen())
//          || (getOpenedQuestIndex()==questIdx && !isOpen())
//   SetTimeGate                     _ZN9CTimeGate11SetTimeGateEiiiii 0x86d2788
//     → +0x08=a(village) +0x0c=b(area) +0x10=c(npc) +0x14=d(quest) +0x18=e(opened)
// 布局见 CTimeGate.h（继承 village_object::IObject，vptr@+0，m_openState@+4）。
// ============================================================================

#include "CTimeGate.h"

#include "PacketGuard.h"   // put_byte（InterfacePacketBuf）

// ORIG 0x8284512：IObject 基类 ctor -> 置 CTimeGate vptr -> m_openState=0。
// C++ 中基类构造、vptr 覆写由编译器自动完成，函数体只需置 m_openState=0。
CTimeGate::CTimeGate()
{
    m_openState = 0;
}

// ORIG 0x8284536：仅重设 vptr（基类 IObject 析构为空，不调用）。
CTimeGate::~CTimeGate()
{
}

// ORIG vtable slot0（IObject 纯虚 reset 的 CTimeGate 覆写；ORIG 符号名 include，
// 函数体 return 0）。void reset() 语义即空操作。
void CTimeGate::reset()
{
}

// ORIG 0x82844d8：put_byte(2); put_byte(m_openState 零扩展); return 1。
bool CTimeGate::put_object_info(PacketGuard& p)
{
    p.put_byte(2);
    p.put_byte((int)m_openState);
    return true;
}

void CTimeGate::SetTimeGate(int a, int b, int c, int d, int e)
{
    m_villageIndex = a;
    m_areaIndex = b;
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
    return m_openState == 0x64;   // ORIG: movzbl +0x04; cmp $0x64; sete
}
