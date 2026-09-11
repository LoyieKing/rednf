unsigned short CAssaulter::GetUserUniqueId()
{
    // ORIG T 0x82e6662：user ? user->get_unique_id() : 0
    if (m_user == 0)
        return 0;
    return (unsigned short)(m_user)->get_unique_id();
}