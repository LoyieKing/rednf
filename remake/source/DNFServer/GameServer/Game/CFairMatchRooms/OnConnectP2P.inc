// OnConnectP2P ORIG T 0x8564a8e 报告 OnConnectP2P.md
// 判例（ADeepY4[4]）：NEAR 196=196（原 DIFF 204）。已收：连接探测循环改 ORIG 形
//   `for(i=0;(i<8)&&(i<port);++i){if(addr[i]=='\0'){_SendMatchStatePacket(4,2);OnEndFight(room);return;}}`（4.4 收敛 ORIG 底部
//   跳转循环布局）、mgrTeam 局部内联直参、GetPlayCountPerOneTeam()&0xff→(unsigned char) 复现
//   movzbl、m_fieldAC==比较复现 sete；双层队员循环去 PlayerInfoThird& 引用、hoist m_user 读序
//   （ORIG 先读 m_user 存槽再判 m_flag，Send 用槽）。残余=+0x40 兜底虚槽调用 ORIG 用 %edx
//   承载 fn、我们用 %eax（寄存器文本差 _NEAR_）+ 双层索引 ORIG 用 lea 链、我们用更紧凑寻址
//   （计数已对称）。
// [b28 判词·N] AE-ndiff=6：残余全为 +0x40 兜底虚槽 call *%edx(ORIG) vs call *%eax
// (4.4) = N7（间接 call 目标恒 %eax，b26 NL28 8 件实证族）。其余逐条同。
void CFairMatch::OnConnectP2P(CUser* user, PvP_Room* room,
                              char* addr, int port)
{
    (void)user;
    if (m_field04 != 4) {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _ReturnTeam(room, room->get_manager_team(), 0);
        return;
    }

    int i;
    for (i = 0; (i < 8) && (i < port); ++i) {
        if (addr[i] == '\0') {
            // 未全部连上：发失败态并直接走 +0x40 兜底虚槽（OnEndFight）
            _SendMatchStatePacket(4, 2);
            OnEndFight(room);
            return;
        }
    }

    ++m_fieldAC;
    if (m_fieldAC == static_cast<int>((unsigned char)GetPlayCountPerOneTeam() * 2)) {
        PacketGuard pkt;
        pkt.put_header(0, 0x117);
        pkt.finalize(true);
        for (int t = 0; t < 2; ++t) {
            for (int p = 0; p < 4; ++p) {
                CUser* u = m_teams[t].m_players[p].m_user;
                if (m_teams[t].m_players[p].m_flag != 0 && u != 0)
                    u->Send(pkt);
            }
        }
        pkt.clear();
        room->battle_reset();
        for (int seat = 0; seat <= 7; ++seat) {
            if (room->get_user_seat(seat) != 0) {
                room->m_relayMgr.OnReady(seat, true);
            }
        }
        room->start_pvp();
        room->make_state_info(reinterpret_cast<char*>(&pkt));
        G_GameWorld()->send_all(pkt);
    }
}