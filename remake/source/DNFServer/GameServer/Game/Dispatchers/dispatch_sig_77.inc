int Dispatcher_MultiMailBoxSend::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_MULTI_MAILBOX_SEND msg;   // sizeof 0x1a6（set_MultimailMsgData memcpy 定案）
    // [b28d-solo-Dispatchers] P171：ORIG 0x082262a7 `sub $0x4160` 后直接 get_state，
    //   全函数无 memset/rep stos 清零 ⇒ 原码无整体清零，删除我方多余 memset
    //   （p167 EXTRA `memset` +1 即此项；我方帧 0x4170 vs ORIG 0x4160）。

    if (user == 0 || user->get_state() != 3 || user->getCurCharacR() == 0)
        return 0;

    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x40);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c,
                                 (unsigned char)(secuErr & 0xff));
        return 0;
    }
    if (user->isHackUser())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x17);
        return 0;
    }

    if (!packet.get_int(msg.m_flagD))
        return LineFunc(0xe1ea,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (msg.m_flagD < 1)
        return LineFunc(0xe1ed,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(msg.m_title11, 0x1e, msg.m_flagD))
        return LineFunc(0xe1f0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(msg.m_gold2f))
        return LineFunc(0xe1f3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    msg.m_count33 = 1;   // ORIG movb $1 初始化后被覆盖
    if (!packet.get_byte(msg.m_count33))
        return LineFunc(0xe1f7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (msg.m_count33 > 10)
        return LineFunc(0xe1fa,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // 条目区 stride 0xb：space@+0、slot@+1、seq u64@+3、price int 与 seq 低 32 位同址
    // ORIG 0x08226512/0x082267f7：`movl $0,-0x18` + 尾测
    //   `movzbl count; movzbl %al; cmp -0x18; setg; test; jne` ⇒ 下标为 int
    //   （P218/P51；uchar 下标产 seta/cmpb 且帧槽错位）。
    for (int i = 0; i < (int)msg.m_count33; ++i)
    {
        // [b28d-solo-Dispatchers] P183 地址重物化：ORIG 每处字段访问都重发完整
        //   `mov i,%edx; shl $2; add; add; add; add $0x30; lea (%ecx,%eax,1); add $K`
        //   全链（0822651e/0822657c/082265da/08226638/0822667d/082266db），无 e 指针槽。
        // ORIG 0x08226538：get_byte 目标即 e+0 本体（无临时槽，P0 槽-局部律）。
        if (!packet.get_byte(*(unsigned char*)(msg.m_body34 + i * 0xb)))
            return LineFunc(0xe1ff,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*(unsigned short*)(msg.m_body34 + i * 0xb + 1)))
            return LineFunc(0xe202,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // ORIG 0x082265da：get_int 直接落 e+3（u64 槽，无临时变量/memcpy）。
        if (!packet.get_int(*(unsigned long*)(msg.m_body34 + i * 0xb + 3)))
            return LineFunc(0xe205,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // ORIG 0x08226638..0x08226678：seq 低 32 位为 0 → 错误包 0x11 并返回
        //（非 break）。
        if (*(const int*)(msg.m_body34 + i * 0xb + 3) == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x11);
            return 0;
        }
        if (!packet.get_int(*(int*)(msg.m_body34 + i * 0xb + 7)))
            return LineFunc(0xe20e,
                            __PRETTY_FUNCTION__,
                            0, 0);

        // ORIG 0x0822671d/0x082267a4：invenType 与 slot 两处各自重取（P183）。
        if (user->CheckItemLock(
                GetInvenTypeFromItemSpace(
                    (ENUM_ITEMSPACE)(unsigned char)msg.m_body34[i * 0xb]),
                (int)*(const unsigned short*)(msg.m_body34 + i * 0xb + 1)) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0xd5);
            return 0;
        }
        bool movable = true;
        CheckItemTradeLimitCountToMoveByUser(
            *user,
            GetInvenTypeFromItemSpace(
                (ENUM_ITEMSPACE)(unsigned char)msg.m_body34[i * 0xb]),
            (int)*(const unsigned short*)(msg.m_body34 + i * 0xb + 1), movable);
        if (!movable)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x72);
            return 0;
        }
    }

    // ORIG 0x0822680f：读长度前先清 m_msgLenA2 与正文缓冲。
    msg.m_msgLenA2 = 0;
    memset(msg.m_msgTextA6, 0, sizeof(msg.m_msgTextA6));
    if (!packet.get_int(msg.m_msgLenA2))
        return LineFunc(0xe229,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(msg.m_msgTextA6, 0x100, msg.m_msgLenA2))
        return LineFunc(0xe22b,
                        __PRETTY_FUNCTION__,
                        0, 0);

    std::string msgText(msg.m_msgTextA6);
    if (CSyncSlangFilter::GetInstance()->HasSlang(msgText))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x9f);
        return 0;
    }
    if ((int)strlen(msg.m_msgTextA6) != msg.m_msgLenA2)
        return LineFunc(0xe237,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (user->GetReliablePerson() != 1 &&
        !user->IsAvailableCurCharacTradeGoldDaily(msg.m_gold2f))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x46);
        return 0;
    }

    // ORIG 0x08226a12..0x08226a39：短路链直接在 %eax 物化（无 needCertify 槽，
    //   P0）⇒ 原码为表达式直用。reqSendMailCertify 定义属 W7-CUser 域（Main 已排期）。
    if (msg.m_gold2f != 0 && user->reqSendMailCertify() != 0)
    {
        user->set_MultimailMsgData(&msg);
        return 0;
    }

    MSG_CHECK_MAIL_RECEIVER check;
    check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_2>(msg);
    int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(user, msg.m_title11,
                                                          &check);
    if (rc != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c,
                                 (unsigned char)(rc & 0xff));
        return 0;
    }

    unsigned char hackFlag = 0;
    // ORIG 0x08226ac3/0x08226b89：下标 `movl $0,-0x10` + 尾测 setg ⇒ int（P218/P51）。
    for (int i = 0; i < (int)msg.m_count33; ++i)
    {
        // ORIG 0x08226acf..0x08226af9：seq 低 32 位从 e+3 读出后直接入 %ebx 作
        //   find_item 实参（无 protoIdx 具名槽，P0）⇒ 表达式内联。
        CItem* proto = G_CDataManager()->find_item(
            *(const int*)(msg.m_body34 + i * 0xb + 3));
        // ORIG 0x08226b07..0x08226b40：`call *vptr+0x10; xor $1; test; je`（bool 直判，
        //   P32/P292）⇒ 写 `!IsAvatarItem()`，勿写 `== 1`。
        if (proto != 0 && !proto->IsAvatarItem() &&
            proto->get_rarity() <= 1)
        {
            hackFlag = 1;
        }
        // ORIG 0x08226b44：重新完整计算 e+3 地址再读（P183）。
        if (*(const int*)(msg.m_body34 + i * 0xb + 3) == 0)
            hackFlag = 1;
        user->IncrementTradeGold(msg.m_gold2f, (int)hackFlag);
    }
    return 0;
