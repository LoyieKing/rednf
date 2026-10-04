// [b28d-4 P0 清绑] 原 `register char* mIdStr asm("ebx")` 已删（N96 全域禁绑定）；
// 具名槽化后 mIdStr 槽位 -0x10→-0x14、多 1 条 load（ORIG 0x0840888f 区 %ebx
// 免槽形）⇒ N96+N18a 封顶，语义等价。
bool DB_UpdateCharacView::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_UPDATE_CHARAC_VIEW* sig =
        stream->GetOutBuffer<SIG_UPDATE_CHARAC_VIEW>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    char buf[0x1905] = { 0 };
    unsigned int zipLen = 0x1905;
    // 技巧36：删 hv 具名视图指针（ORIG 每站点 `mov -0x10(%ebp),%eax` 参数
    // 结果槽直读+add disp，0x0840888f 区；hv 槽使整帧外推 8B）。
    if (!compress_zip(buf, zipLen,
                      sig->m_info,
                      0x14d0))
        return 0;
    // ORIG 0x08408…：`mov %eax,%ebx` mIdStr 直落 callee-saved %ebx（无槽）。
    // [b28d-4 P0 清绑] 原 `register char* mIdStr asm("ebx")` 已删（N96 全域禁绑定）。
    // [J4S1 P329/P227] 删具名 mIdStr（ORIG 无槽）⇒ 表达式内联回实参位，4.4 走
    // callee-saved %ebx 保活（同函数第二块同形已实证）。
    db->set_query("upDate charac_view set info='%s' where m_id=%s",
                  db->blob_to_str(0, buf, (int)zipLen),
                  NumberToString(
                      sig->m_mId, 0));
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into charac_view(m_id,info) values(%s,'%s')",
                      NumberToString(sig->m_mId, 0),
                      db->blob_to_str(0, buf, (int)zipLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}