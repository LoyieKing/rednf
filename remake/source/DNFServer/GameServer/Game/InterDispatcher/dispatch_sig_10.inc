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
    Inter_LoadDungeonClearPktView* pPkt = reinterpret_cast<Inter_LoadDungeonClearPktView*>(data);
    if (pPkt->m_c == 1)
        return 0x2020;
    user->getDungeonClearRefW()->fromString(user, reinterpret_cast<char*>(pPkt) + 0x10);
    // ORIG +0x5a：`movzbl 0x8(%eax),%eax`+`xor $0x1,%eax`+`test %al,%al` ⇒ bool 字段取反。
    if (!pPkt->m_8)
        user->disableSaveDungeonClear();
    user->EnableCharacInfo((CACHE_SECTION)5);
    return 0;
}