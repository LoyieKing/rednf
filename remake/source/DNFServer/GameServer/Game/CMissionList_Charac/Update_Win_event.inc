void CMissionList_Charac::Update_Win_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    Update_Within_Mission_event(user, param, outClear);
    alter();
    if (outClear == true)
        return;
    if (getIndex_byKind(0x1b) != 0)
        return;
    std::vector<int> out;
    check_mission_kind(2, user, param, out);
    check_mission_kind(4, user, param, out);
    check_mission_kind(9, user, param, out);
    check_mission_kind(10, user, param, out);
    check_mission_kind(0xc, user, param, out);
    check_mission_kind(0xe, user, param, out);
    check_mission_kind(0x11, user, param, out);
    check_mission_kind(0x13, user, param, out);
    check_mission_kind(0x14, user, param, out);
    check_mission_kind(0x16, user, param, out);
    check_mission_kind(0x18, user, param, out);
    check_mission_kind(0x1a, user, param, out);
    Send_Cleared_Mission_List(user, out);
}