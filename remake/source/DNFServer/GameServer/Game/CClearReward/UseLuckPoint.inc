// ORIG T 0x08550b14：最大幸运点用户执行扣点，成功记 m_points[seq]=序号。
// [b28c-lane17] idx 由 unsigned int 改 int：①ORIG 0x08550b41 首判是 `cmpl $0x0,-0x10`
// 有符号内存直比（cast 形在 4.4 反产 mov+test）；②0x08550b70 的 `m_users[idx]`
// 点为「值先」装载序（P218：int 下标→size_t 实参转换中转即复现），
// 尾段 m_points[seq]=idx 的 int 直存不变。
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
}