// [b28d-lane2] verdict=DIFF ndiff=119（ORIG 0x084d07d2）：槽序/帧差(N82/P212/P221)+混合残差；gap-15。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=235（ORIG 0x84d07d2）：N82/P112 帧槽平移+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_MouseRegister::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_MOUSE_REGISTER* mouse = reinterpret_cast<SIG_MOUSE_REGISTER*>(data);
    // P324：ORIG 前段是 action 分支（user->getPad() 链），SendCmdErrorPacket(0x74) 落在函数
    //   **后段** ⇒ 原码是 `if (*reinterpret_cast<int*>(mouse->m_pad16) == 0) { 主体 } else { 错误包 }`，非旧稿的提前 return。
    if (*reinterpret_cast<int*>(mouse->m_pad16) == 0)
    {
        // P0/P183：ORIG 无 pad 具名槽，每次重取 user->getPad()（call 返 eax 直作 this）；
        //   m_4 亦无槽往返（重读 0x4(%eax)）⇒ 全链式。
        if (mouse->m_pkgType == 1 || mouse->m_pkgType == 2)
        {
            user->getPad()->setActivate(true);
            user->getPad()->setCertified(true);
            // [H2·2026-09-11 laneA] 槽来源修正：ORIG 084d083e/084d08d8 两处均
            //   `mov -0xc(%ebp),%eax; lea 0x23(%eax),%ebx` —— 经 pPkt 具名槽取址；
            //   旧写 `data + 0x23`（形参 0x10(%ebp)）多发参数槽装载 ⇒ NEAR(nd=4)。
            user->getPad()->setPassword(mouse->m_padPassword);    // msg+0x23 密码
            user->getPad()->setCancelCnt(mouse->m_field1b);
            user->getPad()->setFailCnt(mouse->m_field1f);
        }
        else if (mouse->m_pkgType == 3)
        {
            user->getPad()->setActivate(false);
            user->getPad()->setCertified(false);
            user->getPad()->setPassword(mouse->m_padPassword);   // [H2·laneA] 同上
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
                                 static_cast<unsigned char>(*reinterpret_cast<int*>(mouse->m_pad16)));
    }
    return 0;
}