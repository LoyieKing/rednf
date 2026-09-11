bool CMissionList_Charac::loadData(CUser* pUser, char* pData)
{
    SIG_REQUEST_PVP_MISSION_LIST* pSig;
    int kind;
    const std::list<CMission*>* list;
    _reset();
    m_user = pUser;
    // pData 实为 SIG_REQUEST_PVP_MISSION_LIST（0x14a 字节 DB/存档 blob，
    // _saveData 侧同一类型），定基类型后全部具名字段访问。
    // ORIG 0x085e47ee `mov %eax,-0x14(%ebp)` 亦保留一份本地副本。
    pSig = reinterpret_cast<SIG_REQUEST_PVP_MISSION_LIST*>(pData);

    for (kind = 0; kind < 36; ++kind)
    {
        // ORIG 0x085e481c..0x085e4866：movzwl blob+6/+4/+8/+0xa → this+7/+5/+9/+0xb
        // 即 blob m_missionList[kind]（+0x04 起、步长 8）的 index/kind/count/streak。
        m_missionList[kind].m_index = pSig->m_missionList[kind].m_index;
        m_missionList[kind].m_kind = pSig->m_missionList[kind].m_kind;
        m_missionList[kind].m_count = pSig->m_missionList[kind].m_count;
        m_missionList[kind].m_streak = pSig->m_missionList[kind].m_streak;
    }
    StrToBitset(pSig->m_cleared, m_clearedFlags);   // +0x124（32 字节位图）
    m_field148 = pSig->m_field144;                  // +0x144
    m_field14a = pSig->m_field146;                  // +0x146
    m_field14c = pSig->m_field148;                  // +0x148

    // [b28d-solo-CMissionList_Charac] ORIG 0x085e48e6 段 `call G_CDataManager;
    // mov %eax,(%esp); call get_kind_mission_list` 无 mgr 具名槽（P0 右值内联）；
    // `_ZNKSt4listIP8CMissionSaIS1_EE5beginEv/3endEv` 为 const 版 ⇒ const 指针。
    // 判词 N82：残差=kind/list/pSig 槽号互换（4.4 首次使用序 vs 4.1 声明序，
    // N98 不可翻）；fatigueTime 已按 ORIG 内联（原 GetDailyScheduleTime helper
    // 为发明符号，已删）。
    list = (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(6);
    // [b28d-solo-CMissionList_Charac] P74 补充②：ORIG 0x085e495a 实参序
    // `arg0=&it(-0x1c) arg1=&end(-0x18)` ⇒ 原码是 `it != list->end()`
    //（写 `end() != it` 会反装实参序，探针三形第三例）。
    for (std::list<CMission*>::const_iterator it = list->begin();
         it != list->end(); ++it)
    {
        // [b28d-solo-CMissionList_Charac] P171 照抄：ORIG 0x085e4906 循环体
        // 只发一次 `_ZNKSt20_List_const_iteratorIP8CMissionEdeEv`（operator*）
        // 后直接 `mov (%eax),%eax; call GetMissionIndex`，**无 `(*it)` 判空**
        // （p167 EXTRA +1 const deEv 实证）⇒ 删除原 `(*it) &&` 防御。
        if (isClearMission((*it)->GetMissionIndex()))
        {
            m_field14e = 1;
            break;
        }
    }

    if (isUserNewbie())
    {
        MakeMissionList_forNewbies(*pUser);
        Reset_DailyMission();
    }
    else
    {
        MakeMissionList_forOldUser(*pUser);
    }

    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               pUser->getCurCharacLastPlayTick(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        Reset_DailyMission();
    }
    return 1;
}