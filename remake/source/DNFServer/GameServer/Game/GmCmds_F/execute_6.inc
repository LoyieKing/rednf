// ORIG execute 0x084b34f8：set_multiboxLotteryItemFailCnt(v)；v > 2 时追加
// 下发 0x1c6 空包头通知客户端（多重盒子抽取失败次数刷新）。
// [b28d-solo-GmCmds_F] AE：删具名 int cnt 局部（P0：ORIG 0x084b34f8 两次直读 this+0x8，
// 无槽往返 ⇒ 原码无该局部）后帧 0x30→0x20、user 槽 -0x10→-0xc、PacketGuard -0x1c→-0x18
// 全对齐；61=61，仅 .o 重定位占位差（AE）。
// ORIG 报告：docs/class_func_reports/game_master__CSetMultiboxFailCnt/execute.md。
void CSetMultiboxFailCnt::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    user->set_multiboxLotteryItemFailCnt(m_failCnt);
    if (m_failCnt > 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x1c6);
        packet.finalize(true);
        user->Send(packet);
    }
}