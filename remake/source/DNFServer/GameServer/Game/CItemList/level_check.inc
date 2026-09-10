bool Arad_GiftItem_Set::level_check(CUser* user, const CStackableItem* item)
{
    // ORIG 0x081a31c4：无空指针防御（P140）；usableLevel 值需跨
    // get_charac_job call 存活 ⇒ 内联实参直落 %ebx（P52）；
    // characLevel 在 < 比较与 log 实参各独立重调一次（P48）。
    int kind = str_to_enum(item->m_arad148.m_str8.c_str());
    Data* idx = find(kind, user->get_charac_job(), item->getUsableLevel());
    if (idx == 0)
    {
        return 1;
    }
    if (user->get_charac_level() < item->getUsableLevel())
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x14b, "[ERROR] user level limit charac_no(%d)",
                              user->get_charac_level());
        return 0;
    }
    return 1;
