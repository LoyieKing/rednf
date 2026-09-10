unsigned char* CActionPointManager::GetActionPoint() const
{
    return reinterpret_cast<unsigned char*>(
        const_cast<_SIG_LOAD_ACTION_POINT*>(&m_sig));
