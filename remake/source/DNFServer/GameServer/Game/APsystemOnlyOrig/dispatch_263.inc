// ============================================================================
// DB_InsertRandomOptionItemInform::dispatch —— ORIG 0x08442c6a
// GetDBHandle(DB_HANDLE_1,0) → >>m_id(u32) >>checkCount(uchar) → inSert
// check_pick_up_random_option_item(m_id, check_count) values(%s, %u)。
// exec 失败返 0（ORIG 附 cMyTrace 日志，语义无关省略），成功返 1。
// ============================================================================
bool DB_InsertRandomOptionItemInform::dispatch(int userIdx, int slot,
                                               Stream* stream)
{
    unsigned int m_id = 0;
    unsigned char checkCount = 0;
    *stream >> m_id;
    *stream >> checkCount;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into check_pick_up_random_option_item(m_id, check_count) values(%s, %u)",
        NumberToString(m_id, 0), (unsigned int)checkCount);
    if (!db->exec(true))
    {
        // ORIG 0x08442d42：cMyTrace(__PRETTY_FUNCTION__, 0xa47e, 5) + trace 日志。
        // ORIG 0x08442d2a `mov %eax,%ebx`：NumberToString 结果无具名槽、直落
        // callee-saved %ebx 跨 ctor 保活 ⇒ P0 实参内联 + P227 临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xa47e, 5)(
            "inSert into check_pickup_random_option_item failed at m_id(%s) ",
            NumberToString(m_id, 0));
        return 0;
    }
    return 1;
}