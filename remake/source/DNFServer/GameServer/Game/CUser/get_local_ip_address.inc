int CUser::get_local_ip_address()
{
    return *(int*)((char*)&m_localIp);
