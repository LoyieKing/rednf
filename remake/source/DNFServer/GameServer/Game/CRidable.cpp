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

int CRidable::SetRidableObjectState(int idx, eRidableObjectState state)
{
    // ORIG 0x82feb5e：find != end 门（==end 返回 false）；状态 0/4/3/>4 均
    // 不改写直接返回 true，否则写状态返回 true。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.find(idx);
    if (it == m_objects.end())
        return 0;
    if (!(it->second.m_state == 0 || it->second.m_state == 4 ||
          it->second.m_state == 3 || it->second.m_state > 4))
    {
        it->second.m_state = (int)state;
    }
    return 1;
}

// ORIG 0x82fec2c：遍历对象表，依次 push (m_x, m_y)。
// ORIG 汇编为 pair<int,int>::pair<int&,int&> 模板构造（左值引用实例，非
// make_pair 值传递），故此处用局部左值 + 直接 pair 构造对齐符号集。
void CRidable::GetObjectPos(std::vector<std::pair<int, int> >& out)
{
    for (std::map<int, DungeonRidableObject>::const_iterator it = m_objects.begin();
         it != m_objects.end(); ++it)
    {
        int x = it->second.m_x;
        int y = it->second.m_y;
        out.push_back(std::pair<int, int>(x, y));
    }
}
// ORIG 0x82fe9c8：遍历 m_objects；坐标 (m_x,m_y)==(a,b) 且 state==4 或
// (state==3 && m_flag18) 的对象置 state=1、计数并发送生成包；循环后
// p->put_byte(idx, 计数) 收尾。party 形参 ORIG 未使用（保留签名）。
// 成员偏移按 CRidable.h：pair.second 起 +4 → m_x@+4/m_y@+8/m_field0c@+0xc/
// m_field10@+0x10/m_field14@+0x14/m_field18@+0x18/m_state@+0x1c。
void CRidable::CheckAppearRidableObject(CParty*, int a, int b, PacketGuard* p)
{
    int counter = 0;
    int idx = p->get_index();
    p->put_byte(0);
    for (std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
         it != m_objects.end(); ++it)
    {
        if (it->second.m_x == a && it->second.m_y == b)
        {
            if (it->second.m_state == 4 ||
                (it->second.m_state == 3 && m_flag18 != 0))
            {
                it->second.m_state = 1;
                ++counter;
                SendSpawnRidableObject(p, it->second.m_field10,
                                       it->second.m_field14,
                                       it->second.m_field0c,
                                       it->second.m_field18);
            }
        }
    }
    p->put_byte(idx, counter);
}
