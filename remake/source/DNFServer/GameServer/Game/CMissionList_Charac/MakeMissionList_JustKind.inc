bool CMissionList_Charac::MakeMissionList_JustKind(const CUser& user,
                                                   unsigned int kind)
{
    bool result = false;
    // [b28d-solo-CMissionList_Charac] ORIG 0x085e4fc1..0x085e50f1：`movb $0,-0xd`
    // 预清 → empty 为真 `mov $0,%eax;jmp` 直返常量（非 movzbl result）⇒ 原码
    // `if (list->empty()) return false;`；getIndex!=0 同样直返常量 0；
    // 命中分支 `movb $1,-0xd; jmp 返回点` ⇒ `result = true; break;` 而非 `return true;`。
    // list 为 const 指针（empty/begin/end 全 const 版），it 用 begin()+operator*。
    // 修复后 → AE（96/96）。
    const std::list<CMission*>* list =
        (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
    if (list->empty())
        return false;
    MissionInfo info;
    std::list<CMission*>::const_iterator it = list->begin();
    if (getIndex_byKind((*it)->GetMissionKind()) != 0)
        return false;
    for (; it != list->end(); ++it)
    {
        if ((*it)->isAcceptCondition(user))
        {
            (*it)->makeMissionInfo(info, user);
            addNewMission(info);
            alter();
            result = true;
            break;
        }
    }
    return result;
}