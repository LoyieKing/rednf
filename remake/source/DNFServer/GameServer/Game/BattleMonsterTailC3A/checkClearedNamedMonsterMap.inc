bool CBattle_Field::checkClearedNamedMonsterMap(unsigned char idx, int uid)
{
    if (m_pDungeon == NULL)                       // +0x188
        return false;

    // CDungeon+0x8f0：m_vec8f0（vector<STNamedMonsterMapPos>{int m_x; int m_y;}）。
    // ORIG 对 size()/operator[] 按操作数独立求值（x/y 各一次索引）。
    if (static_cast<unsigned int>(idx) >= m_pDungeon->m_vec8f0.size())
        return false;

    // ORIG 0x08300cee/0x08300d15 两次调 const 版 operator[]
    // （_ZNKSt6vectorI20STNamedMonsterMapPosSaIS0_EEixEj @0x0830fec0；非 const 版
    // 全库无定义）⇒ 接收者须为 const 左值：显式 const 引用视图 cast（P323），
    // 不引入具名局部槽。
    typedef std::vector<STNamedMonsterMapPos> NamedMonsterMapPosVec;
    // ORIG +0x0c6 直接 `movzbl 0x1(%eax),%eax; test; je`（无 info 具名局部槽，P0）。
    if (GetMapInfoFromPos(
            static_cast<const NamedMonsterMapPosVec&>(m_pDungeon->m_vec8f0)[idx].m_x,
            static_cast<const NamedMonsterMapPosVec&>(m_pDungeon->m_vec8f0)[idx].m_y,
            false)->m_visited == 0)              // MapInfo+0x01（无空检查，忠实转写）
        return false;

    map_monster mob;                              // 默认构造（ORIG 081511c8）
    mob.m_mobIndex = uid;                         // +0x0c ← 参数 2
    // ORIG +0x0f3 段：`test %al,%al; je; mov $1,%ebx; mov $0,%esi; jmp; mov $1,%esi`
    // （esi 路径标志跨析构，P56 if 形）。
    if (mob.isNamedMonster())                      // W 08151182（析构后返回）
        return true;
    return false;
