bool DB_UpdateExpertJobStatistic::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int count = 0;
    *stream >> count;
    if (count == 0)
        return 1;

    for (unsigned int i = 0; i < count; ++i)
    {
        int expertJobType = 0;
        unsigned long itemCnt = 0;
        unsigned int itemId = 0;
        *stream >> expertJobType;
        *stream >> itemCnt;
        *stream >> itemId;
        if (itemCnt == 0)
            break;
        db->set_query(
            "upDate expert_job_product_cnt set item_cnt=item_cnt+%d where expert_job_type=%d and occ_date=curdate() and item_id=%d",
            itemId, expertJobType, itemCnt);
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        {
            db->set_query(
                "inSert into expert_job_product_cnt(expert_job_type,occ_date,item_id,item_cnt) values(%d,curdate(),%d,%d)",
                expertJobType, itemCnt, itemId);
            db->exec(true);
        }
    }
    return 1;
