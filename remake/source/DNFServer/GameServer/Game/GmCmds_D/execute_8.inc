// ORIG execute 0x084b38e2：栈构 DisPatcher_DebugCommand 后委托设级。
void CLevelUp::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    DisPatcher_DebugCommand dispatcher;                       // local_14
    dispatcher._debugCommandSetLevel(user, getLevel());
}