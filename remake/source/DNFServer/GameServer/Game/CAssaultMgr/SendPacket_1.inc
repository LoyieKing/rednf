bool CAssaultPlace::SendPacket(PacketGuard& packet)
{
    // ORIG T 0x82e6bf6：全员转发；任一 Send 失败则返回 false。
    bool ok = true;
    for (int i = 0; i < 8; ++i) {
        CUser* user = m_assaulters[i].GetUser();
        if (user != 0)
        {
            // [b28c-L7] P168a①：Main 已按 ORIG 串代落 CUser.h `bool Send(
            // PacketGuard&)`（同批 AntibotSend/SendCmdPacket/SendNotiPacket）⇒
            // 裸形直产 ORIG 0x082e6c2b `call; xor $0x1,%eax; test %al,%al; je`。
            if (!user->Send(packet))
                ok = false;
        }
    }
    return ok;
}