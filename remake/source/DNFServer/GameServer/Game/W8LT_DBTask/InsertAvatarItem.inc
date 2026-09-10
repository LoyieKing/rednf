bool DB_AvatarItemInsert::InsertAvatarItem(SIG_AVATAR_INSERT* sig)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    // [SweepIntervalSet] 旧判词「N78：4.1 内联 memcpy / 4.4 恒 call ⇒ 封顶」被推翻。
    // ORIG 0x083ffe54..083ffec0 五段 = memset(0xc000) → rep movsl 0x33 dwords+尾 word
    // → memset(0x5f32) → rep movsl 0x37 dwords+尾 byte → memset(0x5f23)，
    // 正是 4.4 对「双数组成员结构体聚合初始化」的展开（探针 /tmp/pbig2.o::t23 逐条同形）；
    // 成员声明序 = 槽址升序（m_expireDate 深槽、m_fromUnixtime +0x6000）。
    struct SqlTemplateBuf
    {
        char m_expireDate[0x6000];
        char m_fromUnixtime[0x6000];
    };
    SqlTemplateBuf sql = {
        "inSert into user_items (charac_no,slot,it_id,expire_date,obtain_from,reg_date,ability_no,ipg_agency_no,stat,jewel_socket,hidden_option,emblem_endurance) values(%u,%d,%u,'%s',%d,now(),%d,'%s',%d,'%s',%d,%d)",
        "inSert into user_items (charac_no,slot,it_id,expire_date,obtain_from,reg_date,ability_no,ipg_agency_no,stat,jewel_socket,hidden_option,emblem_endurance) values(%u,%d,%u,from_unixtime('%s'),%d,now(),%d,'%s',%d,'%s',%d,%d)"
    };
    char* q = 0;
    char date[0x40] = "9999-12-31 23:59:59";
    if (((DB_AvatarInsertView*)sig)->m_period == 0) {
        q = sql.m_expireDate;
    } else {
        q = sql.m_fromUnixtime;
        memset(date, 0, 0x40);
        cMyTrace(__PRETTY_FUNCTION__,
                 0xae1, 0)("[InsertAvatarItem] pContext->nExpireTime : %d",
                           ((DB_AvatarInsertView*)sig)->m_period);
        if (((DB_AvatarInsertView*)sig)->m_period > 6 &&
            ((DB_AvatarInsertView*)sig)->m_period <= 0x1e) {
            sprintf(date, "%d",
                    GlobalData::s_systemTime_.getCurSec() +
                        ((DB_AvatarInsertView*)sig)->m_period * 0x15180);
            cMyTrace(__PRETTY_FUNCTION__,
                     0xae5, 0)("[InsertAvatarItem] period value : %s", date);
        } else {
            sprintf(date, "%d", ((DB_AvatarInsertView*)sig)->m_period);
            cMyTrace(__PRETTY_FUNCTION__,
                     0xaea, 0)("[InsertAvatarItem] period value : %s", date);
        }
    }
    char* jewel = db->blob_to_str(0, ((DB_AvatarInsertView*)sig)->m_jewel, 0x1e);
    db->set_query(q, ((DB_AvatarInsertView*)sig)->m_characNo,
                  ((DB_AvatarInsertView*)sig)->m_slot,
                  ((DB_AvatarInsertView*)sig)->m_itId, date,
                  ((DB_AvatarInsertView*)sig)->m_obtainFrom,
                  ((DB_AvatarInsertView*)sig)->m_abilityNo,
                  ((DB_AvatarInsertView*)sig)->m_agency,
                  ((DB_AvatarInsertView*)sig)->m_stat, jewel,
                  ((DB_AvatarInsertView*)sig)->m_hiddenOption,
                  ((DB_AvatarInsertView*)sig)->m_emblemEndurance,
                  ((DB_AvatarInsertView*)sig)->m_field4f,
                  ((DB_AvatarInsertView*)sig)->m_field51);
    bool r = db->exec(true);
    if (!r)
        return 0;
    return 1;
