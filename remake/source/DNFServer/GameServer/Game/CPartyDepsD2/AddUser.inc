// ---------------------------------------------------------------------------
// ::CLuckPoint::AddUser —— ORIG T 0x08550932：user 非空时 push_back 到 m_users。
// ---------------------------------------------------------------------------
void CLuckPoint::AddUser(CUser* user)
{
    if (user != 0)
        m_users.push_back(user);
}