// ============================================================================
// IsPCRoomIP（ORIG T 0x08412c52）
// ============================================================================

bool DB_Login::IsPCRoomIP(char* ip)
{
    MySQL* db = _GetDNFAccountDB();
    if (ip == 0 || ip[0] == '\0')
    {
        return false;
    }
    std::string prefix(ip);     // 去掉最后一段 ".x"：a.b.c → a.b
    std::string suffix(ip);     // 取最后一段数字：x（start_ip/end_ip 比较值）
    prefix.erase(prefix.find_last_of("."), std::string::npos);
    int lastOctet = atoi(suffix.erase(0, suffix.find_last_of(".") + 1).c_str());
    db->set_query("seLect count(*) from dnf_pcroom where ip='%s' and start_ip <= %d and end_ip >= %d",
                  prefix.c_str(), lastOctet, lastOctet);
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }
    int cnt = 0;
    if (!db->get_int(0, cnt) || cnt == 0)
    {
        return false;
    }
    return true;
}
