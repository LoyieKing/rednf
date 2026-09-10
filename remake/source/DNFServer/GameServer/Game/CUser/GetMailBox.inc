void* CUser::GetMailBox()
{
    return *(void**)((char*)&m_premium.m_mailBox);
