// df_game_r Game/ CRidable 专属 TU（从 GameStubs.cpp 拆出）。
// 语义依据 ORIG CheckAppearRidableObject 0x82fe9c8 / SetRidableObjectState
// 0x82feb5e / SendSpawnRidableObject 0x82fece2 / Clear 0x82fe6bc。
#include "CRidable.h"
#include "PacketBuf.h"
#include "PacketGuard.h"

// ORIG ctor 0x82a4406：默认构造 map<int,DungeonRidableObject>@+0x00；
// dtor 0x82a441a：析构该 map。
CRidable::CRidable() : m_objects(), m_flag18(0) {}
CRidable::~CRidable() {}

// ORIG Clear 0x82fe6bc：直接调 m_objects.clear()。
void CRidable::Clear()
{
    m_objects.clear();
}

void CRidable::SendSpawnRidableObject(PacketGuard* p, int a, int b, int c, int d)
{
    // ORIG 0x82fece2：连续写 4 个 int。
    p->put_int(a);
    p->put_int(b);
    p->put_int(c);
    p->put_int(d);
}

void CRidable::SetRidableObjectState(int idx, eRidableObjectState state)
{
    // ORIG 0x82feb5e：未找到返回 false；当前状态 ∈ {0,3,4,>4} 视为不可改，
    // 否则写入新状态并返回 true。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.find(idx);
    if (it == m_objects.end())
        return;
    int cur = it->second.m_state;
    bool blocked = (cur == 0 || cur == 4 || cur == 3 || cur > 4);
    if (!blocked)
        it->second.m_state = (int)state;
}

void CRidable::CheckAppearRidableObject(CParty* /*party*/, int x, int y, PacketGuard* p)
{
    // ORIG 0x82fe9c8：遍历对象表，坐标命中且状态允许（4，或 3 且 +0x18 标志非零）
    // 时置状态 1、计数并发送出生信息。
    int countIdx = p->get_index();
    int count = 0;
    p->put_byte(0);
    for (std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
         it != m_objects.end(); ++it)
    {
        DungeonRidableObject& obj = it->second;
        bool match = (obj.m_x == x && obj.m_y == y);
        if (!match)
            continue;
        bool appear = false;
        if (obj.m_state == 4)
            appear = true;
        else if (obj.m_state == 3 && m_flag18 != 0)
            appear = true;
        if (!appear)
            continue;
        obj.m_state = 1;
        count += 1;
        SendSpawnRidableObject(p, obj.m_field10, obj.m_field14,
                               obj.m_field0c, obj.m_field18);
    }
    p->put_byte(countIdx, count);
}
