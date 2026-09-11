int ARAD::Arad_DB_Charac_Dispatcher::dispatch(int param1, int param2, Stream* stream)
{
    (void)param1;
    (void)param2;
    SIG_CREATE_CHARAC* sig = stream->GetOutBuffer<SIG_CREATE_CHARAC>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x18, "%s", "Context Null");
        return -1;
    }

    if (create_charac(sig) != 0)
    {
        return -1;
    }
    if (create_charac_stat(sig) != 0)
    {
        return -1;
    }
    if (create_inventory(sig) != 0)
    {
        return -1;
    }
    if (create_equip(sig) != 0)
    {
        return -1;
    }
    if (create_skill(sig) != 0)
    {
        return -1;
    }
    if (create_pvp(sig) != 0)
    {
        return -1;
    }
    if (create_charac_dungeon(sig) != 0)
    {
        return -1;
    }
    if (create_charac_quest(sig) != 0)
    {
        return -1;
    }
    return 0;
}