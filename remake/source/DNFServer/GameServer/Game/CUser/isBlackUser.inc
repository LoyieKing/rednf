bool CUser::isBlackUser(unsigned int accId)
{
    if (m_vec8cef4.empty())
        return false;
    for (std::vector<unsigned int>::iterator it = m_vec8cef4.begin();
         it != m_vec8cef4.end(); ++it) {
        if (*it == accId)
            return true;
    }
    return false;
}