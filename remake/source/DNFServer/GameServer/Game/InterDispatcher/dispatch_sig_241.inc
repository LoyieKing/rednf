int Inter_LoadOnlinePreliminaryTeamMatchList::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    GlobalData::s_onlinePreliminaryTeamMgr->OnLoadTeamInfo(user, data);
    return 0;
