// ---------------------------------------------------------------------------
// 29) CUser::check_insert_lottery_reward(CItem const*, vector<pair<Inven_Item,bool>>& rewards,
//                                         vector<pair<Inven_Item,bool>>& leftOver)
//     报告：docs/class_func_reports/CUser/check_insert_lottery_reward.md。ORIG 0x08676296。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08676296：p167=M0/E6:_ZN9__gnu_cxx17__normal_iteratorIPKSt4pairI10Inven_ItembESt6vectorIS3_SaIS3_EEEC1IPS3_EERKNS0_IT_NS_11__enable_ifIXsrSt10__are_sameISC_SB_E7__valueES8_E6__typeEEE;_ZNK9__gnu_cxx17__normal_iteratorIPKSt4pairI10Inven_ItembESt6vectorIS3_SaIS3_EEEptEv;_ZN9__gnu_cxx17__normal_iteratorIPKSt4pairI10Inven_ItembESt6vectorIS3_SaIS3_EEEppEv，gap=-50；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08676296：N86/P28 封顶——p167 MISSING
// `normal_iterator<...>::operator++(int)`×2（ppEi），EXTRA `operator->`×2（ptEv）；
// 4.1 后置 ++ 独立 sret 临时槽不可复现。
int CUser::check_insert_lottery_reward(const CItem* opener,
                                       std::vector<std::pair<Inven_Item, bool> >& rewards,
                                       std::vector<std::pair<Inven_Item, bool> >& leftover)
{
    // 第一轮：逐项判背包空位（item_type=(byte@+0x01)，空位数需 ≥1）
    // [b28d-solo-CUserOnlyOrig P167/P96/P28] ORIG 两循环均命中非 const
    // `__normal_iterator<...>::operator++(int)`（后置）与 `operator!=<P*,P*,vector>`；
    // 旧写法 const_iterator + 前置 ++ 产 const 转换构造/const 版 operator!= 差集。
    // [DualCUOO 2026-09-09] ORIG 第一轮 0x086762a0 `lea -0x30` 直接作 begin 的 sret
    // （声明+初始化）；第二轮 0x086763a9 `lea -0xbc` 临时 + 0x086763c1 拷回 -0x30
    // ⇒ 两轮复用同一 it（第二轮是赋值语句），故 it 提到两轮之外声明。
    std::vector<std::pair<Inven_Item, bool> >::iterator it = rewards.begin();
    for (; it != rewards.end(); it++)
    {
        // [b28d-solo-CUserOnlyOrig P167 新判据] 旧判：ORIG 命中 `operator*`(deEv) 而非
        // `operator->`(ptEv)（p167 EXTRA ptEv ×2）⇒ 用 `(*it).first` 形。
        // [b28d-PoolCUOO2 复测 2026-09-09] 并列试 `it->first` 形：p167 变为
        // MISSING ppEi×2 + EXTRA ptEv×2、ndiff 336→293——但
        // `normal_iterator<PSt4pairI10Inven_ItembE...>::ptEv` 在 ORIG nm=0（deEv/ppEi=1），
        // 发射即发明符号（missing_in_game 265→266）⇒ 违反硬口径1，回退 deEv 形。
        // 旧判词方向无误：本 TU 只允许对 ORIG 已有符号多发调用。MISSING ppEi×2 系
        // N113（4.4 对结果未用后置 ++ 恒折 ppEv，ppEi 不可复现）维持封顶。
        // [DualCUOO 2026-09-09 双判据 gap] ORIG 0x086762d2 `call deEv` 后紧跟
        // 0x086762d8..0x08676307 的 mov 链，终以 `movzwl 0x3c(%eax)`/`mov %ax,-0x32(%ebp)`
        // 收尾 ⇒ 拷贝长 0x3e=62B = 整个 pair<Inven_Item,bool>（Inven_Item 本体 61B）；
        // 随后 0x0867630b `movzbl -0x6d(%ebp),%eax` 从**栈拷贝**取 first[1] 字节。
        std::pair<Inven_Item, bool> item = *it;
        // [DualCUOO] ORIG 0x08676326/0x0867632a 为 `movzbl -0x6d(%ebp),%eax; movzbl %al,%ebx`
        // 无具名 uchar 的栈中转 ⇒ 源码把 m_field1 直接内联进实参位（P329 反向）。
        if (getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)item.first.m_itemType, 1) != 1)
            return 4;
    }
    // 第二轮：逐项插入
    // [DualCUOO 2026-09-09 双判据 gap] ORIG 三处 `call ppEi`（0x08676400 回收分支、
    // 0x086765d2 默认回收分支、0x08676654 循环体末尾）⇒ 第二轮是「空增量 for + 三处
    // 显式 it++」，非标准 for 增量（那样只有一份 ppEi，本处缺 7 条）。
    for (it = rewards.begin(); it != rewards.end(); )
    {
        std::pair<Inven_Item, bool> it2 = *it;               // 整体拷贝（pair 视图，62B）
        // [DualCUOO] ORIG 0x0867645c `movl $0xffffffff,-0x14(%ebp)`：r 在循环体开头
        // 声明并初始化，两个分支内改为赋值（0x08676652 / 0x0867671f 均 `mov %eax,-0x14`）。
        int r = -1;
        // [P328⑤/P171 死局部照抄] ORIG 0x0867646a `movl $0x0,-0x10(%ebp)`（Ghidra 侧
        // local_14 全函数无读取点）—— 补该具名死局部以对齐 r=-0x14 / script=-0xc 及其后
        // it/it2 全部槽位（旧稿缺此槽：it=-0x2c vs ORIG -0x30、it2=-0xa8 vs ORIG -0xac）。
        // [A06 2026-09-10 A/B 实测] +1 条（348→349）换 ndiff 407→136 **严格改善** ⇒ 保留。
        int unusedZero = 0;
        (void)unusedZero;
        // [DualCUOO 2026-09-09] ORIG 0x0867646a `mov -0xaa(%ebp),%eax; cmp $0x1,%eax`
        // （对象起始 -0xac ⇒ +0x02 的 dword）⇒ 源码是 m_addInfo == 1；旧「+4 u16 视图」
        // 读的是 m_addInfo 高 16 位，与 ORIG 语义不同，按 ORIG 修正。
        if (it2.first.m_addInfo == 1)
        {
            leftover.push_back(it2);                          // 直接回收
            it++;
            continue;
        }
        CItem* script = G_CDataManager()->find_item(it2.first.m_addInfo);
        if (script == 0)
            return 0x11;
        // [b28d-solo-CUserOnlyOrig P282 真 bug] ORIG 0x086764de `add $0x10,%eax; call *%edx`
        // ⇒ vptr+0x10 = slot4 = CStackableItem::IsAvatarItem（_ZTV14CStackableItem @0x08c96ea0
        // word[6]=0x08514a92=_ZNK14CStackableItem12IsAvatarItemEv，nm 反查实证）；
        // 旧源码误写 IsClearAvatar（真槽 vptr+0x18）⇒ 运行时调错虚函数。
        if (script != 0 && script->IsAvatarItem() == true)
        {
            if (getCurCharacInvenW()->GetRemainCapacity((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)0) <= 0)
                return 4;
            goto INSERT_OK;
        }
        if (script->IsCreatureItem() /*virt+0x14*/ &&
            script->GetItemType() /*virt+0x0c*/ == 0x16)
        {
            // [s3 2026-09-11 翻正 AE 348/347·ndiff 8→0] 三处形修正（逐形实测）：
            //   ① 守卫改显式 if/else（`m_field1 == 5` 作 if 侧）：ORIG 0x08676586
            //      `jne 0x08676669` 落点为**内层 else 块**（`mov $0x11; jmp ret` 位于
            //      if 体末尾之后、外层 else 之前）⇒ 早退单条 `if (!=5) return` 系错误形。8→2。
            //   ② else 分支补 ORIG-only 死存储 `r = -1;`（0x0867673 `movl $-1,-0x14(%ebp)`）：
            //      旧 CoBatchC 在**旧结构**下实测反劣故回滚；改 ① 后复测 2→1，保留。（+2 条）
            //   ③ 删 if 体末尾 `goto INSERT_OK;`：改由 if/else 自然落点 ⇒ 消 INSERT_OK 前
            //      4.4 padding 跳板（`nop; jmp .+2; nop`）⇒ 1→0，AE。
            if (it2.first.m_itemType == 5)
            {
                if (!user_creature::CCreatureItemConverter::SetEggItem(&it2.first))
                    return 0x11;
                r = getCurCharacInvenW()->tryInsertItemIntoInventory(it2.first);
                if (r == -1) return 4;
            }
            else
            {
                return 0x11;
            }
        }
        else
        {
            // [CoBatchC 2026-09-11 负结论·已回滚] 补 ORIG-only 死存储
            // （`movl $0xffffffff,-0x14(%ebp)` @0x0867673）实测 **349→351 反劣**
            // （4.4 额外发 store + 未消掉我方多出的 2 条）⇒ 回滚；本件 gap -2 属 N82 族。
            // [s3 ②] ORIG-only 死存储（结构改 if/else 后复测：ndiff 2→1，保留）
            r = -1;
            r = getCurCharacInvenW()->tryInsertItemIntoInventory(it2.first);
            if (r == -1)
            {
                ((CStackableItem*)opener)->get_multibox_default_item(&leftover);
                it++;
                continue;
            }
            // [s3 试形·已排除] 花括号形（`{ return 4; }`）零收益；`!script` 与 `==0` 同效。
            if (!script) return 4;
        }
INSERT_OK:
        leftover.push_back(it2);
        it++;
    }
    return 0;
}