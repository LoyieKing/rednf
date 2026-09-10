user_creature::CCreatureItem* user_creature::CCreatureItemFactory::AcquireCreatureItem(
    user_creature::CREATURE_ITME_TYPE type)
{
    user_creature::CCreatureItem* item = 0;
    if (type == 1)
        item = reinterpret_cast<user_creature::CCreatureItem*>(m_creaturePool.Acquire());
    else if (type == 0)
        item = reinterpret_cast<user_creature::CCreatureItem*>(m_eggPool.Acquire());
    // ORIG 0x833f5b6：非空 → 0x833f5bc mov (%eax),%eax; mov (%eax),%edx;
    // mov -0xc,%eax; mov %eax,(%esp); call *%edx —— 经 vtable[0] 对池内存
    // placement 构造 C1；成功/失败统一尾 mov -0xc,%eax 返回（0x833f613）。
    if (item != 0)
    {
        typedef void (*CtorCall)(void*);
        // ORIG 0x833f5bc：fnptr 双级解引用（call *%edx）+ arg 经 %eax 装
        // (%esp)。4.4 恒 fnptr→%eax/arg→%edx，角色互换属 N7（虚调用 callee
        // 寄存器互换）；register 绑 %edx 反产 spill 对拷，维持最简内联形。
        (reinterpret_cast<CtorCall>(*reinterpret_cast<CtorCall**>(item)))(item);
        return item;
    }
    // ORIG 0x833f5cd：ctor cMyTrace(__PRETTY_FUNCTION__@0x8c2eda0, 0x68, 5)；
    // 0x833f5f0：operator()("%d]"@0x8c2d761, PF, 0x68)。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x68, 5);
    trace("%d]", __PRETTY_FUNCTION__, 0x68);
    return item;
