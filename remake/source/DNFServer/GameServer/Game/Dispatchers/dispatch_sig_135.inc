int Dispatcher_CreatureScriptMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08204d8e（docs/class_func_reports/Dispatcher_CreatureScriptMessage/dispatch_sig.md）
    // -0x157 起为局部聊天消息缓冲（ORIG 独立字段布局）
    unsigned char chatType;                   // base+0x0d  local_14e
    unsigned short target;                    // base+0x0e  local_14d
    unsigned int fromId;                      // base+0x10  local_14b（unsigned get_int）
    int recvNameLen;                          // base+0x14  local_147
    char recvName[0x1e];                      // base+0x18  acStack_143
    int msgLen;                               // base+0x36  local_125
    char msg[0x100];                          // base+0x3a  acStack_121

    // +0x00：state < 3 → return 0
    if (user->get_state() < 3)
        return 0;
    // +0x2b：getCurCharacVill() == 0 → return 0
    if (user->getCurCharacVill() == 0)
        return 0;
    // +0x49：getCurCharacR() == 0 → return 0
    if (user->getCurCharacR() == 0)
        return 0;
    // +0x67：IsEquippedCreature(GetCreatureMgrR(getCurCharacInvenR())) != 1 → return 0
    if (!user->getCurCharacInvenR()->GetCreatureMgrR()->IsEquippedCreature())
        return 0;

    // +0x93：5 段读取（每段失败 → LineFunc 对应行号后 jmp 尾部）
    if (!packet.get_byte((char&)chatType))
        goto L_9cf3;
    if (!packet.get_short(target))
        goto L_9cf4;
    if (!packet.get_int((unsigned int&)fromId))
        goto L_9cf5;
    if (!packet.get_int(msgLen))
        goto L_9cf6;
    if (!packet.get_str(msg, 0x100, msgLen))
        goto L_9cf7;

    // +0x20d：chatType==7 或 ==1 → 继续读接收方信息
    if (chatType == 7 || chatType == 1)
    {
        if (!packet.get_int(recvNameLen))
            return LineFunc(0x9cfb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_str(recvName, 0x1e, recvNameLen))
            return LineFunc(0x9cfc,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (chatType == 7)
        {
            Packet_Monitor_Other_Channel_Chat notify;                     // ctor 0822b8e6
            notify.m_characNo = user->getCurCharacNo();                   // +0x13
            notify.m_buddyNameLen = (char)recvNameLen;                    // +0x1b
            memcpy(notify.buddy_n_user_id_what, recvName, recvNameLen);   // +0x1c
            notify.m_msgLen = (unsigned char)msgLen;                      // +0x3a
            memcpy(notify.m_msg, msg, msgLen);                            // +0x3b
            {
                unsigned short total = (unsigned short)((unsigned char)msgLen + 0x3b);  // local_294
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
                proxy->SendPacket((char*)&notify, (int)(unsigned int)total);
            }
            return 0;
        }
    }

    // +0x3bd：普通频道聊天
    G_GameWorld()->send_chat_msg(user, (char)chatType, target, fromId,
                                 msg, msgLen, 0, 0, (ENUM_NOTIPACKET)0x77);
    return 0;

L_9cf3:
    return LineFunc(0x9cf3,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf4:
    return LineFunc(0x9cf4,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf5:
    return LineFunc(0x9cf5,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf6:
    return LineFunc(0x9cf6,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf7:
    return LineFunc(0x9cf7,
                    __PRETTY_FUNCTION__,
                    0, 0);
