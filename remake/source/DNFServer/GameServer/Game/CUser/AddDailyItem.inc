void CUser::AddDailyItem()
{
    // 2026-09-10（G3CUser-B 负结论）：槽集对照 ORIG 48 槽 / 我方 48 槽，但整体平移
    //   +3 字节（ORIG tmp=-0x19a ↔ 我方 -0x19d；ORIG m_addInfo2 槽 -0x193 ↔
    //   我方 -0x196）⇒ 我方在 Inven_Item tmp 之前多出 3 字节对象（1~3 个 char/bool
    //   具名量）。本轮已核 def/tmp/slot/sender/body/packet 六个具名量均与 ORIG 一一
    //   对应（ORIG 各有槽），未定位到第 7 个多余量；30 余条 diff 亦全为跨版本
    //   寄存器/槽号形（含 ORIG 0x08656e34 段 `cmp $0x2; jg` 与 0x08656f60 尾部
    //   `lea -0x8(%ebp),%esp` 序）。判 N 封顶，不再动。
    // b28d-solo-CUser 试形（P0 槽往返）：ORIG 0x08656caa 无 level 槽——call 结果直比
    //   `cmp $0x13,%eax; setg %al; test; je`；我方具名 level 落 -0x20 并被 mem 直比。
    if (get_charac_level() > 0x13)
    {
        CItem* def = G_CDataManager()->find_item(0x1057);
        if (def != 0)
        {
            Inven_Item tmp;
            int slot = getCurCharacInvenR()->GetInvenData(0x1057, tmp);
            if (slot < 0)
            {
                def->make_item(tmp);
                // ORIG 0x08656d4c/0x08656d56：先 m_addInfo2(+7)=3 再 m_addInfo(+2)=0x1057
                tmp.m_addInfo2 = 3;   // 数量
                tmp.m_addInfo = 0x1057;
                slot = getCurCharacInvenW()->insertItemIntoInventory(
                    tmp, eItemAddReason_0xd, true, false);
            }
            else if (tmp.m_addInfo2 < 3)
            {
                m_historyLog.ItemAdd((INVEN_TYPE)1, 3 - tmp.m_addInfo2,
                                     tmp.get_add_info(), tmp,
                                     (eItemAddReason)0xd);
                tmp.m_addInfo2 = 3;
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, tmp);
            }
            if (slot >= 0)
                SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }
    std::vector<std::pair<int, int> > resetList;
    std::vector<std::pair<int, int> > delList;
    ResetItemByScript(resetList);
    RefillItemByScript(resetList);
    deleteSpecificItem(resetList, delList);
    // b28d-solo-CUser p167：ORIG 0x08656caa 用非 const `iterator`（我方 const_iterator
    // 多转换构造 + 混合 operator!=）⇒ 改非 const iterator。
    // ORIG 0x08656d??：循环用**后置** `it++`（call …normal_iterator…ppEi，非前置 ppEv）。
    for (std::vector<std::pair<int, int> >::iterator it = delList.begin();
         it != delList.end(); it++)
    {
        // ORIG 0x08657000..0x08657076：first 两条件 `||` 物化（mov $1/$0 + test，
        //   无槽）后与 second<0 各自落独立 nop 岛（0x3cb/0x3ce）⇒ P223 双守卫形。
        if (it->first < 0 || it->first > 0x1b)
            goto skipFirst;
        if (it->second < 0)
            goto skipSecond;
        SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)it->first,
                           it->second);
        goto skipJoin;
        skipFirst:;
        skipSecond:;
        skipJoin:;
    }

    m_advanceAltar.refillTicketFree();   // ORIG 0x0865f6dc：每日门票免费补满
    // ORIG 0x086570e1：`cmp $0x4811f1ef,%eax; setg %al; test; je 续` ⇒ 早返形
    //   （写 `< 0x4811f1f0` 会产 setl，形态不符）。
    if (GlobalData::s_systemTime_.getCurSec() > (time_t)0x4811f1ef)
        return;
    CItem* def = G_CDataManager()->find_item(0x1cab);
    if (def == 0)               // ORIG 0x0865710f `cmpl $0,-0x18; jne 续` ⇒ 早返
        return;
    Inven_Item tmp;
    def->make_item(tmp);
    // ORIG 0x08657149/0x08657153：先 m_addInfo2(+7)=1 再 m_addInfo(+2)=0x1cab
    tmp.m_addInfo2 = 1;
    tmp.m_addInfo = 0x1cab;
    int slot = -1;              // ORIG 0x0865715d `movl $0xffffffff,-0x14` 预清（P198①）
    slot = getCurCharacInvenW()->insertItemIntoInventory(
        tmp, eItemAddReason_0xd, true, true);
    if (slot == -1)
    {
        char sender[0x15];
        strncpy(sender,
                g_scriptStringManager_.findString(
                    4, "game_server_msg_24", 0),
                0x14);
        char body[0x100];
        strncpy(body,
                g_scriptStringManager_.findString(
                    4, "game_server_msg_25", 0),
                0xff);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, tmp, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false, false);
    }
    else
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        // ORIG 0x08657392 `xor $1,%eax; test %al,%al; je succ` ⇒ 原码为
        //   `if (!MakeItemPacket(...)) { 空 } else { finalize+Send }`（P292 负形）。
        if (!getCurCharacInvenR()->MakeItemPacket(
                (INVEN_TYPE)1, slot, packet))
        {
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
