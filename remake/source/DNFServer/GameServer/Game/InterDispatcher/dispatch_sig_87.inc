int Inter_MouseRegister::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MouseRegisterPktView* pPkt = reinterpret_cast<Inter_MouseRegisterPktView*>(data);
    // P324：ORIG 前段是 action 分支（user->getPad() 链），SendCmdErrorPacket(0x74) 落在函数
    //   **后段** ⇒ 原码是 `if (pPkt->m_16 == 0) { 主体 } else { 错误包 }`，非旧稿的提前 return。
    if (pPkt->m_16 == 0)
    {
        // P0/P183：ORIG 无 pad 具名槽，每次重取 user->getPad()（call 返 eax 直作 this）；
        //   m_4 亦无槽往返（重读 0x4(%eax)）⇒ 全链式。
        if (pPkt->m_4 == 1 || pPkt->m_4 == 2)
        {
            user->getPad()->setActivate(true);
            user->getPad()->setCertified(true);
            user->getPad()->setPassword(data + 0x23);    // msg+0x23 密码
            user->getPad()->setCancelCnt(pPkt->m_1b);
            user->getPad()->setFailCnt(pPkt->m_1f);
        }
        else if (pPkt->m_4 == 3)
        {
            user->getPad()->setActivate(false);
            user->getPad()->setCertified(false);
            user->getPad()->setPassword(data + 0x23);
        }

        PacketGuard packet;
        packet.put_header(1, 0x74);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74,
                                 static_cast<unsigned char>(pPkt->m_16));
    }
    return 0;
