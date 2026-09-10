void CMissionList_Charac::Update_Within_Mission_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    std::vector<int> out;
    check_mission_kind(0x1b, user, param, out);
    if (!out.empty())
    {
        outClear = true;
    }
    Send_Cleared_Mission_List(user, out);
