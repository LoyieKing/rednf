// [b28d-4] DIFF：S 档 gap=37（缺失转写/调用）+ N82 槽序；ORIG 0842a11e
bool DB_UpdateDeathTowerBestRecord::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG：GetOutBuffer<SIG> + GetDBHandle(2,0) + upDate/inSert 双查询。
    SIG_DEATH_TOWER_UPDATE_BEST_RECORD* sig =
        stream->GetOutBuffer<SIG_DEATH_TOWER_UPDATE_BEST_RECORD>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // [b28d-TakeW8LT 形复原] ORIG 0x0842a11e **无** memberCount/membersStr 具名局部：
    // gcc 对变参从右到左求值，ORIG 0x0842a158..a1c8 先倒序读 9 个 sig 字段
    // （m_stage@0x64→%edi、m_memberCount@0x6→-0x4c、m_towerIndex@0x4→-0x48、
    // m_accId@0x0→-0x44、m_memberCount→-0x40、m_playTime@0x68→-0x3c、
    // m_memberCount→-0x38、m_stage→%esi、m_memberCount→%ebx），再在实参 0xc 位
    // 置调 blob_to_str（0x0842a1de）⇒ 源码形 = blob_to_str 内联为 set_query 实参，
    // m_memberCount 逐点重取（ORIG movzwl 26 次 vs 旧形缓存后 6 次）。
    db->set_query(
        "upDate charac_tower_record set member_info_%d='%s',stage_%d=%d,play_time_%d=%d,"
        "occ_time_%d=now() where charac_no=%d and tower_index=%d and stage_%d<=%d",
        sig->m_memberCount,
        db->blob_to_str(0, sig->m_members, (unsigned int)sig->m_memberCount * 0x17u),
        sig->m_memberCount, sig->m_stage, sig->m_memberCount, sig->m_playTime,
        sig->m_memberCount, sig->m_accId, sig->m_towerIndex, sig->m_memberCount,
        sig->m_stage);
    // ORIG 0x0842a24e..a277：`exec==1 && affected!=0` 短路求值后物化到 %eax
    // （mov $0x1,%eax / mov $0x0,%eax 两分支，无 bool 局部栈槽；旧形用
    //  needInsert 局部 ⇒ 多发 movb/cmpb 栈槽往返）⇒ 源码形 = 单个取反表达式。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into charac_tower_record(charac_no,tower_index,member_info_%d,stage_%d,"
            "play_time_%d,occ_time_%d) values(%d,%d,'%s',%d,%d,now())",
            sig->m_memberCount, sig->m_memberCount, sig->m_memberCount,
            sig->m_memberCount, sig->m_accId, sig->m_towerIndex,
            db->blob_to_str(0, sig->m_members, (unsigned int)sig->m_memberCount * 0x17u),
            sig->m_stage, sig->m_playTime);
        db->exec(true);
    }
    return true;
}