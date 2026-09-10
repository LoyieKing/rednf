void CGameManager::check_user_var(CUser* user)
{
    // ORIG：直接 && 条件（无中间 bool 变量）。
    if (user->get_charac_guildkey() != 0 && user->get_guild_exp() != 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x92c,
                              "[CHECK_USER_VAR] RESET GUILD EXP ERROR!");
    }
    if (user->get_charac_level_before_dungeon() != 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x92f,
                              "[CHECK_USER_VAR] RESET CHARACTER LEVEL BEFORE DUNGEON ERROR!");
    }
    if (user->CheckInParty())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x932,
                              "[CHECK_USER_VAR] RESET PARTY ERROR!");
    }
    if (user->CheckInTrade())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x935,
                              "[CHECK_USER_VAR] RESET TRADE INDEX ERROR!");
    }
    if (user->CheckInPvp())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x938,
                              "[CHECK_USER_VAR] RESET PVP INDEX ERROR!");
    }
    if (user->CheckInWarRoom())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x93b,
                              "[CHECK_USER_VAR] RESET WARROOM INDEX ERROR!");
    }
    if (user->getPlayingDungeonIndex() != -1)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x93e,
                              "[CHECK_USER_VAR] RESET DUNGEON ERROR!");
    }
    if (user->IsProgLogout())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x941,
                              "[CHECK_USER_VAR] RESET PROG LOGOUT ERROR!");
    }
    if (user->m_characterView.isSaveCharacView())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x944,
                              "[CHECK_USER_VAR] RESET CHARACTER VIEW ERROR!");
    }
    if (user->checkInBlueMarble())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x948,
                              "[CHECK_USER_VAR] RESET BLUEMARBLE INDEX ERROR!");
    }
