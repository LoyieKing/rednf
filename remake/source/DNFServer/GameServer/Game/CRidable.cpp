// df_game_r Game/ CRidable 专属 TU（从 GameStubs.cpp 拆出）。
// 语义依据 ORIG CheckAppearRidableObject 0x82fe9c8 / SetRidableObjectState
// 0x82feb5e / SendSpawnRidableObject 0x82fece2 / Clear 0x82fe6bc。
#include "CRidable.h"
#include "PacketBuf.h"
#include "PacketGuard.h"

// ORIG ctor 0x82a4406：默认构造 map<int,DungeonRidableObject>@+0x00；
// dtor 0x82a441a：析构该 map。
// ORIG 0x82a4406：仅 map C1（m_flag18 未初始化，字节一致优先）。
CRidable::CRidable() : m_objects() {}
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

bool CRidable::SetRidableObjectState(int idx, eRidableObjectState state)
{
    // ORIG 0x082feb5e 逐条：find == end → return 0；条件 (m_state==0||==4||==3||>4)
    // 成立 → `test %al,%al; je 赋值段; mov $0,%eax` **return 0**（真语义 bug：旧代码
    // 注释写"直接返回 true"并把 return true 放在条件外，返回码反了，P249 同族第 9 例）；
    // 条件不成立 → `m_state = state; mov $1,%eax` return 1。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.find(idx);
    if (it != m_objects.end())
    {
        if (it->second.m_state == 0 || it->second.m_state == 4 ||
            it->second.m_state == 3 || it->second.m_state > 4)
            return false;
        it->second.m_state = (int)state;
        return true;
    }
    // ORIG 0x082fec25：it==end 的 return 0 落在函数末尾独立块（cond 成立的
    // return 0 在 0x082fec06 中间块）⇒ 用末尾 return 共享该块。
    return false;
}
// ORIG 0x082fec2c：遍历对象表，依次 push (m_x, m_y)。
// ORIG 汇编为 pair<int,int>::pair<int&,int&> 模板构造（左值引用实例，非
// make_pair 值传递），故此处用局部左值 + 直接 pair 构造对齐符号集。
void CRidable::GetObjectPos(std::vector<std::pair<int, int> >& out)
{
    // b28d-40: N82 封顶——帧 0x48 vs 0x38 + 迭代器/pair 槽序差，语义闭合。ORIG 0x082fec2c。
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
    // b28d-40: N82 封顶——对象槽 -0x24/-0x28 平移 + 帧差，语义闭合。ORIG 0x082fe9c8。
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
