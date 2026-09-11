// ============================================================================
// Arad_GiftItem_Set::level_check（ORIG 0x081a31c4，_ZN17Arad_GiftItem_Set11level_checkEP5CUserPK14CStackableItem）
// 语义：
//   kind  = str_to_enum(item->m_arad148.m_str8.c_str())
//   job   = user->get_charac_job()
//   ulv   = item->getUsableLevel()
//   data  = find(this, kind, job, ulv)
//   data==0 -> 1；job 不可用（find 未命中）则报 "user level limit" 日志并返回 0；
//   user->get_charac_level() < ulv -> 报日志并返回 0；否则 1。
// NOTE：汇编中 get_charac_level 比较对象是 CUserCharacInfo 基类，故对 user 强转访问。
// ============================================================================
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
}