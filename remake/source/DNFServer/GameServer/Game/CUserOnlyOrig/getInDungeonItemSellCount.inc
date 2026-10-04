//
// 44/45. CUser::GetCurCharacInventoryRef(ushort,ushort) 非 const T 0x08680f2e / const T 0x08680c8a
//
// 44/45. CUser::GetCurCharacInventoryRef(ushort,ushort) 非 const T 0x08680f2e / const T 0x08680c8a
// const 版报告在案（GetCurCharacInventoryRef.md）；非 const 版【报告缺失·objdump 同构】，
// 差异：getCurCharacInvenW/getCurCharacCargoW/CInventory::GetInvenRef(int,int)/CCargo::GetCargoRef(int)
// /CExpandEquipslot::GetInvenRefW(INVEN_TYPE,int)。共用私有展开见 priv_GetCurCharacInventoryRefImpl。
// Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short space, unsigned short index)
// {
//     return priv_GetCurCharacInventoryRefImpl(/*writeMode=*/true, space, index);
// }
// const Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short space, unsigned short index) const
// {
//     return priv_GetCurCharacInventoryRefImpl(false, space, index);
// }
// impl 草稿（switch 与 ORIG jump table 逐一对应）：
//   if (getCurCharacR()==NULL) return NULL;（非const用同一 getCurCharacR）
//   switch(space):
//     case 0:  return invRef(GetCurCharacInvenR/W(), 1, index);
//     case 1:  return invRef(..., 2, index);
//     case 2:  return cargoRef(GetCurCharacCargoR/W(), index);
//     case 3:  return invRef(..., 0, index);
//     case 7:  return invRef(..., 3, index);
//     case 0x12: expand = GetCharacExpandDataR(this,9);
//                return CExpandEquipslot::GetInvenRef[R|W](expand, INVEN_TYPE(5), index);
//     case 0x13..0x16: title = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe);
//                return CTitleBook::getTitleSlot(title, space-0x13, index);
//     default: return NULL;
//
// 46. CUser::GetDailyBadge(short*) W 0x0868e0c（EPs）【报告缺失】
// ORIG: for(i=0;i<=2;++i) out[i]=(short)*(ushort*)(this+(0x47040+i)*2+4)
// 数组基址 = 0x47040*2+4 = 0x8E084 ushort m_dailyBadge[3]
// 注意 nm 显示另存在 _ZN5CUser13GetDailyBadgeE15ENUM_BADGE_TYPE @0x08568e48——
// EPs 版按指针写，ENUM_BADGE_TYPE 版签名冲突需 ODR 裁决（meta 备注）。
// short CUser::GetDailyBadge(short* out)
// {
//     for (int i = 0; i <= 2; ++i) {
//         out[i] = (short)m_dailyBadges[i];
//     }
// }
//
// 47. CUser::GetETC(short) W 0x08697474【报告缺失】
// int CUser::GetETC(short key)
// {
//     std::map<short, int>& m = m_map8eaa4;
//     if (m.find(key) == m.end()) {
//         return 0;
//     }
//     int val = m[key];
//     m.erase(key);
//     return val;
// }
//
// 48. CUser::getHashCode(unsigned char*) T 0x084fa298
// void CUser::getHashCode(unsigned char* digestOut)
// {
//     md5_context ctx;                                             // 220 字节局部（-0xe0）
//     md5_starts(&ctx);
//     unsigned int sz = m_characList.size();
//     unsigned char* base = (unsigned char*)&m_characList.operator[](0);
//     md5_update(&ctx, base, sz * 0x14d1);
//     md5_finish(&ctx, digestOut);
// }
//
// [b28d-22 N82] ORIG 0x0868b106：p167=CLEAR，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868b106：P30/P311 成员数组视图 + 常量索引
// （`m_counts[i+0x23ac8]`）复现 ORIG `add $0x23ac8,%edx; mov 0x8(%eax,%edx,4)` 两步寻址
// （旧 reinterpret_cast<int(&)[7]> 产 add 基址形）；视图元素与 m_addInfo 同型化（P248 档③）。
// 残余 this/idx 装载序 + lea-vs-add（P30 结构性伪影；76=76 条，指令数已对齐）。
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
        if (m_sellCountData[i] == it->m_addInfo) found = true;
        i++;
    }
    if (!found)
    {
        // [H6B 2026-09-11 第二轮负结论·已回滚] 试把常量写在索引表达式左侧
        // （`m_counts[0x23ac8 + m_itemSellCount]`，欲令 4.4 走 ORIG 的 `add $0x23ac8,%ecx`）：
        // 实测 **76→76、ndiff 12→12 中性**（差集仍 ORIG-only add:1 / OURS-only lea:1）⇒ 回滚；
        // N107「写点 add↔lea」不可源码择形。
        m_sellCountData[m_itemSellCount] = it->m_addInfo;
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
}