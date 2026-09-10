bool DB_UpdateActionPoint::Update(bool bExpireDate, const _SIG_LOAD_ACTION_POINT& sig)
{
    // 伪 helper 归一：ORIG 直接调 DBMgr::GetDBHandle
    // [b28d-solo-APSystemOnlyOrig] 残差 = 全帧槽基浅 4（db -0x24↔-0x20、
    // now -0x3c↔-0x38、dateTm -0x68↔-0x64、date -0x30↔-0x2c）+ 帧 0xbc↔0xac；
    // P190 double framePad 实测反劣（-0x6c→-0x7c、diff 268→280）⇒ N82/N115 封顶。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    struct tm dateTm;
    if (bExpireDate)   // localTime(now-0x15180)
    {
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x081243ca..0x081243e5：单个
        // time_t 局部 -0x38，`now = now - 0x15180` 为 load/sub/store 三形
        // （地址被 localtime_r 取用，故非内存 add）；旧版 int now + time_t tt
        // 双局部多 4 条 + 帧 +0x10。
        time_t now = GlobalData::s_systemTime_.getCurSec();
        now = now - 0x15180;
        localtime_r(&now, &dateTm);
    }
    else
    {
        __GetCurrentDNFDate(dateTm);
    }
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x081243f7：memset 长度 0xc（date 槽
    // -0x2c 起 12 字节；旧版 11 字节短 1）。
    char date[ConstOCC_DATELength + 1];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02d", dateTm.tm_year + 1900, dateTm.tm_mon + 1,
            dateTm.tm_mday);

    std::string clearState;
    MakeActionClearStateString(clearState, sig.m_actionPoints, 300);

    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124472..0x081244d7 指令序 =
    // `zipLen=0x400` → `cmp $0x7ff; jbe`（容量检查）→ memset → compress_zip；
    // 旧版把检查放在 compress 之后、memset 放在 zipLen 之前 ⇒ 整段错位。
    unsigned int zipLen = 0x400;
    if (zipLen > 0x7ff)
        return false;
    memset(g_szZipAP_CLEAR_STATE, 0, ConstMaxZipAP_CLEAR_STATE);
    if (!compress_zip(g_szZipAP_CLEAR_STATE, zipLen, clearState.c_str(),
                      static_cast<int>(clearState.size())))
    {
        return false;
    }

    char* blobStr = db->blob_to_str(0, g_szZipAP_CLEAR_STATE, zipLen);
    db->set_query("update charac_action_point set ap_sum=%d, is_reward_medal=%u, \t\t\t\t\tis_reward_item_1=%u, is_reward_item_2=%u, is_reward_item_3=%u, is_reward_item_4=%u,\t\t\t\t\t\tap_clear_state='%s'\t\t\t\t\t\twhere occ_date='%s'\t\t\t\t\t\tand charac_no=%u",
                  sig.m_apSum, sig.m_isRewardMedal, sig.m_isRewardItem[0],
                  sig.m_isRewardItem[1], sig.m_isRewardItem[2],
                  sig.m_isRewardItem[3], blobStr, date, sig.m_characNo);
    bool execOk = db->exec(true);
    if (!execOk || db->getAffectedRowCount() == 0)
    {
        blobStr = db->blob_to_str(0, g_szZipAP_CLEAR_STATE, zipLen);
        db->set_query("insert into charac_action_point\t\t\t\t\t\t\t(occ_date, charac_no, ap_sum, is_reward_medal, \t\t\t\t\t\t\tis_reward_item_1, is_reward_item_2, is_reward_item_3, is_reward_item_4,\t\t\t\t\t\t\t\tap_clear_state)\t\t\t\t\t\t\t\tvalues \t\t\t\t\t\t\t('%s', %u, %u, %u, %u, %u, %u, %u, '%s')",
                      date, sig.m_characNo, sig.m_apSum, sig.m_isRewardMedal,
                      sig.m_isRewardItem[0], sig.m_isRewardItem[1],
                      sig.m_isRewardItem[2], sig.m_isRewardItem[3], blobStr);
        execOk = db->exec(true);
    }
    return execOk;
