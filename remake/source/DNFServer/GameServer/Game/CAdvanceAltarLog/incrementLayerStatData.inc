void CAdvanceAltarLog::incrementLayerStatData(unsigned short itemType,
                                              unsigned int itemIndex,
                                              unsigned short a3,
                                              unsigned short useCountDelta,
                                              unsigned short useStarDelta)
{
    Guard<Mutex> guard(&m_mutex);

    const long long key = makeKey(itemType, itemIndex, a3);
    std::map<long long, STAdvanceAltarLayerStatData>::iterator it =
        m_layers.find(key);
    // ORIG 0x08156e7d/0x08156e94 每语句**只发 1 次** `_Rb_tree_iterator::operator->`
    // （`movzwl 0x12(%eax),%edx; add -0x44(%ebp),%dx; mov %dx,0x12(%eax)`，写回同一
    // %eax）；展开式 `it->second.f = (ushort)(it->second.f + d)` 使 `it->second` 出现
    // 两次 ⇒ 4.4 每语句 2 次 op->（共 4 call）⇒ 必须用复合赋值（本波 -8 条）。
    if (it != m_layers.end())
    {
        it->second.m_useCount += static_cast<unsigned short>(useCountDelta);
        it->second.m_useStar += static_cast<unsigned short>(useStarDelta);
        return;
    }

    STAdvanceAltarLayerStatData data;                   // 栈装配（ORIG local_30 区）
    // ORIG 0x08156d60：装配只经 STAdvanceAltarLayerStatDataC1 清零
    // （movw@0/movl@4/movw@8/movw@a/movw@c，+0x2 空洞与 +0xe pad 不写），
    // **无 memset 调用**（0x08156d60..0x08156e51 区间仅 C1Ev + 逐字段装配）。
    // [b28d-E03 2026-09-10] 字段真值修正：ORIG 0x08156e1d 把 a3 写 second+0x8
    //   （= m_itemLevel，pair<long long,ST> second@+8）；原写 m_field2(+0x2) 是
    //   真语义 bug（+0x2 洞 ORIG 全程不写，实证 0x8156e0f..0x8156e39 段）。
    data.m_itemType = itemType;
    data.m_itemIndex = itemIndex;
    data.m_itemLevel = a3;
    data.m_useCount = static_cast<unsigned short>(data.m_useCount + useCountDelta);
    data.m_useStar = static_cast<unsigned short>(data.m_useStar + useStarDelta);
    // ORIG 0x08156e52 段：operator[] 结果 %eax 直比 &data（无具名槽），
    //   `cmp %edx,%eax; je <出口>`（call 结果直用、无 store/load）。
    // [b28d-E03 2026-09-10 定案 N18a/N7 封顶 112 vs 111] 试形与读数：
    //   ①具名无 register（store/load 落 -0xc 槽）=113；②P15 赋值即条件=115；
    //   ③纯 register（非 asm 绑定，P0 合规）值落 %ebx=**112（最优）**；
    //   ④register + 反转比较方向（&data != slot）=113（已回退）。
    //   另以 c5root 4.1.2 实证（/tmp/probe_mc2.cpp、/tmp/probe_caa41*.cpp）：4.1.2 对
    //   具名/const/register 三种指针形**一律 spill**（cmp 前必有 `mov -0xN(%ebp),%eax`），
    //   无法造出 ORIG「call 结果跨 lea/cmp/je 保持 %eax」的无槽形 ⇒ 4.4 侧恒多 1 条
    //   `mov %eax,%ebx`（callee-saved 保存），跨版本寄存器分配差封顶。
    register STAdvanceAltarLayerStatData* slot = &m_layers[key];
    if (slot != &data)
        std::memcpy(slot, &data, 0xe);
