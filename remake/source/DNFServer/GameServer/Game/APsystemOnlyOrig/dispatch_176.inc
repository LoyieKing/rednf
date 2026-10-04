// ============================================================================
// DB_SaveRedeemItemList::dispatch —— ORIG 0x0840e074（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_REDEEMINFO_LIST_TO_DB>（+0x00 charac_no；压缩源全 0x59c）
// → buf[0x6c7] 清零 → outLen=0x6c7 → compress_zip(buf,&outLen,sig,0x59c)
// 失败返 0 → GetDBHandle(DB_HANDLE_3,0) → blob_to_str(0,buf,outLen) →
// set_query("upDate charac_inven_expand set redeem_info='%s' where
// charac_no=%u", blobStr, charac_no)；exec 失败返 0；
// getAffectedRowCount()==0 → blob_to_str 再编 → set_query("inSert into
// charac_inven_expand(charac_no, redeem_info) values(%u, '%s')",
// charac_no, blobStr)；exec 失败返 0；成功返 1。
// ============================================================================
bool DB_SaveRedeemItemList::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    static const int handleIdx = 3;
    SIG_REDEEMINFO_LIST_TO_DB* sig;
    MySQL* db;
    sig = (
        stream->GetOutBuffer<SIG_REDEEMINFO_LIST_TO_DB>());
    char buf[0x6c7] = { '\0' };
    unsigned int outLen = 0x6c7;
    if (compress_zip(buf, outLen, (const char*)sig, 0x59c) != 1)
        return 0;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_inven_expand set redeem_info='%s' where charac_no=%u",
        db->blob_to_str(0, buf, (int)outLen), sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_inven_expand(charac_no, redeem_info) values(%u, '%s')",
            sig->m_characNo, db->blob_to_str(0, buf, (int)outLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}