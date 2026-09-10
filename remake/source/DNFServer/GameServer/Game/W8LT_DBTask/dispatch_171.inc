bool DB_SaveObjectBringUp::dispatch(int /*userIdx*/, int /*slot*/,
                                    Stream* stream)
{
    // 执行序照 ORIG：GetDBHandle → GetOutBuffer → get_db_table_name 链构造
    // std::string（链式内联，无 sm 局部槽）。表名每次 set_query 前 toTString
    // 即时求值（ORIG 无 CSE）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_OBJECT_BRING_UP* sig = stream->GetOutBuffer<SIG_OBJECT_BRING_UP>();
    std::string tableStr(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x19b));
    // ORIG 0x08446a1e 段 `movzbl 0xd(%eax),%eax; test %al,%al; je <flag==0 块>`
    // ⇒ flag!=0 块直落、flag==0 块在后（4.4 then 恒前置）⇒ 交换块并取反
    // （同 DB_GoblinPadUpdateReward / DB_UpdateQuest 的 P222 正向包裹形）。
    if (sig->m_rewardFlag != 0)
    {
        db->set_query(
            "upDate %s set use_material_cnt=use_material_cnt+%u, give_reward_flag=1, occ_date = now() where charac_no=%u and m_id=%s",
            toTString(tableStr), (unsigned int)sig->m_usedMaterialCnt,
            sig->m_characNo, NumberToString(sig->m_accId, 0));
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into %s(charac_no,m_id,use_material_cnt,give_reward_flag,occ_date) values(%u,%s,%u,%u,now())",
                toTString(tableStr), sig->m_characNo,
                NumberToString(sig->m_accId, 0),
                (unsigned int)sig->m_usedMaterialCnt,
                (unsigned int)sig->m_rewardFlag);
            if (db->exec(true) != 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xaebd, 5);
                trace("upDate DB_SaveObjectBringUp::dispatch failed at charac_no(%u),count(%u),flag(%u)",
                      sig->m_characNo, (unsigned int)sig->m_usedMaterialCnt,
                      (unsigned int)sig->m_rewardFlag);
                return 0;
            }
        }
    }
    else
    {
        db->set_query(
            "upDate %s set use_material_cnt=use_material_cnt+%u where charac_no=%u and m_id=%s",
            toTString(tableStr), (unsigned int)sig->m_usedMaterialCnt,
            sig->m_characNo, NumberToString(sig->m_accId, 0));
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into %s(charac_no,m_id,use_material_cnt) values(%u,%s,%u)",
                toTString(tableStr), sig->m_characNo,
                NumberToString(sig->m_accId, 0),
                (unsigned int)sig->m_usedMaterialCnt);
            if (db->exec(true) != 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xaece, 5);
                trace("upDate DB_SaveObjectBringUp::dispatch failed at charac_no(%u),count(%u)",
                      sig->m_characNo, (unsigned int)sig->m_usedMaterialCnt);
                return 0;
            }
        }
    }
    return 1;
