// [b28d-solo-CGuildServerProxy N75/N107/N18a: 36/36] ORIG @0x0846f130 在压 strncpy
//   常量前 `lea -0x27(%ebp),%eax; lea 0xd(%eax),%edx` 两步保活 dst；4.4 恒右起求值
//   （dst 装载落最后）且折 `lea+add`。P226 语句表达式（({ pkt.m_name; })）、具名指针
//   中间量、P231 裸 register（v28 探针：双 lea+顺序对，但改用 %ebx 且 push/pop 破帧）
//   等 13 形实测均不闭合 ⇒ 封顶 | ORIG 0x0846f0f2
//   b28d-solo2 复验追加：`strncpy(&pkt.m_name[0], name, 0x10)`（P311 数组元素取址）
//   与 `pkt.m_name` 逐条同形（4.4 仍右起求值 + add）⇒ P75 跨版本求值序封顶不变。
void CGuildServerProxy::SendGameServerInfo(char* name, unsigned short port,
                                           unsigned char group, unsigned char channel)
{
    Packet_Game_Server_Regist pkt;
    pkt.m_group = group;
    pkt.m_channel = channel;
    pkt.m_type = 1;
    pkt.m_port = port;
    // [b28d-solo3 一次性旋钮·否] P316 volatile 具名 dst 指针
    // （`char* volatile dst = pkt.m_name;`）实测 36→38 劣化：dst 落栈槽往返
    // （`mov %eax,-S; mov -S,%eax`）且 dst 装载仍在压栈后 ⇒ 已回滚。
    strncpy(pkt.m_name, name, 0x10);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}