int DB_MouseRegister::checkPassword(unsigned int mid, const char* mousepass,
                                    bool param3, int& failCnt, int& cancelCnt,
                                    bool& validityExpired, char* ip, char* buf3)
{
    // ORIG 栈槽：0x8=mid 0xc=mousepass 0x10=param3 0x14=failCnt& 0x18=cancelCnt&
    // 0x1c=validityExpired& 0x20=ip 0x24=buf3（static 无 this 槽）。
    char local35[36];       // 明文缓冲（-0x31，strncpy dst / encryptAES src）
    char local59[36];       // encryptAES_ 密文缓冲（-0x55）
    char local11;           // version_info（-0xd）
    MySQL* db;              // -0xc

    db = _getDBHandleForDevideAccountDB();
    validityExpired = false;
    cancelCnt = 0;
    memset(ip, 0, 0x24);
    db->set_query(
        "seLect mousepass,fail_cnt,cancel_cnt, version_info from member_mousepass"
        " where m_id=%s and enable_flag = '1'",
        NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 1;
    }
    if (!db->get_str(0, ip, 0x24))
    {
        return 1;
    }
    if (!db->get_int(1, failCnt))
    {
        return 1;
    }
    if (!db->get_int(2, cancelCnt))
    {
        return 1;
    }
    if (param3 && failCnt > 2)
    {
        return 0x49;
    }
    if (!db->get_byte(3, local11))
    {
        return 1;
    }
    if (local11 == '\x01')
    {
        return 0xa6;
    }
    strncpy(local35, mousepass, 0x24);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, local35, local59);
    validityExpired = true;
    return 0;
}