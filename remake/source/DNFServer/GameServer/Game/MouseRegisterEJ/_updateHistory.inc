bool DB_MouseRegister::_updateHistory(unsigned int mid, int modifyType, char* ip)
{
    MySQL* db = _getDBHandleForDevideAccountDB();
    db->set_query("seLect mousepass from member_mousepass where m_id=%s",
                  NumberToString(mid, 0));
    if (!(db->exec(true) && db->get_n_rows() != 0 && db->fetch()))
    {
        return false;
    }

    char prevMousepass[36];
    if (!db->get_str(0, prevMousepass, 0x24))
    {
        return false;
    }

    char ym[10] = { '\0' };
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm lt;
    localtime_r(&now, &lt);
    strftime(ym, 10, "%Y%m", &lt);
    db->set_query(
        "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass,"
        " modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
        ym, NumberToString(mid, 0), prevMousepass, modifyType, ip);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return false;
    }
    return true;
