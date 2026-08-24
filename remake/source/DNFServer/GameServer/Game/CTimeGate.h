#ifndef DNF_GAME_CTIMEGATE_H_
#define DNF_GAME_CTIMEGATE_H_

#include "CVillageObjectMgr.h"   // village_object::IObject（唯一权威声明）

// ============================================================================
// CTimeGate : village_object::IObject —— 时间之门（village 中的可视图标）
// ----------------------------------------------------------------------------
// ORIG 布局（继承 IObject，vptr @ +0x00）：
//   +0x00  vptr（vtable for CTimeGate 0x8c0beb8；slot0=include/reset、slot1=put_object_info）
//   +0x04  unsigned char m_openState （SetLevel 写入 +0x04；isOpen 读 +0x04 == 0x64；
//            put_object_info 以 movzbl 零扩展读取）
//   +0x08  int  m_villageIndex     （getVillageIndex 0x84ed480：mov +0x08）
//   +0x0c  int  m_areaIndex        （getAreaIndex    0x84ed48c：mov +0x0c）
//   +0x10  int  m_npcIndex         （getNpcIndex     0x84ed498：mov +0x10）
//   +0x14  int  m_questIndex       （getQuestIndex   0x8234e2e）
//   +0x18  int  m_openedQuestIndex （getOpenedQuestIndex 0x8234e3a）
//   sizeof == 0x1c（与 GlobalData 旧 m_pad[0x1c] 一致）
// 虚函数：
//   ctor 0x8284512：IObject 基类 ctor -> 置 CTimeGate vptr -> m_openState=0
//   dtor 0x8284536：仅重设 vptr
//   put_object_info 0x82844d8（vtable slot1，IObject 纯虚）：
//       put_byte(2); put_byte(m_openState); return 1
//   slot0（IObject 纯虚 reset；ORIG 该槽符号名为 include，返回 0）→ 空操作
// ============================================================================

class CTimeGate : public village_object::IObject
{
public:
    CTimeGate();                                   // ORIG 0x8284512
    ~CTimeGate();                                  // ORIG 0x8284536

    // village_object::IObject 抽象方法实现
    void reset();                                  // ORIG vtable slot0（空操作）
    bool put_object_info(PacketGuard& p);          // ORIG 0x82844d8

    void SetTimeGate(int a, int b, int c, int d, int e);   // ORIG 0x86d2788
    bool setBlindTimeGateStateQuest(int questIdx);         // ORIG 0x8234e46
    void SetLevel(int level) { m_openState = (unsigned char)level; } // ORIG 0x84ed4a4
    char isOpen() const;                                   // ORIG 0x8234e1c

    int getVillageIndex() const { return m_villageIndex; } // ORIG 0x84ed480
    int getAreaIndex() const { return m_areaIndex; }       // ORIG 0x84ed48c
    int getNpcIndex() const { return m_npcIndex; }         // ORIG 0x84ed498
    int getQuestIndex() const { return m_questIndex; }     // ORIG 0x8234e2e
    int getOpenedQuestIndex() const { return m_openedQuestIndex; } // ORIG 0x8234e3a

private:
    unsigned char m_openState;   // +0x04
    char m_pad1[3];              // +0x05..+0x07
    int m_villageIndex;        // +0x08
    int m_areaIndex;           // +0x0c
    int m_npcIndex;            // +0x10
    int m_questIndex;          // +0x14
    int m_openedQuestIndex;    // +0x18
};

#endif
