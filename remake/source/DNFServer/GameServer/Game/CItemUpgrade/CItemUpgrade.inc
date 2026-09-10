CItemUpgrade::CItemUpgrade()
{
    // ORIG 0x085461c8 逐段转写：
    //   [表区] CItemUpgradeTable(this+0x04)（upgrade_table_t 薄包装，ctor 委托
    //          upgrade_table_t 0x89000ca）——本重建沿用类内裸偏移约定，无成员建模；
    //   [基类] IItemUpgrade(this) 0x080fc834 + vptr 写入（编译器生成；槽序
    //          ProcUpgrade→LoadUpgradeScript，ORIG vtable 0x08c9d310 实证）；
    //   [+0x4ec] = 100000（randInt 热身上界）；
    //   [热身] time(NULL) 种子 → operator new(0x9c8) → CMTRand(seed) 存 +0x4e8，
    //          循环 100 次 randInt(*+0x4ec) 预热 MT19937 状态（结果弃用）。
    // b28d-lane12: ORIG 0x085461c8 实证三修（ndiff 20→0，verdict AE）——
    //   ① 基类声明序改 CItemUpgradeTable 在前（CDataManager.h；0x085461d1 `add $0x4`
    //      +call 表区先于 0x085461e2 call IItemUpgrade 实证；primary base/布局不变）；
    //   ② 去具名 pRand 局部：0x08546242 循环每次重读 m_rand（+0x4e8，P183）；
    //   ③ range 先于 seed 声明（0x0854620c seed@-0x24 / 0x0854625f range@-0x20）
    //      + 实参内联赋值 `new CMTRand(seed = time(0))`：复现 0x0854620f
    //      `lea -0x24(%ebp),%esi` 在 operator new 前装载 &seed（四形实测唯一命中：
    //      普通形/两句式/语句表达式/逗号形均 ndiff=4）。
    m_randMax = 100000;
    unsigned long range;
    unsigned long seed;
    m_rand = new CMTRand(seed = time(0));
    for (int i = 0; i < 100; ++i)
    {
        range = static_cast<unsigned long>(m_randMax);
        m_rand->randInt(range);
    }
