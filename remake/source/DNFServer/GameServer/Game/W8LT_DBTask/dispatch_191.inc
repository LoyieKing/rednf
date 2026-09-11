// ORIG 0x084227f6（_ZN23DB_SaveGoldZeroTradeLog8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；GetOutBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>；字段：
// +0 to_m_id(uint,NumberToString(,0))、+4 from_m_id(uint,NumberToString(,1))、
// +8/+0x1c from_nat_ip/from_peer_ip(str)、+0x30 from_nat_type(byte)、
// +0x31 from_mac_addr(str)、+0x72/+0x86 to_nat_ip/to_peer_ip(str)、
// +0x9a to_nat_type(byte)、+0x9b to_mac_addr(str)、+0xdc injury_money(uint)；
// inSert dnf_zeromoney_log(...)；返 exec(true)==1。
bool DB_SaveGoldZeroTradeLog::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_GOLD_ZERO_TRADE_LOG* sig =
        stream->GetOutBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_zeromoney_log(to_m_id,to_nat_ip,to_peer_ip,to_nat_type,from_m_id,from_nat_ip,from_peer_ip,from_nat_type,occ_time,injury_money,to_mac_addr,from_mac_addr) values(%s,'%s','%s',%d,%s,'%s','%s',%d,now(),%d,'%s','%s')",
        NumberToString(sig->m_to_m_id, 0),
        sig->m_to_nat_ip, sig->m_to_peer_ip,
        (int)sig->m_to_nat_type,
        NumberToString(sig->m_from_m_id, 1),
        sig->m_from_nat_ip, sig->m_from_peer_ip,
        (int)sig->m_from_nat_type,
        sig->m_injury_money, sig->m_to_mac_addr,
        sig->m_from_mac_addr);
    if (!db->exec(true))
        return 0;
    return 1;
}