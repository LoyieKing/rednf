int Dispatcher_CranePickUp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821d508（docs/class_func_reports/Dispatcher_CranePickUp/dispatch_sig.md）
    // +0x00：state != 3 → 错误 0x13
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e7, 0x13);
        return 0;
    }
    // +0x42：!isUseCraneStart → 错误 0x11
    if (!user->isUseCraneStart())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e7, 0x11);
        return 0;
    }
    // +0x79：复位抓娃娃启动标志
    user->setUseCraneStart(false);

    // ORIG 段直证：四槽初始化在**此处**（`movw $0xffff,-0x26; movl $<A>,-0x24;
    // movl $0x0,-0x10; movb $0x0,-0x9`，位于 state/isUseCraneStart 两门之后），
    // 非函数入口 ⇒ 声明随使用点下移（原入口处初始化使槽整体上移 0x10）。
    short slot = -1;                          // local_2a -0x26
    int pickIdx = -1;                         // local_28 -0x24
    unsigned int errCode = 0;                 // local_14 -0x10
    bool sendResultFail = false;              // local_d -0x9

    // +0x9a：读槽位/抽取序号
    if (!packet.get_short(slot))
        return LineFunc(0xd122,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(pickIdx))
        return LineFunc(0xd123,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x128：判定抓取
    errCode = 0;
    if (G_CGameManager()->GetCraneMinigameManager()->isPickSuccess(slot, pickIdx))
    {
        errCode = (unsigned int)G_CGameManager()->GetCraneMinigameManager()->sendItem(user, (int)slot);
        sendResultFail = (errCode == 0);
    }
    else
    {
        errCode = 0xbe;
    }

    // +0x190：sendItem 成功（errCode==0）→ 应答包；否则失败 trace + 错误包
    if (!sendResultFail)
    {
        PacketGuard guard;                                            // 0858dd4c
        guard.put_header(1, 0x1e7);
        guard.put_byte(1);
        guard.put_int(pickIdx);
        guard.put_short((short)G_CGameManager()->GetCraneMinigameManager()->getItemCnt((int)slot));
        guard.finalize(true);
        user->Send(guard);
        return 0;                                                     // guard 析构 0858de80
    }

    // +0x19f：失败路径
    {
        char* accIdStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd135, 0);
        trace("ServerMessage : Dispatcher_CranePickUp failed : m_id : %s error %d",
              accIdStr, errCode);
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e7, (unsigned char)(errCode & 0xff));
    return 0;
