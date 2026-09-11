// ============================================================================
// DB_CountOfHumanCertify::dispatch —— ORIG 0x084282a0（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x84282a0：GetDBHandle(DB_HANDLE_6,0) → *stream >> m_id(uint) →
// set_query("upDate human_certify_try_count set count = count + 1 where
// m_id = %s", NumberToString(m_id,0)) → exec(true)。exec 失败或受影响行数
// 为 0 → inSert human_certify_try_count(m_id,count) values(%s,1)（m_id）；
// insert exec 失败 → cMyTrace(0x69cd,5) "Update and Insert into
// HumanCertifyCount failed" 返 0；成功返 1。
// ============================================================================
bool DB_CountOfHumanCertify::dispatch(int /*userIdx*/, int /*slot*/,
                                      Stream* stream)
{
    unsigned int m_id;                     // -0x20（ORIG 0x8424xxx 帧顶
                                           // 无 `movl $0,-0x20` 清零 ⇒
                                           // 声明无初始化）
    MySQL* db;                             // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                           E_SERVER_GROUP_NONE);
    *stream >> m_id;
    db->set_query(
        "upDate human_certify_try_count set count = count + 1 where m_id = %s",
        NumberToString(m_id, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into human_certify_try_count(m_id, count) values(%s, 1)",
            NumberToString(m_id, 0));
        if (db->exec(true) != 1)
        {
            // ORIG 0x0842839d(+0xfd)：trace 临时对象落 -0x1c、m_id 占远槽
            // -0x20（P37 临时形；4.4 具名对象反先分配致两槽互换）。
            cMyTrace(__PRETTY_FUNCTION__, 0x69cd, 5)(
                "Update and Insert into HumanCertifyCount failed");
            return 0;
        }
    }
    return 1;
}