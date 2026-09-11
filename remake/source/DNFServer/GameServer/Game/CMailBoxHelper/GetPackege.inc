// ---- 用户侧取件 ----
// [本波 2026-09-09 修，DIFF→AE] 旧判词「N82/P112 帧差 0x8c vs 0x9c + 全帧槽
// 平移 8」的真因是两枚多余具名局部，非版本常数：
//  ①P0/P183：money 具名局部（ORIG 0x08554ca7 值全程 %ecx，无栈槽往返）⇒ 删，
//    改内联 `mail->GetPackageMoney() % 100000 != 0`（调用次数 2=2 不变）；
//  ②P0：senderName 具名局部（ORIG 0x08554ce0/0x08554cf6 GetSenderName 两次
//    独立调用，strlen 结果 spill -0x5c）⇒ 删，改内联实参。
//  效果：帧 0x9c→0x8c、槽全部归位（box -0x20/mail -0x1c/items -0x34/err -0x38/
//  packet -0x44/it -0x48,-0x4c/removeList -0x50,-0x54 与 ORIG 逐一对齐）。
//  ③P253：`removeList[2] = {0, 0}` 补回 ORIG 0x08554df3/0x08554dfa 两条预清零
//    （无初始化列表时缺 2 条 store）⇒ gap 4→0。
//  复验 check_tu_routed：141/299/1/16 → 141/300/1/15；本件 verdict=AE。
void CMailBoxHelper::GetPackege(CUser* user, unsigned int mailIndex)
{
    // ORIG 0x085549de
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, 0x15);
        return;
    }
    CMailBox::CMail* mail = box->GetMail(mailIndex);
    if (mail == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, 0x15);
        return;
    }

    std::vector<std::pair<int, int> > items;
    ENUM_ERROR err = ENUM_ERROR_NONE;
    if (!_DoGetPackage(user, mail, items, true, err))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, (unsigned char)(err & 0xff));
        return;
    }
    _DoGetPackage(user, mail, items, false, err);

    PacketGuard packet;
    // P28/P13：ORIG 两次 iterator→const_iterator 转换构造 + ne<const,const>
    // （_ZNKSt6vector…3endEv 结果先转 const_iterator 再比较）。
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = items.end();
    for (; it != itEnd; ++it)
    {
        packet.put_header(0, 0xe);
        packet.put_byte(it->first);
        packet.put_short(1);
        if (it->first == 1)
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)2, it->second, packet);
        else if (it->first == 7)
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)3, it->second, packet);
        else
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, it->second, packet);
        packet.finalize(true);
        user->Send(packet);
    }

    // [P0/P183] ORIG 0x08554ca7：money 无栈槽（值全程 %ecx 承载除法链
    // `mov %eax,%ecx; mov %ecx,%eax; shr $0x5; …; mov %ecx,%edx; sub`）⇒
    // 原码是内联表达式而非具名局部（具名版多 -0x20 槽 + 全帧槽平移 8）。
    // ORIG 0x08554ce0/0x08554cf6 GetSenderName 两次独立调用（strlen 一次、
    // makeRequest 第 5 参一次），strlen 结果 spill -0x5c ⇒ 同样内联。
    if (mail->GetPackageMoney() % 100000 != 0)
    {
        DB_AccountHack_CheckMyAccount::makeRequest(
            user->GetUID(), user->get_acc_id(), 0, mail->GetPackageMoney(),
            const_cast<char*>(mail->GetSenderName()),
            (int)std::strlen(mail->GetSenderName()));
    }

    user->m_historyLog.RecvMail(mailIndex, mail->GetSenderName(), user->get_acc_name());

    packet.clear();
    packet.put_header(1, 0x62);
    packet.put_byte(1);
    packet.put_int((int)mailIndex);
    packet.finalize(true);
    user->Send(packet);

    // ORIG 0x08554df3/0x08554dfa：两元素各自 `movl $0x0` 预清零后才赋
    // 1/mailIndex ⇒ 原码数组带 `{0, 0}` 初始化列表（无初始化则缺 2 条 store）。
    unsigned int removeList[2] = { 0, 0 };
    removeList[0] = 1;
    removeList[1] = mailIndex;
    ReqDBRemoveMail(user, removeList, false);
    box->RemoveMail(mailIndex);
}