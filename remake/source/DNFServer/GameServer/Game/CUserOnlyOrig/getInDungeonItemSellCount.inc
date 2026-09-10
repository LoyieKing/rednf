int CUser::getInDungeonItemSellCount(int space, int idx)
{
    // ORIG 0x0868b106（报告 getInDungeonItemSellCount.md 全文转写）。
    if ((signed)m_itemSellCount >= 7)
    {
        return 0;
    }
    if (getCurCharacR() == NULL)
    {
        return 0;
    }
    Inven_Item* it = GetCurCharacInventoryRef((unsigned short)space, (unsigned short)idx);
    if (it == NULL)
    {
        return 0;
    }
    bool found = false;                                          // -0xd
    int i = 0;                                                   // -0xc
    // [b28d-solo-CUserOnlyOrig P30/P311] ORIG 读点 `mov idx,%edx; mov this,%eax;
    // add $0x23ac8,%edx; mov 0x8(%eax,%edx,4),%edx` = 下标含常量 `[i+K]` 两步形
    // （K=0x8eb28/4）；成员数组视图 + 常量索引复现。
    // [b28d-solo-CUserOnlyOrig P211档③ 负例] 视图元素 int 化实测反劣（读点 +1 条 `mov %eax,%edx`
    // 异型中转，装载序未翻）⇒ 维持 unsigned（与 m_addInfo 同型）。
    struct SellCountView { int m_head[2]; unsigned int m_counts[0x23acf]; };
    // [b28d-22 修法 P25] ORIG 0x0868b1a3-0x0868b1c2 条件序列由
    // `while (!(found || 6 < i))` 逐条复现（探针 /tmp/p22g2_f.cpp a/c）；
    // 删 slots 局部（ORIG 每点重算 m_sellCountData，P183）。
    while (!(found || 6 < i))
    {
        // [b28d-solo-CUserOnlyOrig P288/P291] ORIG 0x0868b1xx 写点
        // `mov 0x8eb24(%eax),%ecx; add $0x23ac8,%ecx; mov %edx,0x8(%eax,%ecx,4)`
        // = scaled 索引；`((int*)char数组)[i]` 在 4.4 恒折 shl+add
        // （旧形 OUR: add $0x8eb28; shl $2; add; mov (%edx)）⇒ 用数组引用视图
        // `reinterpret_cast<int(&)[7]>` 出 ORIG 单条 scaled store/load。
        if (((SellCountView*)this)->m_counts[i + 0x23ac8] == it->m_addInfo) found = true;
        i++;
    }
    if (!found)
    {
        ((SellCountView*)this)->m_counts[m_itemSellCount + 0x23ac8] = it->m_addInfo;
        m_itemSellCount = m_itemSellCount + 1;
    }
    // [G6Sub3 2026-09-10 复核判 N 封顶] 76/76、帧 0x28 全等；残差仅 3 处装载序/寻址形：
    // ①读点 ORIG `mov -0xc(%ebp),%edx; mov 0x8(%ebp),%eax; add $0x23ac8,%edx;
    //   mov 0x8(%eax,%edx,4),%edx`（**index 先、base 后**）vs 4.4 base/index 互换 ⇒
    //   视图 cast 恒 base 先（P235）；正解需权威头给匿名 union 数组视角，本泳道禁改 .h；
    // ②写点 ORIG `add $0x23ac8,%ecx` vs 4.4 `lea 0x23ac8(%edx),%ecx`（N107 反向族）；
    // ③写点 4.4 多一条 `mov 0x8(%ebp),%edx`（eax 保活 base、edx 中转读 m_itemSellCount）⇒ N7。
    // 另：P211 档③ 视图元素 int 化已实测反劣（见上方注释）⇒ 不再试。
    return m_itemSellCount;
