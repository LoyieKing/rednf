// ============================================================================
// DB_UpdateAvatarHistory::dispatch —— ORIG 0x08445246（报告 dispatch.md 全文）
// ============================================================================
// 无基类前置。*stream >> uiId(uint) → >> characNo(int) →
// get_binary(agency, 0x18)（0x19 字节栈区清零，%s 尾零）→ GetDBHandle(3,0)
// → upDate user_items set to_ipg_agency_no='%s', m_time=now()
//   where ui_id=%d and charac_no=%d → 返回 exec(true)==1。
bool DB_UpdateAvatarHistory::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序照 ORIG 栈槽（低→高）：agency(-0x2d) characNo(-0x14)
    // uiId(-0x10) r(-0x11) db(-0xc)。uiId 初始化最先（ORIG movl $0 在
    // agency 展开之前）；agency 用初始化列表（ORIG 内联展开与 gcc 4.4
    // 的 ={0} 初始化展开一致，非 memset 调用）。
    unsigned int uiId = 0;
    int characNo;
    char agency[0x19] = {0};
    MySQL* db;
    *stream >> uiId;
    *stream >> characNo;
    stream->get_binary(agency, 0x18);
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set to_ipg_agency_no='%s', m_time=now() where ui_id=%d and charac_no=%d",
        agency, characNo, uiId);
    if (!db->exec(true))
        return 0;
    return 1;
}