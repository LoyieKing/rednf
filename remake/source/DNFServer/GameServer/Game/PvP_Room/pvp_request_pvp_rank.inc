int PvP_Room::pvp_request_pvp_rank()
{
    // [b28d-7] P223：ORIG 0x085dcd92 jne 落空 then 占位岛 nop（0x085dce1f），
    // body 尾 jmp（0x085dce02）越过 PacketGuard landing pad 落 0x085dce20。
    // 复现形=否定 goto + 尾空标签（含初始化声明包入内层复合块）。
    if (m_rankRequested != 0)
        goto PVP_RANK_DONE;
    m_rankRequested = 1;
    {
        PacketGuard local_18;
        local_18.put_header(0, 0x31);
        local_18.finalize(true);
        send_to_pvp(local_18);
        m_fieldC0 = 1;
    }
PVP_RANK_DONE:
    ;
}