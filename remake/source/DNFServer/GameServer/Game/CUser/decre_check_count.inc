void CUser::decre_check_count()
{
    *(short*)((char*)&m_connectCount) = 0;
}