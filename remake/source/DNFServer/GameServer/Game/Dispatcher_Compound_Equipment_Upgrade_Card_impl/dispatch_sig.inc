// ============================================================================
// 装备升级卡合成（ORIG 0x08227e00 T，约 710 指令）
//
// 包协议（回包 header 0x169）：
//   材料 = 升级卡(slotCard) + 目标装备(slotEquip)（合成配方），放入槽 slotTarget。
//   流程见 docs/class_func_reports/Dispatcher_Compound_Equipment_Upgrade_Card。
//
// [b28d-solo-Dispatcher_Compound_Equipment_Upgrade_Card_impl] dispatch_sig 判词：
//   ORIG 0x08227e00 T 709 insn（orig_insns>500 ⇒ §6.2 三件套口径）。
//   ① p167 调用点差集 CLEAR（81/81，0 missing / 0 extra）；
//   ② gap=3（ORIG 709 / 我方 706）；③ 栈槽集合与 ORIG 全等（-0xa..-0x198）。
//   ⇒ 语义完成、byte 层 N 封顶。
//   已修语义缺口（p167 差集定位）：set_ReturnSendPacket 内联→成员调用 ×3（MISSING -3，
//     同时消 put_byte/put_short/put_int EXTRA 各 3）；getCurCharacInvenR/W 逐调用点内联
//     （P0，MISSING -4/-2）；cardIdx/equipIdx 局部→直传字段地址；items[2] 数组（倒计数
//     隐式构造 ×2，0x08227ebf）+ rCard/rEquip 复用原槽；insertedSlots 非 const 迭代器
//     （P96，消 EXTRA 4 项）；out 循环缓存 end（P13）+ iterator→const_iterator 转换构造；
//     find_item 实参内联 + rew.m_addInfo/put_item_idx 字段口径（0x08228729/0x082288d7）；
//     slot 对结构体化复现 `lea -0x2c; add $0x2`；删 ORIG 不存在的 slot 预清与 booster 局部。
//   残余 N（语义等价、跨版本不可复现）：
//     N7 间接 call 目标寄存器——4.4.7 恒 %eax，ORIG 4.1 用 %edx（0x082281bf GetItemType、
//        0x08228749 make_item；本 TU 未路由 4.4.6，P230 分诊）。
//     跨版本 lowering——`if (slot >= 0)` 负数判定 ORIG 0x0822883f 为
//        `movzwl; not; shr $0xf,%ax; test %al,%al; je`（5 条），4.4 恒 `cmpw $0; js`（2 条）；
//        探针 /tmp/s303_probe.s、/tmp/s303_probe2.s 七形（<0 / >=0 / (ushort)>=0x8000 /
//        (uint)(ushort)>>15 / ~x>>15 等）全败。
//     末尾 1 条对齐 nop 属 §6.1 豁免。
// ============================================================================
int Dispatcher_Compound_Equipment_Upgrade_Card::dispatch_sig(CUser* user,
                                                             PacketBuf& packet)
{
    // ---- 1. 角色/状态校验：state==3 且存在当前角色才放行 ----
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return 0;

    // ---- 2. 频道校验：channel type 7（赛事/特殊频道）拒绝 ----
    if (G_GameWorld()->GetChannelType() == 7)
    {
        return LineFunc(0xe4c3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // ---- 3. 读包：slotCard / slotEquip / slotTarget ----
    // ORIG 无 movw $0 预清（0x08227e86 直接 lea）⇒ 声明不带初始化（P171 照抄）。
    // ORIG 0x08227e98 对第二个 short 取址用 `lea -0x2c; add $0x2`（相邻标量）
    // ⇒ 原码是 2 成员结构体（card@-0x2c、equip@-0x2a）+ 独立 slotTarget@-0x2e
    // （4.4 先声明浅 ⇒ 结构体占浅槽，P212/P309；成员取址复现 lea+add）。
    struct SlotPair
    {
        unsigned short card;
        unsigned short equip;
    };
    SlotPair slots;
    unsigned short slotTarget;
    packet.get_short(slots.card);
    packet.get_short(slots.equip);
    packet.get_short(slotTarget);

    // ---- 4. 取两个材料的物品 ----
    // [b28d-solo-DispCompoundUpgradeCard] ORIG 0x08227ebf 倒计数隐式构造循环 ×2
    // （esi=1→-1、步长 0x3d）⇒ 原码是 2 元素 Inven_Item 数组，且后续 rCard/rEquip
    // 复用同一对槽（0x08228451/0x08228545 写 -0x154/-0x117）。
    Inven_Item items[2];
    items[0] = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)slots.card);
    items[1] = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)slots.equip);

    // ---- 5. 依升级卡索引查合成目标（booster）索引；材料数量须 >0 ----
    // ORIG 0x082280d3/0x082280e0：直接 lea 字段地址（无 cardIdx/equipIdx 局部槽，
    // P0 槽-局部律）；形参为 unsigned long&，字段为 unsigned int（同宽 4B）⇒ 同址视图。
    unsigned long boosterIdx = GlobalData::s_pEvent_UpGrade_Jar
                                   ->getBoosterIndex_byUpgradeCardIndex(
                                       reinterpret_cast<unsigned long&>(items[0].m_addInfo),
                                       reinterpret_cast<unsigned long&>(items[1].m_addInfo));
    if (boosterIdx == 0 || items[0].m_addInfo2 <= 0 || items[1].m_addInfo2 <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x169, 0x13);
        return 0;
    }

    // ---- 6. 校验目标装备的合成配方 ----
    Inven_Item itemTarget = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)slotTarget);
    if (!GlobalData::s_pEvent_UpGrade_Jar->check_requiredRecipe(itemTarget))
    {
        return 0;
    }

    // ---- 7. 结果脚本必须是可堆叠助力（stackable booster，GetItemType==0xf） ----
    CItem* boosterScript = G_CDataManager()->find_item((int)boosterIdx);
    if (boosterScript == 0 || boosterScript->GetItemType() != 0xf)
        return 0;

    std::vector<stStackableBoosterItemInfo_t> out;
    // ORIG 0x082281eb 直接用 boosterScript(-0x14) 作 receiver，无独立 booster 局部
    // （多一个 4B 槽 ⇒ 全帧槽号 +4 平移，P0 槽-局部律）。
    static_cast<CStackableItem*>(boosterScript)->getBoosterItem(out);

    PacketGuard pg;
    pg.put_header(1, 0x169);
    std::vector<int> insertedSlots;

    if (out.empty() || out.size() >= 2)
    {
        // 结果异常 → 回失败包：put_byte(0), put_byte(0x13)
        pg.put_byte(0);
        pg.put_byte(0x13);
    }
    else
    {
        // ---- 合成成功：删除 3 个材料 ----
        // ORIG 每次调用点各发一次 getCurCharacInvenW()（0x08228295/0x082282d3/
        // 0x08228311），不缓存 invenW（p167 MISSING -2）。
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slotTarget, 1,
                                                (eItemDelReason)0x29, true);
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slots.card, 1,
                                                (eItemDelReason)0x29, true);
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slots.equip, 1,
                                                (eItemDelReason)0x29, true);

        // 删后重读各槽剩余数量：ORIG 写回 items[]/itemTarget 原槽并调用
        // set_ReturnSendPacket（ORIG T 0x08228a2e，本类成员，p167 MISSING -3）。
        itemTarget = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)slotTarget);
        items[0] = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)slots.card);
        items[1] = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)slots.equip);

        pg.put_byte(1);
        set_ReturnSendPacket(pg, slotTarget, itemTarget.m_addInfo2);
        set_ReturnSendPacket(pg, slots.card, items[0].m_addInfo2);
        set_ReturnSendPacket(pg, slots.equip, items[1].m_addInfo2);

        // ORIG 0x0822868e/0x082286b5：begin()/end() 各一次 + 转换构造到 const_iterator
        // （it@-0x58、e@-0x5c），循环条件只调 operator!=（P13 缓存 end）。
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it = out.begin(),
                 e = out.end();
             it != e; ++it)
        {
            // ORIG 0x082286e1：find_item 实参内联重读 it->m_itemIdx（第 1 次 operator->）。
            CItem* itemScript = G_CDataManager()->find_item((int)it->m_itemIdx);
            if (itemScript == 0)
                continue;

            Inven_Item rew;
            // ORIG 0x08228717..0x08228729：C1 → 写 rew+2(=m_addInfo) = it->m_itemIdx
            // （第 2 次 operator->），此后 put_item_idx 直接读该字段（0x082288d7）。
            rew.m_addInfo = it->m_itemIdx;
            itemScript->make_item(rew);
            // ORIG 0x08228757：booster 结构 +8 处 ushort 作为 add_info。
            // stStackableBoosterItemInfo_t 权威布局（+8 = m_addInfo，具名字段）。
            rew.set_add_info((int)it->m_addInfo);

            short slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                rew, (eItemAddReason)0x2b, true, true);
            // ORIG 0x0822884b 布局：slot<0 跳 mail（then 远置）、push_back 直落
            // ⇒ 源形取正极性 if(slot>=0){push_back}else{mail}（P222/P191）。
            if (slot >= 0)
            {
                insertedSlots.push_back((int)slot);
            }
            else
            {
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    user->getCurCharacName(), rew, 0, user->getCurCharacNo(),
                    0, 0, 0, user->GetServerGroup(), 0, 0);
            }
            pg.put_item_idx(rew.m_addInfo);
        }
    }

    pg.finalize(true);
    user->Send(pg);

    // ORIG 遍历用非 const iterator（__normal_iterator<int*>，P96）；
    // const_iterator 会多一组转换构造 + 混合 ne 实例（p167 EXTRA 4 项）。
    for (std::vector<int>::iterator it = insertedSlots.begin();
         it != insertedSlots.end(); ++it)
    {
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, *it);
    }
    return 0;
}