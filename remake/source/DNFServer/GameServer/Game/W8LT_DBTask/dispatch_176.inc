bool DB_SaveRedeemItemList::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    static const int handleIdx = 3;
    DB_RedeemInfoView* sig;
    MySQL* db;
    sig = reinterpret_cast<DB_RedeemInfoView*>(
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
