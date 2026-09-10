bool DB_LoadCharac::_LoadEventBringUpInfo(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    std::string tableName(
        ((EventClassify::CEventScriptMng*)G_CDataManager()
             ->get_event_script_mng())
            ->get_db_table_name(0x19b));
    db->set_query(
        "seLect use_material_cnt, give_reward_flag from %s where charac_no=%u "
        "and m_id=%s",
        toTString(tableName), sig->m_characNo,
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        // [b28d-solo] P227：ORIG 0x08409de3 先把 sig->m_characNo 装 %ebx 跨
        // cMyTrace ctor 保活 ⇒ 全表达式临时链（具名 trace 局部为 N80 形）。
        cMyTrace(__PRETTY_FUNCTION__, 0x225b, 5)(
            "DB_LoadCharac::_LoadEventBringUpInfo, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    // [b28d-solo] P0：ORIG 0x08409e38 `mov %eax,-0x10(%ebp); cmpl $0x0,-0x10`
    // ⇒ 具名 int 局部。
    int nRows = db->get_n_rows();
    if (nRows == 0)
        return true;

    unsigned char useMaterialCnt = 0;
    unsigned char giveRewardFlag = 0;
    // [b28d-solo] P0：ORIG 0x08409e5e `movzbl %al,%eax; mov %eax,-0xc`（bool→uint
    // 截断槽往返）⇒ 具名 unsigned int 局部，两次 get_ubyte 复用同一槽。
    unsigned int ret = db->fetch();
    if (ret == 0)
        return false;
    ret = db->get_ubyte(0, useMaterialCnt);
    if (ret == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x226b, 5)(
            "DB_LoadCharac::_LoadEventBringUpInfo, get_ubyte(1) ERROR "
            "charac_no=%u", sig->m_characNo);
        return false;
    }
    ret = db->get_ubyte(1, giveRewardFlag);
    if (ret == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2272, 5)(
            "DB_LoadCharac::_LoadEventBringUpInfo, get_ubyte(2) ERROR "
            "charac_no=%u", sig->m_characNo);
        return false;
    }
    sig->m_characData.m_unkB2.m_useMaterialCnt = useMaterialCnt;
    // [b28d-solo] ORIG 0x08409f62 `test %al,%al; setne %dl; mov %dl,0x138`
    // ⇒ 直接布尔比较赋值（三目 ?1:0 产 je/mov$1/mov$0 分支物化）。
    sig->m_characData.m_unkB2.m_giveRewardFlag = (giveRewardFlag != 0);
    return true;
