// [b28d-27] DIFF N18a/P75：ORIG 0x084714ec；ORIG 目标地址两段 lea
// （`lea -0x27(%ebp),%eax; lea 0xd(%eax),%edx`）并提前装载，4.4 恒折 `lea+add`
// 且延迟到压参（register asm 两向实测更差）⇒ 封顶。p167 CLEAR（3/3）。
void CMonitorServerProxy::SendGameServerInfo(char* name, unsigned short port,
                                             unsigned char group, unsigned char channel)
{
    Packet_Game_Server_Regist pkt;
    pkt.m_group = group;
    pkt.m_channel = channel;
    pkt.m_type = 1;
    pkt.m_port = port;
    // 残差（N18a 族，probe 实证）：ORIG 4.1 `lea -0x27(%ebp),%eax;
    // lea 0xd(%eax),%edx` 两段 lea 直落 %edx + name 经 %eax；4.4 恒折
    // `lea+add` 且实参装载寄存器互换。register asm(edx) 绑定 ndiff 6→7、
    // asm(eax) 6→8，均比直写差；`&pkt.m_name[0]` 与 `pkt.m_name` 同形（lea+add 后置）
    // ⇒ 保持源形，记 N。
    strncpy(pkt.m_name, name, 0x10);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}