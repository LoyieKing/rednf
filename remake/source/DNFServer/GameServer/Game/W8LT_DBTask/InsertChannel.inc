bool DB_InsertChannel::InsertChannel(SIG_INSERT_CHANNEL* sig)
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0x21/-0x20；gcc O0 按声明序分配）。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, ((DB_InsertChannelView*)sig)->m_serverGroup);
    db->set_query(
        "inSert into game_channel (gc_no,gc_now,gc_ip,gc_port,gc_max,gc_game,gc_channel,gc_ch_group,gc_channeltype,gc_up_time, gc_type) values(%d,0,'%s',%d,%d,%d,'%s',%d,'%s',now(),%d)",
        ((DB_InsertChannelView*)sig)->m_gcNo,
        ((DB_InsertChannelView*)sig)->m_gcIp,
        ((DB_InsertChannelView*)sig)->m_gcPort,
        ((DB_InsertChannelView*)sig)->m_gcMax,
        ((DB_InsertChannelView*)sig)->m_gcGame,
        ((DB_InsertChannelView*)sig)->m_gcChannel,
        ((DB_InsertChannelView*)sig)->m_gcChGroup,
        ((DB_InsertChannelView*)sig)->m_gcChanneltype,
        ((DB_InsertChannelView*)sig)->m_gcType);
    r = db->exec(true);
    if (!r)
    {
        db->set_query(
            "upDate game_channel set gc_now=0,gc_ip='%s',gc_port=%d,gc_max=%d,gc_game=%d,gc_channel='%s',gc_ch_group=%d,gc_channeltype='%s',gc_up_time=now(),gc_type=%d where gc_no=%d",
            ((DB_InsertChannelView*)sig)->m_gcIp,
            ((DB_InsertChannelView*)sig)->m_gcPort,
            ((DB_InsertChannelView*)sig)->m_gcMax,
            ((DB_InsertChannelView*)sig)->m_gcGame,
            ((DB_InsertChannelView*)sig)->m_gcChannel,
            ((DB_InsertChannelView*)sig)->m_gcChGroup,
            ((DB_InsertChannelView*)sig)->m_gcChanneltype,
            ((DB_InsertChannelView*)sig)->m_gcType,
            ((DB_InsertChannelView*)sig)->m_gcNo);
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
        sprintf(buf, "(%d, %d, 0)", ((DB_InsertChannelView*)sig)->m_gcNo, i + 1);
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
                      ((DB_InsertChannelView*)sig)->m_gcNo);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    return 1;
