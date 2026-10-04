// ORIG 0x8403734：按 (DB_HANDLE_2, sig->m_serverGroup) 取句柄；
// game_channel 插入失败转 update 同键行，仍失败返 0；随后向 channel_occ_info
// 批量灌 100 行 (gc_no, age 1..100, 0)，失败转 update occ_num 清零，仍失败返 0。
// [b28d-4 P0 清绑] 原 `register unsigned int v/w asm("eax")` 已删（N96 全域禁绑定）；
// ORIG 0x8403961/0x8403980 两段 `mov -0x1c,%eax; cmp $0x62/%0x63,%eax` 物化，
// 4.4 对 unsigned 槽比较恒融合 `cmpl $imm,-0x1c` ⇒ N18a/P209 封顶，语义等价。
bool DB_InsertChannel::InsertChannel(SIG_INSERT_CHANNEL* sig)
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0x21/-0x20；gcc O0 按声明序分配）。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, sig->m_serverGroup);
    db->set_query(
        "inSert into game_channel (gc_no,gc_now,gc_ip,gc_port,gc_max,gc_game,gc_channel,gc_ch_group,gc_channeltype,gc_up_time, gc_type) values(%d,0,'%s',%d,%d,%d,'%s',%d,'%s',now(),%d)",
        sig->m_gcNo,
        sig->m_gcIp,
        sig->m_gcPort,
        sig->m_gcMax,
        sig->m_gcGame,
        sig->m_gcChannel,
        sig->m_gcChGroup,
        sig->m_gcChanneltype,
        sig->m_gcType);
    r = db->exec(true);
    if (!r)
    {
        db->set_query(
            "upDate game_channel set gc_now=0,gc_ip='%s',gc_port=%d,gc_max=%d,gc_game=%d,gc_channel='%s',gc_ch_group=%d,gc_channeltype='%s',gc_up_time=now(),gc_type=%d where gc_no=%d",
            sig->m_gcIp,
            sig->m_gcPort,
            sig->m_gcMax,
            sig->m_gcGame,
            sig->m_gcChannel,
            sig->m_gcChGroup,
            sig->m_gcChanneltype,
            sig->m_gcType,
            sig->m_gcNo);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    std::string str;
    str.clear();
    // ORIG 0x8403961-0x8403986 `mov -0x1c,%eax; cmp $0x62/%0x63,%eax; ja/setbe`
    // 两段物化；4.4 对 unsigned 槽比较恒融合 `cmpl $imm,-0x1c`。
    // [b28d-solo-2 P310] 局部改 int + 显式 (unsigned int) 比较即逐条复现两段形。
    int i;
    for (i = 0; (unsigned int)i <= 99u; ++i)
    {
        char buf[0x64] = { '\0' };
        sprintf(buf, "(%d, %d, 0)", sig->m_gcNo, i + 1);
        str.append(buf);
        if ((unsigned int)i < 99u)
            str.append(", ");
    }
    db->set_query("inSert into channel_occ_info (gc_no, age, occ_num) values %s",
                  str.c_str());
    r = db->exec(true);
    if (!r)
    {
        db->set_query("upDate channel_occ_info set occ_num = 0 where gc_no = %d",
                      sig->m_gcNo);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    return 1;
}