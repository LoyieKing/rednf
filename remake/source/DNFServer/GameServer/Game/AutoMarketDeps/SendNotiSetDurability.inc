void CAutoMarketConditionsControl::SendNotiSetDurability(CUser* user)
{
    // b28c-33：ORIG then 块尾直落 epilogue（无 jmp/nop 岛）⇒ 正向包裹形；
    // 早返形使 4.4 多发 `jmp <epilogue>; nop`（+2 条）。
    if (m_field3e8 != 0)
    {
        PacketGuard packet;
        MakeNotiSetDurability(packet,
                              static_cast<int>(m_durCurrent * 1000.0));
        user->Send(packet);
    }
