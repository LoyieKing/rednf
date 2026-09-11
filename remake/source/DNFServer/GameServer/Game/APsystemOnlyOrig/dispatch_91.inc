// ============================================================================
// DB_SavePrivateStore::dispatch —— ORIG 0x0841bf02（报告 dispatch.md 全文）
// ============================================================================
// 无基类前置。GetOutBuffer<private_store::SIG_PRIVATE_STORE> → GetDBHandle(3,0)
// → upDate store set store='%s', use_doll='%d' where charac_no=%d
//   （%s=blob_to_str(0, m_items, 0x428)、use_doll=m_dollCreated、charac_no）；
// exec 成功且 getAffectedRowCount()!=0 → 返 1；否则补 inSert
//   values(%d, '%s', '%d') = m_characNo, blob, m_dollCreated；
// inSert exec 失败 cMyTrace(0x4f6d,5) 返 0，成功返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841bf02
bool DB_SavePrivateStore::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    // 声明序照 ORIG 栈槽（低→高）：sig(-0x14) db(-0xc)。
    private_store::SIG_PRIVATE_STORE* sig =
        stream->GetOutBuffer<private_store::SIG_PRIVATE_STORE>();
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate store set store = '%s', use_doll = '%d' where charac_no =%d",
        db->blob_to_str(0, sig->m_items, 0x428),
        // ORIG 0x0841bf40：`mov 0x430(%eax),%eax; mov %eax,%ebx` 两段装载 ⇒
        // 实参处 (unsigned int) cast（探针 g4；P265 变体）。
        (unsigned int)sig->m_dollCreated,
        sig->m_characNo);
    // ORIG 0x0841bf9f：bVar2 无槽——`xor $1; test; jne` + `or %edx,%eax; test;
    // jne` + `mov $1/$0,%eax` 双常数岛，`test %al; je` 跳尾 ⇒ P226/P316 语句
    // 表达式零槽短路物化。
    if (({ db->exec(true) != 1 || db->getAffectedRowCount() == 0; }))
    {
        db->set_query(
            "inSert into store(charac_no, store, use_doll) values(%d, '%s', '%d')",
            sig->m_characNo,
            db->blob_to_str(0, sig->m_items, 0x428),
            (unsigned int)sig->m_dollCreated);
        if (db->exec(true) != 1)
        {
            // ORIG 0x0841c039：m_characNo 在 ctor 前落 %ebx 跨 ctor 保活
            // ⇒ P227 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x4f6d, 5)(
                "inSert store failed at %d", sig->m_characNo);
            return 0;
        }
    }
    return 1;
}