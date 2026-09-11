// ===========================================================================
// OnEvadeAssaultPlace —— ORIG T 0x082efbf6 (0x5d)：catch 空吞，恒返 1。
// ===========================================================================
bool CAssaultMgr::OnEvadeAssaultPlace(CUser* user)   // 恒返 1；ORIG 调用点 xor $1 ⇒ bool
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->EvadeAssaultPlace(user);
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;
}