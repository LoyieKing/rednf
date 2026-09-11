void CMissionList_Charac::MakeMissionList_forNewbies(const CUser& user)
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        // [b28d-solo-CMissionList_Charac] ORIG 调用 `_ZNKSt4listIP8CMissionSaIS1_EE5emptyEv`
        // /`beginEv`/`endEv` 全为 **const 版**且无 `_List_iterator`→`_List_const_iterator`
        // 转换构造 call ⇒ 原码 list 声明为 `const std::list<CMission*>*`。
        const std::list<CMission*>* list =
            (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
        if (list->empty())
        {
            continue;
        }
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e4d50..0x085e4d8d：MissionInfo
        // info(-0x1c) 构造 → list->begin()(-0x20) → const_iterator::operator* →
        // `cmp $2; sete; test; je 循环` ⇒ 原码用 begin()/`*it` 而非 front()
        // （front() 少一次 begin sret 调用与迭代器槽）。增量段 ORIG 为 `ppEv`
        // ⇒ `++it` 前置（P28 反向）。修复后 → AE（93/93）。
        MissionInfo info;
        std::list<CMission*>::const_iterator it = list->begin();
        if ((*it)->GetMissionGrade() == 2)
        {
            continue;
        }
        for (; list->end() != it; ++it)
        {
            if ((*it)->isAcceptCondition(user))
            {
                (*it)->makeMissionInfo(info, user);
                addNewMission(info);
                alter();
                break;
            }
        }
    }
}