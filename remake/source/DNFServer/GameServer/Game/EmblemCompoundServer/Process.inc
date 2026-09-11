// [b28d-solo-EmblemCompoundServer] Process 判词（ORIG 0x08191afa，426 insn）。
// 已修：补 `newItem.m_addInfo = resultItem->get_index()`（ORIG +0x2fc，原缺此步）；
// make_pair 改 `(unsigned long&)m_addInfo` 左值直传复现 ORIG 符号
// `_ZSt9make_pairIRmRiE…`（0x08111a37）；put_byte 实参改
// `Singleton<EmblemCompoundServer>::Get()->IsResultSelectable(grade0)`（ORIG +0x555，
// 非 this，且去 `?1:0` 物化）；resultIdx 改 unsigned int 复现 find_item 实参先装
// %ebx 跨 receiver call（P290/P52）；cost/invW 去具名局部（P157 重取）。
// 残余 N：ORIG +0x3e4 `movzwl; not; shr $0xf,%ax; test %al; jne`（4.1 对 short<0
// 的符号位提取 lowering）；4.4 对 `< 0` 恒 `cmpw $0; jns`，对 (unsigned short)~x>>15
// 形恒提升 32 位（movzwl+sar 6 条，实测）⇒ 跨版本 lowering 封顶（P196 负例族），
// gap 3 全在此段；其余助记符序列逐条同构（mnem-diff 仅此 1 块）。
bool EmblemCompoundServer::Process(CUser* user,
                                   arad::SigAradEmblemCompound* sig)
{
    unsigned int resultIdx = (unsigned int)GetEmblemRandomIndex(sig->m_grade0, sig->m_selectIdx);
    if (resultIdx == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x13);
        return false;
    }
    CItem* resultItem = G_CDataManager()->find_item(resultIdx);
    if (resultItem == 0 || resultItem->GetItemType() != 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x16);
        ARAD::PRINT_ERROR::find_item("EMBLEM COMPOUND", user, resultIdx);
        return false;
    }

    // ORIG +0xe1：m_cost[grade0] 内联实参（无 cost 槽，值保活 %ebx 跨 receiver call），
    // getCurCharacInvenW 每个调用点重取（P157/P52）。
    if (user->getCurCharacInvenW()->use_money(m_cost[sig->m_grade0],
                                              eMoneySubReason(0x23), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 10);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x24e,
            "EMBLEM COMPOUND : [money:%d] [need:%d] [charac_no:%d]",
            user->getCurCharacInvenR()->get_money(), m_cost[sig->m_grade0],
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    // 删除两张纹章材料（type1 slot0/slot1，reason=8）
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(1), (int)sig->m_slot0, 1,
                          eItemDelReason(8), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 1);
        ARAD::PRINT_ERROR::delete_item("EMBLEM COMPOUND", user, sig->m_slot0);
        return false;
    }
    if (user->getCurCharacInvenW()->delete_item(INVEN_TYPE(1), (int)sig->m_slot1, 1,
                          eItemDelReason(8), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 1);
        ARAD::PRINT_ERROR::delete_item("EMBLEM COMPOUND", user, sig->m_slot1);
        return false;
    }

    // 合成结果物品：ORIG +0x2ee：newItem 构造 → m_addInfo = resultItem->get_index()
    // → 虚 make_item(newItem)（vptr+8）→ set_add_info(1) → insert(reason=9)。
    Inven_Item newItem;
    newItem.m_addInfo = resultItem->get_index();
    resultItem->make_item(newItem);
    newItem.set_add_info(1);
    short insertSlot = (short)user->getCurCharacInvenW()->insertItemIntoInventory(
        newItem, eItemAddReason(9), true, true);
    // [b28d-solo-EmblemCompoundServer] N：ORIG +0x3e4 用 `movzwl; not; shr $0xf,%ax;
    // test %al; jne`（4.1 对 short<0 的符号位提取 lowering）；4.4 对 `< 0` 恒
    // `cmpw $0; jns`，对 (unsigned short)~x>>15 形恒提升 32 位（movzwl+sar，实测
    // 6 条 vs ORIG 5 条）⇒ 跨版本 lowering 封顶，保持自然语义。
    // [P6AuditOldN-S1 2026-09-11 试验] ORIG +0x3e4 `movzwl; not; shr $0xf,%ax;
    //   test %al; jne`（16 位）⇒ 试 `(unsigned short)x >> 15` 形（= x<0，语义等价）。
    if (((unsigned short)insertSlot) >> 15)
    {
        const char* title =
            g_scriptStringManager_.findString(4, "game_server_msg_122", (bool*)0);
        const char* body =
            g_scriptStringManager_.findString(4, "game_server_msg_123", (bool*)0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, newItem, 0, (unsigned int)user->getCurCharacNo(), body,
            (unsigned int)std::strlen(body), 0, user->GetServerGroup(),
            false, false);
    }
    sig->m_resultIdx = resultIdx;

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24e);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_short(sig->m_slot0);
    packet.put_short(sig->m_slot1);
    // ORIG +0x555：receiver 是 Singleton<EmblemCompoundServer>::Get()（08193776），
    // grade0 先物化 %ebx 跨 receiver call（P52）；bool 直传 put_byte（勿 ?1:0 物化）。
    packet.put_byte(
        ARAD::Singleton<EmblemCompoundServer>::Get()->IsResultSelectable(
            sig->m_grade0));
    packet.put_int(sig->m_resultIdx);
    packet.finalize(true);
    user->Send(packet);
    if (insertSlot >= 0)
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)insertSlot);

    getCompoundEmblemStatistic()->increaseCount((int)sig->m_grade0);

    std::vector<std::pair<int, int> > created;   // StatisticsEmblemCreate 入参
    // ORIG 0x08111a37 `make_pair<unsigned long&,int&>(unsigned long&,int&)`（IRmRiE）：
    // m_addInfo 以 unsigned long& 左值直传（P61/P315；值临时会实例化出另一符号）。
    created.push_back(std::make_pair((unsigned long&)newItem.m_addInfo,
                                     newItem.m_addInfo2));
    AvatarConvertServer::StatisticsEmblemCreate(created);
    return true;
}