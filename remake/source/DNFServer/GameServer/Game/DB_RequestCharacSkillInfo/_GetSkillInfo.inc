// ORIG 0x08447312：seLect %s from skill（DB_HANDLE_3，列名=get_skill_slot_name(slot)）；
// exec/fetch 失败或 count==0 → logFormat 后返回 0；get_compressed_blob_data(0, 0x198, 2)
// 解压已学技能表。
// [b28d-19] N：P75 —— 实参装载时刻（ORIG 在 i++ 前先载 len→%ecx，4.4 末位装载；cast/逗号/具名三形同产）；ORIG 0x08447312。
bool DB_RequestCharacSkillInfo::_GetSkillInfo(unsigned int characNo, char slot,
                                              _Mastered_skill* skills)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    db->set_query("seLect %s from skill where charac_no=%d", get_skill_slot_name(slot), characNo);
    bool ok = db->exec(true);
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1c5,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo exec() failed, charac_no(%d)", characNo);
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1ce,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo fetch() failed, charac_no(%d)", characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1d5,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo no result, charac_no(%d)", characNo);
        return 0;
    }
    int i = 0;
    int len = 0x198;
    // b28 判词（P75 在册 0x08447312）：ORIG 4.1 在 i++ 前先载 len(-0xc)→%ecx 跨
    // 副作用保活；4.4.7 恒在最后装载位实装——实参装载时刻跨版本调度差，
    // cast/逗号/具名三形同产。残差 1 insn（装载位置）封顶。
    if (!get_compressed_blob_data(db, i++, (char*)skills, len, 2))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1e0,
                              "DB_RequestCharacSkillInfo::_GetSkillInfo get_compressed_blob_data failed, charac_no(%d)", characNo);
        return 0;
    }
    return 1;
}