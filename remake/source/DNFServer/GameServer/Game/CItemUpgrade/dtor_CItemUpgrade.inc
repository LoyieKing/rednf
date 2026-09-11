CItemUpgrade::~CItemUpgrade()
{
    // ORIG 0x85462c6：析构 +0x4e8 的 CMTRand*（构造时 new，见 0x8546247）。
    // [b28d-solo-CItemUpgrade] ORIG 0x085462f5：`movl $0x0,0x4e8(%eax)` 在 if 之外
    // 无条件执行 ⇒ 清零必须写在 if 外（写成 if 内 m_rand=0 少一条且分支布局偏移）。
    // 基类析构变体展开序（IItemUpgradeD1 this+0 → TableD1 this+4）由编译器生成，
    // 与 ORIG 同名调用一致。
    if (m_rand)
    {
        ::operator delete(m_rand);
    }
    m_rand = 0;
}