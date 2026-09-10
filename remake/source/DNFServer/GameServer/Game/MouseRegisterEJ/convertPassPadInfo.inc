int DB_MouseRegister::convertPassPadInfo(unsigned int mid, char* mousepass,
                                         int& failCnt, char& cancelCnt,
                                         int& cancelCnt2,
                                         bool& validityExpired, char* ip,
                                         bool& flag2, char* buf3,
                                         long& validityTime, bool& flag3)
{
    validityExpired = false;

    MySQL* db = _getDBHandleForDevideAccountDB();
    db->set_query(
        "seLect mousepass, fail_cnt, cancel_cnt, version_info,"
        " validity_time>unix_timestamp(now()),"
        " adddate(now(),interval -1 month) >= occ_time from member_mousepass"
        " where m_id=%s and enable_flag = '1'",
        NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        cancelCnt = '\0';
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
    if (!db->get_int(2, cancelCnt2))
    {
        return 1;
    }
    char versionInfo;
    if (!db->get_byte(3, versionInfo))
    {
        return 1;
    }
    validityTime = 0;
    if (!db->get_uint(4, reinterpret_cast<unsigned int&>(validityTime)))
    {
        return 1;
    }
    char expiredFlag = '\0';
    if (!db->get_byte(5, expiredFlag))
    {
        return 1;
    }
    flag3 = (expiredFlag != 0);

    flag2 = false;
    if (validityTime != 0)
    {
        flag2 = true;
    }

    if (versionInfo == '\x02')
    {
        validityExpired = true;
        cancelCnt = '\x02';
        return 0;
    }

    cancelCnt = '\x01';
    {
        char plainPadded[36];
        char cipherHex[36];
        strncpy(plainPadded, mousepass, 0x24);
        encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_,
                    plainPadded, cipherHex);
        if (strncmp(ip, cipherHex, 0x24) != 0)
        {
            failCnt = failCnt + 1;
            _updateFailCnt(mid, failCnt, buf3);  // ORIG param_9=buf3（报告 +0x265）
            if (failCnt > 9)
            {
                return 0x49;
            }
            return 0x48;
        }
    }

    failCnt = 0;
    cancelCnt2 = 0;
    if (!_updateHistory(mid, 7, buf3))  // ORIG param_9=buf3（报告 +0x2ab）
    {
        return 1;
    }
    db->set_query(
        "upDate member_mousepass set enable_flag = '0' where m_id=%s",
        NumberToString(mid, 0));
    // ORIG 尾：`exec;xor$1;test;jne(ret1); getAffected;or %edx,%eax;test;
    // mov$1/mov$0; test %al; je` ⇒ 内联判定，无 bool 局部（`register bool`
    // 会强占 ebx 使帧变大 +0x64/push ebx）。
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return 1;
    }
    validityExpired = false;
    return 0;
