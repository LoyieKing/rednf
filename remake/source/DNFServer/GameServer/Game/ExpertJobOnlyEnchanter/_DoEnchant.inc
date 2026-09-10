int CEnchanter::_DoEnchant(ENUM_TRY_ENCHANT_TYPE type)
{
    // [b28d-56 判词·N] 封顶 N82/P182：ORIG 0x0849cb2c 帧 `sub $0x60` vs 我们
    // `add $<A>,%esp`（大帧 4.4 用 add 形）；553/552 条、助记符序列同，仅帧/槽号差。
    // ---- 门禁块 1：对家有效且双方不同时校验登记费用 ----
    if (m_peer != 0 && m_peer->getCurCharacR() != 0 && m_user != m_peer)
    {
        // [b28d-solo-ExpertJobOnlyEnchanter] 真语义修正：ORIG 0x048-0x0d5 的门禁是
        // `if (bVar4 && m_field38 > 0) { if (bVar4b && CheckMoney!=1) return 0x11;
        //   if (get_money() < m_field38) return 0x11; }`——原 else-if 链漏了
        // ①m_field38 > 0 前置门禁；②bVar4b 为假时 get_money 检查仍须执行。
        if (m_field38 > 0)
        {
            // ORIG 条件照写：此处复核的是 m_peer 的当前角色（非 m_user）
            // [b28d-solo-ExpertJobOnlyEnchanter] ORIG 0x056-0x0a1 先把
            // `m_user != 0 && m_peer->getCurCharacR() != 0` 物化成 %al 岛再与
            // CheckMoney 短路；平铺三条件链 4.4 把 CheckMoney 提到物化之前
            // （残余顺序差）。P226 语句表达式形实测 553→558 劣化（+5 条），已回退。
            if (m_user != 0 && m_peer->getCurCharacR() != 0 &&
                m_user->CheckMoney(m_field38) != 1)
                return 0x11;
            if (m_peer->getCurCharacInvenR()->get_money() < m_field38)
                return 0x11;
        }
    }

    // ---- 材料槽与配方校验 ----
    // [b28d-solo-ExpertJobOnlyEnchanter] P198①/P15：ORIG 0x0da 有预清
    // `movl $0x0,-0x28(%ebp)` 且 `cmpl $0x0,-0x28; setne %al; test %al; je`
    // ⇒ 「声明=0 + 赋值即条件」两句式。
    int ret = 0;
    if ((ret = IsRightMaterial()) != 0)
        return ret;

    // [b28d-solo-ExpertJobOnlyEnchanter] P0/P134：ORIG 该 bool 无槽（`call; movzbl
    // %al,%ecx` 直传）⇒ 内联实参，勿具名（具名多槽 + movb 往返，帧 +4）。
    if (!IsExistRecipeItem(m_user, reinterpret_cast<CItem*>(m_field40),
                           IsUseOtherUserMonsterCard()))
        return 0x15;

    // [b28d-solo-ExpertJobOnlyEnchanter] ORIG 初始化/声明序（槽位实证）：
    // successSent(-0x21) → ref=0 预清(-0x20) → PacketGuard(-0x44) → invenType(-0x1c)
    // → ref=GetInvenRef(-0x20) → itemIndex0(-0x18) → oldCardRef(-0x14)
    // → addedCard=0(-0x10)。ref 预清为 P198① 两句式。
    bool successSent = false;
    Inven_Item* ref = 0;
    PacketGuard packet;
    int invenType =
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)m_materials[0].m_field0e);

    // [b28d-solo-ExpertJobOnlyEnchanter] P0：ORIG slot 值经 callee-saved 直传
    // （`movzwl 0x14(%eax),%ebx`），无具名槽 ⇒ 实参内联成员读。
    ref = ((CUser*)m_materials[0].m_user)->getCurCharacInvenW()
              ->GetInvenRef(invenType, m_materials[0].m_slot);
    if (ref == 0)
        return 0x11;

    if (GlobalData::s_rentalSystem->CheckRentableItem(1,
            (unsigned int)ref->m_addInfo))
        return 0x11;
    if (GlobalData::s_rentalSystem->CheckRentableItem(2,
            (unsigned int)ref->m_addInfo))
        return 0x11;

    int itemIndex0 = (int)m_materials[0].m_itemIndex;   // -0x18：历史日志参数
    int oldCardRef = ref->m_fieldd;                     // -0x14：写卡前快照
    CItem* addedCard = 0;                               // -0x10：本次写入的怪物卡物品

    if (_DrawEnchant())
    {
        addedCard = reinterpret_cast<CItem*>(m_field3c);
        if (addedCard != 0)
            ref->m_fieldd = addedCard->get_index();

        packet.clear();
        packet.put_header(0, 0xe);
        packet.put_byte((int)m_materials[0].m_field0e);
        packet.put_short(1);

        // [b28d-solo-ExpertJobOnlyEnchanter] 真语义修正（数据源用错）：ORIG
        // 0x2a0-0x2cd receiver = this+0x2c（m_peer）的 getCurCharacInvenR，
        // 非 m_materials[0].m_user（+0x8，仅 GetInvenRef 段用）。
        if (!m_peer->getCurCharacInvenR()->MakeItemPacket(
                (INVEN_TYPE)invenType, m_materials[0].m_slot, packet))
            return 0;   // ORIG：MakeItemPacket 失败直接返 0（packet 析构）

        packet.finalize(true);
        m_peer->Send(packet);
        successSent = true;
    }

    // ---- 统计：卡被消耗计数 ----
    if (addedCard != 0 && m_user != 0)
    {
        GetInstanceExpertJobStatistic()->increaseItemCount(
            (ENUM_EXPERT_JOB_TYPE)m_user->GetCurCharacExpertJobType(),
            (unsigned long)addedCard->get_index(), 1);
    }

    // ---- 主体验收包 + 配方材料扣除（含 exp/金钱回显）----
    if (m_user != 0 && m_user->getCurCharacR() != 0)
    {
        if (m_field38 > 0)
        {
            m_user->getCurCharacInvenW()->gain_money(m_field38,
                                                     (eMoneyAddReason)0x10, true,
                                                     0);
            m_user->SendUpdateItemList(CUser::eSendTarget_1,
                                       (ENUM_ITEMSPACE)0, 0);
        }

        packet.clear();
        packet.put_header(1, 0xac);
        packet.put_byte(1);
        packet.put_byte((int)type);
        packet.put_byte((int)(unsigned char)successSent);
        packet.put_int(m_user->GetCurCharacExpertJobExp());
        packet.put_int(m_user->getCurCharacInvenR()->get_money());

        // P15 赋值即条件（ORIG 0x4c5 `cmpl $0,-0x28; setne %al; test %al; je`）。
        if ((ret = UseMaterialOfRecipe(
                 m_user, reinterpret_cast<CItem*>(m_field40), &packet,
                 IsUseOtherUserMonsterCard())) != 0)
        {
            if (ref != 0)
                ref->m_fieldd = 0;
            return ret;
        }
        packet.finalize(true);
        m_user->Send(packet);
    }

    // ---- 对家结算：扣费/卡片删除/终止包 ----
    if (m_peer != 0 && m_peer->getCurCharacR() != 0 && m_user != m_peer)
    {
        if (m_field38 > 0)
        {
            m_peer->getCurCharacInvenW()->use_money(m_field38,
                                                    (eMoneySubReason)0x17,
                                                    true);
            m_peer->SendUpdateItemList(CUser::eSendTarget_1,
                                       (ENUM_ITEMSPACE)0, 0);
        }

        if (IsUseOtherUserMonsterCard())
        {
            bool delOk = m_peer->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, m_materials[1].m_slot, 1, (eItemDelReason)8,
                true);
            if (!delOk)
            {
                // [b28d-solo-ExpertJobOnlyEnchanter] P227 全表达式临时链：ORIG
                // 0x615-0x668 在 ctor 前求值 varargs（slot1→%esi、getCurCharacNo→%ebx
                // 跨 ctor 保活）⇒ 具名 trace 两语句形会把它推到 ctor 之后。
                cMyTrace(__PRETTY_FUNCTION__, 0x3d9, 5)(
                    "CUser::compound_item, getCurCharacInvenW()->delete_item failed , %d %d",
                    m_peer->getCurCharacNo(), (int)m_materials[1].m_slot);
            }

            packet.clear();
            packet.put_header(0, 0xe);
            packet.put_byte(0);
            packet.put_short(1);
            m_peer->getCurCharacInvenR()->MakeItemPacket(
                (INVEN_TYPE)1, m_materials[1].m_slot, packet);
            packet.finalize(true);
            m_peer->Send(packet);
        }

        packet.clear();
        packet.put_header(1, 0xab);
        packet.put_byte(1);
        packet.put_byte((int)(unsigned char)successSent);
        packet.finalize(true);
        m_peer->Send(packet);
    }

    // ---- 历史日志（CUser +0x79700 即 cUserHistoryLog 内嵌成员）----
    m_peer->m_historyLog.EnchantItem(itemIndex0, oldCardRef, ref->m_fieldd,
                                     successSent, m_peer->get_acc_name(),
                                     m_peer->getCurCharacName());
    return 0;
