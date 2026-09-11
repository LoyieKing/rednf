// ===========================================================================
// OnAbuseUser —— ORIG T 0x082efc54 (0x5d)：catch 空吞，恒返 1。
// ===========================================================================
bool CAssaultMgr::OnAbuseUser(CUser* user)   // 恒返 1；ORIG 调用点 xor $1 ⇒ bool（CAssaultMgr.h:472）
{
    try
    {
        CAssaultPlace* place =
            _GetAssaultPlace(static_cast<unsigned int>(user->GetAssaultPlace()));
        place->AbuseUser(user);
    }
    catch (ENUM_ERROR)
    {
        // ORIG catch 仅 __cxa_begin/end_catch 吞异常。
    }
    return 1;
}