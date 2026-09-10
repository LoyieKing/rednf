void CMissionList_Charac::Update_Kill_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    alter();
    std::vector<int> out;
    check_mission_kind(5, user, param, out);
    check_mission_kind(0xf, user, param, out);
    check_mission_kind(0x19, user, param, out);
    Send_Cleared_Mission_List(user, out);
