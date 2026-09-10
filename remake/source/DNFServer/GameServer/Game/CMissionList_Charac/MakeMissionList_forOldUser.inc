void CMissionList_Charac::MakeMissionList_forOldUser(const CUser& user)
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e4e88..0x085e4ef7 同 forNewbies：
        // info(-0x1c) 构造 → begin()(-0x20) → operator* → GetMissionKind/getIndex_byKind
        // → operator* → GetMissionGrade；`list` 亦为 const 指针（empty/begin/end const 版）。
        // 循环条件实参序 this=&it(-0x20)、arg=&end(-0x14) ⇒ `it != list->end()`（与
        // forNewbies 相反，P74 补充② 三件三形）。修复后 → AE（109/109）。
        const std::list<CMission*>* list =
            (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
        if (list->empty())
        {
            continue;
        }
        MissionInfo info;
        std::list<CMission*>::const_iterator it = list->begin();
        if (getIndex_byKind((*it)->GetMissionKind()) != 0)
        {
            continue;
        }
        if ((*it)->GetMissionGrade() == 2)
        {
            continue;
        }
        for (; it != list->end(); ++it)
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
