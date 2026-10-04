// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessSkillConfirm @0x08434340
// dbPremium=GetDBHandle(6,0) / dbSkill=GetDBHandle(3,0)；blob 写 skill 表
// （SKILL_CONFIRM_STATE 标志）→ cMyTrace(0x7d1a) → CDataManager+0xc
// GetRestrictCode(0xc5) → member_premium service_end=now() 更新
// （event_id=restrictCode, pre_type=0x21, m_id=NumberToString(accId), server_id
// =serverGroup）→ cMyTrace(0x7d29) → 1。
// ============================================================================
bool DB_UpdateOneDayLetheSkill::ProcessSkillConfirm(
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig, int /*slot*/)
{
    MySQL* dbPremium =
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                          (ENUM_SERVER_GROUP)0);
    MySQL* dbSkill =
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                          (ENUM_SERVER_GROUP)0);
    char* blob = 0;
    if (!put_compressed_blob_data(
            dbSkill, 0, sig->m_skills, 0x198, &blob))
    {
        return false;
    }
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        dbSkill->set_query("upDate %s set skill_slot_lethe='%s', lethe_flag=%u  where charac_no=%u",
                           get_skill_table_name(false), blob,
                           (int)CPremiumLetheManager::SKILL_CONFIRM_STATE,
                           sig->m_characNo);
    }
    else
    {
        dbSkill->set_query("upDate %s set skill_slot_lethe_2nd='%s', lethe_flag_2nd=%u  where charac_no=%u",
                           get_skill_table_name(false), blob,
                           (int)CPremiumLetheManager::SKILL_CONFIRM_STATE,
                           sig->m_characNo);
    }
    if (!dbSkill->exec(true))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x7d1a, 0)(
        "ONE_DAY_LETHE : DB_CONFIRM_SKILL-FLAG_WRITE");
    int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(0xc5);
    int preType = 0x21;
    // ORIG +0x?? `mov -0x20(%ebp),%eax` 迟装载紧贴 NumberToString arg1 spill
    // （unsigned 槽无强转 ⇒ 不提前物化，ORIG 0x08434340 体 +0xa8 段）。
    unsigned int accId = sig->m_accID;
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    dbPremium->set_query("upDate member_premium set service_end=now() where event_id=%u and pre_type=%u and m_id=%s and server_id in(0,%u) and service_end>now()",
                         restrictCode, preType,
                         NumberToString(accId, 0),
                         serverGroup);
    if (!dbPremium->exec(true))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x7d29, 0)(
        "ONE_DAY_LETHE : DB_CONFIRM_SKILL-PREMIUM_END_CHANGE");
    return true;
}