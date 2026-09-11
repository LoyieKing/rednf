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
        // ORIG 0x833f5b6：`mov (%eax),%eax; mov (%eax),%edx; mov -0xc(%ebp),%eax;
        // mov %eax,(%esp); call *%edx` = vtable 槽 0 的**真实虚调用**
        // （CCreatureItem vtable 槽序 Reset/IsSave/LoadCreatureItem/SaveCreatureItem，
        // 见 CInventory.h 注释，槽 0 = Reset）⇒ 写 `item->Reset()`。
        // [MiscLane 2026-09-11 翻正] 旧形为 fnptr typedef 桥（发明局部类型 +
        // `(*(CtorCall**)item)(item)`），4.4 无论 4.4.7/4.4.6 均产
        // `call *%eax`（真 fnptr 用 %eax）；改真实虚调用后，4.4.6 后端（本 TU
        // 已路由 C6446R）产 ORIG 的 `mov (%eax),%edx; call *%edx`
        // （探针 /tmp/lane_MiscLane/probe_vc.cpp h1/h3 实证）。语义等价：
        // vtable[0] 即 Reset，且不再依赖手工 fnptr 双解引用。
        item->Reset();
    }
    else
    {
        // ORIG 0x833f5cd：ctor cMyTrace(__PRETTY_FUNCTION__@0x8c2eda0, 0x68, 5)；
        // 0x833f5f0：operator()(format@0x8c2d761, PF, 0x68)。
        // [语义修复 2026-09-11] 0x8c2d761 的 .rodata 实字节 = "[%s][%d]"
        //   （objdump -s 直证 5b 25 73 5d 5b 25 64 5d 00）；旧文 "%d]" 系地址错标
        //   造成的**日志文本契约 bug**（§6.5：AE 归一化不比较 .rodata 内容 ⇒
        //   verdict 不可见）⇒ 按 ORIG 直证改正。
        // [MiscLane 2026-09-11] 旧形为「早返 `return item;` + 函数尾再 `return item;`」
        //   ⇒ 4.4 在真路径多发一次 `mov -0xc(%ebp),%eax`（ORIG 0x833f5cb 直证为
        //   `jmp 833f613` 直达共享尾块、无重复装载）。改 if/else + 单一尾
        //   `return item;` 即复现 ORIG 的 je→失败块 / 真路径 jmp→共享尾 形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x68, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x68);
    }
    return item;
}
