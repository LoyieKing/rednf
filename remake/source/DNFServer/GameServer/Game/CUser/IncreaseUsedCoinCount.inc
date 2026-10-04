void CUser::IncreaseUsedCoinCount()
{
    *(int*)((char*)&m_usedCoinCount) = *(int*)((char*)&m_usedCoinCount) + 1;
}