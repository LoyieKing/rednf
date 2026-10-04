int Inter_LoadDungeonClear::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x200e;
    if (user->get_state() == 0)
        return 0x2012;
    if (user->IsProgLogout())
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2018,
            "[Inter_LoadDungeonClear::dispatch_sig] State(PROG_LOGOUT)\n");
        return 0;
    }
    // ORIG：包基址槽在 PROG_LOGOUT 门之后才物化。
    SIG_LOAD_DUNGEON_CLEAR* load = reinterpret_cast<SIG_LOAD_DUNGEON_CLEAR*>(data);
    if (load->m_selected == 1)
        return 0x2020;
    user->getDungeonClearRefW()->fromString(user, load->m_clearData);
    // ORIG +0x5a：`movzbl 0x8(%eax),%eax`+`xor $0x1,%eax`+`test %al,%al` ⇒ bool 字段取反。
    if (!load->m_pad8[0])
        user->disableSaveDungeonClear();
    user->EnableCharacInfo((CACHE_SECTION)5);
    return 0;
}