void CLevelUp::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    DisPatcher_DebugCommand dispatcher;                       // local_14
    dispatcher._debugCommandSetLevel(user, getLevel());
