// ==================================================================
// CBattle_Field::checkClearedNamedMonsterMap @ 0x08300c76
// 报告：docs/class_func_reports/CBattle_Field/checkClearedNamedMonsterMap.md
// [SweepBattleMonsterC3A] 符号线索闭环：ORIG 0x08300cee/0x08300d15 调 **const** 版
// `_ZNKSt6vectorI20STNamedMonsterMapPosSaIS0_EEixEj` @0x0830fec0（全库无**非** const
// 版定义；直接写 `[idx]` 会误选非 const 版 ⇒ 我方缺失该 ORIG 符号）⇒ 源码用 const
// 引用视图 cast（P323），
// p167 MISSING `…ixEj`×2 / EXTRA 非 const×2 已归零。Main 代落 `isNamedMonster()`
// throw()（WarField.h/MapInfo.cpp）后 `_Unwind_Resume` 消失、calls 9→7=ORIG，
// gap 81/75=+6：残差全为 4.1 代码生成风格（esi 路径标志 `mov $0/$1,%esi`+push/pop
// esi、GetMapInfoFromPos 实参求值槽 -0x10/-0xc）+ char m_visited 判零 4.4 sete（P73）
// ⇒ N 封顶（语义等价：p167 calls 7=7、差集仅 C1/C2 别名）。
// ==================================================================
// [P8S5 2026-09-11 负结论·4.4 bool 值域折叠] 唯一残差（tail，析构后）：
//   ORIG 0x8300d91 `test %esi,%esi; je 0x8300d9a`；我方 0x3e9 `cmp $0x1,%esi; jne`。
//   %esi 两处常量赋值（`mov $0x1,%esi` / `mov $0x0,%esi`）⇒ 4.4 知值域{0,1} 把 `!=0` 折成 `==1`；
//   4.1 不折。条件表达式形（`isNamedMonster()!=0` → `isNamedMonster()`）已同形（`test %al,%al; je`），
//   尾部折叠点在源码之外（编译器 fold）⇒ 不可造。本波未改（gap −1、blocks 1）。
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
    // 两次索引各自重新求值（4.1 无 CSE）。ORIG 0x08300cf5/0x08300d1d 把两个字段
    // 分别落槽 -0x10/-0xc（P0：有槽往返 ⇒ 原码有具名局部；声明序 x 先 = 槽更深）。
    typedef std::vector<STNamedMonsterMapPos> NamedMonsterMapPosVec;
    const int posX =
        static_cast<const NamedMonsterMapPosVec&>(m_pDungeon->m_vec8f0)[idx].m_x;
    const int posY =
        static_cast<const NamedMonsterMapPosVec&>(m_pDungeon->m_vec8f0)[idx].m_y;
    // ORIG +0x0c6 直接 `movzbl 0x1(%eax),%eax; test %al,%al; je 0x08300d95`（fail 岛，
    // 无 info 具名局部槽，P0）⇒ 负极性 `==0 return` 会让 4.4 出 `xor $1; test; je 主流程`
    // （多 1 条 + 布局反）；改正极性包裹（P214①）：visited!=0 才走主体。
    if (GetMapInfoFromPos(posX, posY, false)->m_visited != 0)  // MapInfo+0x01（无空检查）
    {
        map_monster mob;                          // 默认构造（ORIG 081511c8）
        mob.m_mobIndex = uid;                     // +0x0c ← 参数 2
        // ORIG +0x0f3 段：`test %al,%al; je; mov $1,%ebx; mov $0,%esi; jmp; mov $1,%esi`
        // （esi 路径标志跨析构，P56 if 形）。
        if (mob.isNamedMonster() != 0)             // W 08151182（析构后返回）
            return true;
    }
    return false;
}