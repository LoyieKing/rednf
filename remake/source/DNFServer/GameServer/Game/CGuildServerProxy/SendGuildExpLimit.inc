// [b28d-solo-CGuildServerProxy N94: 43/42] 唯一残差=循环体 scaled-index 大常量拆分：
//   ORIG @0x0846ee96 `mov -0xc(%ebp),%edx; add $0x14aa,%edx; mov 0x1c(%eax,%edx,8),%eax`
//   （C%8=4 拆成 add+disp8），4.4 恒折单条 `mov 0xa56c(%eax,%edx,8),%eax`
//   （N94 定论：4.4 仅当 C%8==0 才拆，源形不可择）⇒ 封顶 | ORIG 0x0846ee44
//   b28d-solo2 复验追加：`*(m_levelUpCost[i] + 1)` 指针形劣化为 shl+add+add+mov 6 条；
//   P311 二维视图需 X*C=0x14aa 且第二维长 ≥0x11，本布局 0xa56c 非 8 倍数不可构造 ⇒ 定案。
// [P8S5 2026-09-11 负结论·N82 4.1 寻址分解族] ORIG 0x846ee93/96/9c：
//   `mov -0xc(%ebp),%edx; add $0x14aa,%edx; mov 0x1c(%eax,%edx,8),%eax`；
//   我方 = `mov 0xa56c(%eax,%edx,8),%eax`（0x14aa*8+0x1c = 0xa56c，同址）。
//   探针实测（/tmp/r5/p8s5/disp.cpp：struct{char pad[0xA550]; unsigned long x[32][2];}
//   + `o[i]=s->x[i][1]`）4.4 恒产 disp 形 `mov 0xa554(%edx,%ecx,8)` ⇒ 源码不可造，回滚态保留。
//   本波未改（gap +1 唯一残差）
void CGuildServerProxy::SendGuildExpLimit()
{
    Packet_Guild_Exp_Limit pkt;
    pkt.m_channel = G_CEnvironment()->get_channel_no();
    pkt.m_guildExpLimit8 = G_CDataManager()->m_guildParameter.m_levelUpCost[8][1];
    pkt.m_guildExpLimit16 = G_CDataManager()->m_guildParameter.m_levelUpCost[16][1];
    for (int i = 0; i < 0x11; i++)
    {
        pkt.m_guildExpLimit[i] = G_CDataManager()->m_guildParameter.m_levelUpCost[i][1];
    }
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}