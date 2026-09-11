// ORIG 0x08550612
void CLogGameChannel::DoSaveLog(SIG_SAVE_LOGGAMECHANNEL* pSig)
{
    MySQL* db = GetLogTargetDB();
    db->set_query(
        "inSert into log_game_channel(gc_up_time,gc_game,gc_no,gc_channel,"
        "gc_ch_group,gc_ip,gc_now,gc_new,gc_out) values(now(),%d,%d,'%s',%d,"
        "'%s',%d,%d,%d)",
        (int)(char)pSig->m_game,
        pSig->m_no,
        pSig->m_channel,
        (int)pSig->m_chGroup,
        pSig->m_ip,
        (int)pSig->m_now,
        (int)pSig->m_new,
        (int)pSig->m_out);
    db->exec(true);
}