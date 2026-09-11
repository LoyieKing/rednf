// ============================================================================
// findMissionValue @ 0x08168010
//   bool findMissionValue(CUser*, MissionNo::T, HeroMissionValue*&)
//   判活 → mission ∈ [1,6] → m_values[charac_no] 的 vector 中匹配 mission。
// ============================================================================
bool HeroMissionEvent::findMissionValue(CUser* user,
                                        HeroMissionCondition::MissionNo::T mission,
                                        HeroMissionValue*& out)
{
    // ORIG 0x0816802e `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (!IsEventing(0))
        return false;
    if (mission <= 0 || mission >= 7)
        return false;

    unsigned int characNo =
        static_cast<unsigned int>(user->get_charac_no(-1));
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
        return false;

    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        // [b28d-solo-HeroMissionEvent] ORIG 0x081680e9 `mov (%eax),%eax;
        // cmp 0x10(%ebp),%eax`：去掉 `static_cast<int>(mission)` 后 4.4 直接
        // 内存源比较，省去 mov+mov 两段（N18b 的翻正形）。
        if (vit->m_missionType == mission)
        {
            out = &*vit;
            return true;
        }
    }
    return false;
}