bool DB_UpdateGoblinPadAdvicePwdRewardTime::dispatch(int userIdx, int slot,
                                                     Stream* stream)
{
    // 无 bool 槽：`if (!exec || affected==0) return 0; return 1;` 使 gcc4.4 在
    // 聚合块直生 mov $1/jmp/mov $0 + test %al/je（探针实证与 ORIG 0x843f64e
    // 逐条同构；写作 return !(...) 会被 De Morgan 折叠丢 xor）。mId 占 -0x10。
    unsigned int mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    mId = 0;
    *stream >> mId;
    db->set_query(
        "upDate member_mousepass set reward_time=0 where m_id=%s and enable_flag = '1'",
        NumberToString(mId, 0));
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
        return 0;
    return 1;
