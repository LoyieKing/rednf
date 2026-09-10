bool CLuckPoint::UseLuckPoint(int itemType, int rarity, int grade, int seq)
{
    if (m_users.empty())
    {
        return false;                                   // [0x8550925 guard]
    }
    int idx = (int)_GetMaxLuckPointUserIndex();
    if (idx < 0 || m_users.size() <= (unsigned int)idx) // [js / ja 两段]
    {
        return false;
    }
    CUser* user = m_users[idx];
    if (user == 0)
    {
        return false;                                   // [0x8550b87]
    }
    if (!UseLuckPoint(user, itemType, rarity, grade))   // [0x8550baf xor $1]
    {
        return false;
    }
    m_points[seq] = idx;                                // [0x8550bc2 operator[](p4)]
    return true;
