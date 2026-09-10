void CMailBox::GetPackageLoadLack(unsigned int* out, unsigned int n)
{
    for (unsigned int i = 0; i < n && i < m_loadLackList.size(); ++i)
        out[i] = m_loadLackList[i];
