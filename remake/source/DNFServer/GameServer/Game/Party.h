// ORIG exported symbols: nm ORIG 0x084869f4 (C1 W) / 0x08486a1e (D1/D2 W) /
// 0x084869d6 (Reset W)。报告缺失（组内报告均属 CParty/QuickParty 域），
// 2026-08-28 按 §3.2 nm/objdump 转写（DungeonPartyManager.cpp TU 数据类族）。
//
// 布局证据（objdump 实证，ORIG）：
//   Party::C1 0x84869f4 = call IData::C1 + call PlayInfo::C1 + call Party::Reset
//   Party::Reset 0x84869d6 = call PlayInfo::Reset + movl $0,0x3c(%eax)
//   Party::D1/D2 0x8486a1e = 5 字节 trivial（push ebp/mov/leave/ret）
//   pair<const int,Party> 移动 ctor 0x8488902 拷贝 0..0x3c ⇒ sizeof(Party)=0x40
//   CDungeonSuccessNode::UpdateCharacter 0x84851de it->second+0x3c 自增
// 基类：IData（空，EBO 偏移 0）、PlayInfo（偏移 0，m_f3c@+0x3c；
//   CUser.h PlayInfo 修正 2026-08-28 Main 代落：m_f3c 移入 PlayInfo，
//   m_f40/m_f44/m_schoolNo 移入 Character，CUser 布局不变）。
#ifndef DNF_GAME_PARTY_H_
#define DNF_GAME_PARTY_H_

#include "IData.h"
#include "CUser.h"   // PlayInfo 基类（m_f3c @+0x3c，Main 代落已落）

class Party : public IData, public PlayInfo
{
public:
    Party();     // W 0x084869f4（C1/C2 同址）
    ~Party();    // W 0x08486a1e（空体，ORIG 5 字节 trivial 形态）
    void Reset();  // W 0x084869d6（PlayInfo::Reset + 清 +0x3c）
};

#endif  // DNF_GAME_PARTY_H_
