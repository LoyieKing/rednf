// ---------------------------------------------------------------------------
// DB_UpdateGoblinPadAdvicePwdRewardTime::dispatch —— ORIG 0x0843f5d2
// ---------------------------------------------------------------------------
// ORIG：GetDBHandle(6,0) → >>mId(uint) → NumberToString(mId,0) → set_query(
// "upDate member_mousepass set reward_time=0 where m_id=%s and enable_flag = '1'",
// s) → exec(true)。exec==1 且 getAffectedRowCount()!=0 → 返 1，否则返 0。
// 声明序 mId→db 与 ORIG 栈槽对应（-0x10/-0xc）；GetDBHandle 初始化先于
// mId=0（ORIG 0x06 先 call 后 movl $0）。
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
}