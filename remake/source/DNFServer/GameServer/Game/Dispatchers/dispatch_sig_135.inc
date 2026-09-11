// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08204d8e orig/ours=246/240 ndiff-blocks=22
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x157(%ebp),%eax | add    $0xd,%eax | movl   $&_ZZN32Dispatcher_CreatureScriptMessage12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0
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

    // +0x93：5 段读取。[G1Csig] ORIG 直证：5 处 LineFunc 块**与判断同段就地**
    // （orig[53:62]…orig[116:124]），失败即 `call LineFunc; jmp 共享出口`；我方原用
    // goto 把 5 块搬到函数尾部（diff `insert orig[240] our[199:229]`）⇒ 改就地 return。
    if (!packet.get_byte((char&)chatType))
        return LineFunc(0x9cf3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(target))
        return LineFunc(0x9cf4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int((unsigned int&)fromId))
        return LineFunc(0x9cf5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(msgLen))
        return LineFunc(0x9cf6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(msg, 0x100, msgLen))
        return LineFunc(0x9cf7,
                        __PRETTY_FUNCTION__,
                        0, 0);

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
}