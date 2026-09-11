// b28d-CUser P292：ORIG 0x08697598 体 movzbl 直返（无 setne）⇒ bool。
bool CUser::is_update_ontime_last_recv_idx()
{
    return m_ceraUserInfo.m_updateOntime;   // §4.3 具名(UserInfo+0x68)
}