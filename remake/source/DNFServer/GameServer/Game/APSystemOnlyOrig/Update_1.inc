// Update —— 报告汇编+反编译转写。ORIG T 0x08124cb2。
// [S5-2026-09-11 §6.5 核查] DB_UpdateRewardMedal::Update（AE 件）：SQL 串
//   `update charac_action_point set is_reward_medal=%u \t…\twhere charac_no = %u…`
//   TAB 敏感比对命中 ORIG；无 prett 引用；enc_kr_scan 0 mismatch。
bool DB_UpdateRewardMedal::Update(_SIG_UPDATE_REWARD_MEDAL& sig)
{
    // ORIG 0x08124cb8..+0x1e：GetDBHandle(s_db_mgr,2,0) 直接内联（非经辅助函
    // 数），db 存 -0x10 槽。ap_db_handle() 中继会退化为外联 call 差出调用点。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("update charac_action_point set is_reward_medal=%u \t\t\t\t\twhere charac_no = %u\t\t\t\t\t\tand is_reward_medal = %u \t\t\t\t\tand occ_date = '%s'",
                  1, sig.m_characNo, 0, sig.m_occDate);
    // ORIG 0x08124d21..+0x72：exec 返回 bool 先落 -0x9 槽再 movzbl 回传。
    bool ret = db->exec(true);
    return ret;
}