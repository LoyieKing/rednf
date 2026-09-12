// ============================================================================
// DB_SaveObjectBringUp::dispatch —— ORIG 0x08446a1e（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_OBJECT_BRING_UP>（+0x00
// m_accId、+0x04 m_characNo、+0x0c m_usedMaterialCnt、+0x0d m_rewardFlag）→
// G_CDataManager()->get_event_script_mng()->get_db_table_name(0x19b) 构造
// std::string → toTString 取表名。m_rewardFlag==0：upDate %s set
// use_material_cnt=use_material_cnt+%u where charac_no=%u and m_id=%s；
// 失败/affected==0 补 inSert into %s(charac_no,m_id,use_material_cnt)
// values(%u,%s,%u)；insert 失败 cMyTrace(0xaece,5)(...count(%u)) 返 0。
// m_rewardFlag!=0：upDate 追加 give_reward_flag=1, occ_date = now()；
// affected==0 补 inSert ...(charac_no,m_id,use_material_cnt,
// give_reward_flag,occ_date) values(%u,%s,%u,%u,now())；insert 失败
// cMyTrace(0xaebd,5)(...count(%u),flag(%u)) 返 0。全绿返 1。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08446a1e
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
                // [R14a16 2026-09-12 · P227] ORIG 0x08446baa..0x08446c01 在 cMyTrace ctor
                // 之前右起求 varargs（%edi/%esi/%ebx 跨 ctor 保活）⇒ 全表达式临时链形
                // （两语句形会把 ctor 提到求值之前）。
                cMyTrace(__PRETTY_FUNCTION__, 0xaebd, 5)(
                    "upDate DB_SaveObjectBringUp::dispatch failed at charac_no(%u),count(%u),flag(%u)",
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
                // [R14a16 2026-09-12 · P227] 同上（ORIG 0x08446d2x 段同形）。
                cMyTrace(__PRETTY_FUNCTION__, 0xaece, 5)(
                    "upDate DB_SaveObjectBringUp::dispatch failed at charac_no(%u),count(%u)",
                    sig->m_characNo, (unsigned int)sig->m_usedMaterialCnt);
                return 0;
            }
        }
    }
    return 1;
}