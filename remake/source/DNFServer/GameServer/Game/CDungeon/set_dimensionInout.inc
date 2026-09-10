void CDataManager::set_dimensionInout(int idx, char value)
{
    if (idx > 5)
        return;
    if (idx < 0)
        return;
    m_dimensionInout[idx] = value;
