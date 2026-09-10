bool DB_SaveEventItem::_HardCodeEventItems(SIG_SAVE_EVENTITEM* sig)
{
    // [b28-L11 fresh] 旧「je 目标块首对齐 nop 错位 ndiff 24」判词已由 nop 对齐
    // 剥离口径（§6.1）消解，fresh ndiff=0（AE 归一后逐条全等）。
    static const int handleIdx = 3;
    if (sig->m_field0c != 0x65ae)
    {
        return false;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect id from charac_event_items where charac_no=%d and it_id=%d",
                  sig->m_field04, sig->m_field0c);
    int id = 0;
    if ((db->exec(true) && db->get_n_rows() != 0 && db->fetch() &&
         db->get_int(0, id) && id != 0) ? false : true)
    {
        db->set_query("inSert into charac_event_items(charac_no,it_id,stack_count,event_code,reg_time,delete_time,delete_flag) values(%d,%d,%d,%d,now(),now(),1)",
                      sig->m_field04, sig->m_field0c, sig->m_field10, sig->m_field08);
        if (!db->exec(true))
        {
            return 1;
        }
    }
    else
    {
        db->set_query("upDate charac_event_items set stack_count=stack_count+1 where id=%d",
                      id);
        if (!db->exec(true))
        {
            return 1;
        }
    }
    return 1;
