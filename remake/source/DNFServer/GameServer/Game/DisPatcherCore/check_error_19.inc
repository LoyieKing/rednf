unsigned int DisPatcher_FinishLoad::check_error(CUser* user, MSG_BASE& /*msg*/)

{
    if (user->GetDebugCommand() == 1)
    {
        return 0x7fffffff;
    }
    return 0;
}
