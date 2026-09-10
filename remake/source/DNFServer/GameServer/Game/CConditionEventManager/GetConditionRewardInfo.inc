void CConditionEventManager::GetConditionRewardInfo(
    char useQuickPartyAdd, std::vector<stConditionEventRewardInfo>& out,
    short curIdx, char step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)curIdx != field0)
        return;
    out.clear();
    std::multimap<int, stConditionEventRewardInfo>* mm;
    if (!useQuickPartyAdd)
        mm = &G_CDataManager()->m_conditionEvent.m_multiMap20;
    else
        mm = &G_CDataManager()->m_conditionEvent.m_multiMap38;
    std::multimap<int, stConditionEventRewardInfo>::iterator it =
        mm->lower_bound((int)step);
    std::multimap<int, stConditionEventRewardInfo>::iterator endIt =
        mm->upper_bound((int)step);
    // [b29-Lane21] ORIG 0x0833547b `mov -0x20(%ebp),%eax; mov %eax,-0x28(%ebp)` =
    //   lower_bound 结果（it@-0x20）的**具名副本**（cur@-0x28，for-init 处声明）⇒
    //   独立槽 + 后续槽位整体深 4；直接以 it 循环少一槽（残差 9 行）。
    // b28d-52：ORIG 0x083354de 增量段为后置 ppEi（P28 增量侧：三槽阶梯）+
    // 具名结构拷贝（0x0833548e 逐字段 +4/+8/+0xc 后 push_back(&info)）。
    for (std::multimap<int, stConditionEventRewardInfo>::iterator cur = it;
         cur != endIt; cur++)
    {
        stConditionEventRewardInfo info = cur->second;
        out.push_back(info);
    }
