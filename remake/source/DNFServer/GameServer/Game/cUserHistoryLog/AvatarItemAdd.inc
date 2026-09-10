void cUserHistoryLog::AvatarItemAdd(int itemIdx, int avatarUid, const char* agency, eAvatarItemAddReason reason)
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (reason == static_cast<eAvatarItemAddReason>(1) || reason == static_cast<eAvatarItemAddReason>(3)
        || reason == static_cast<eAvatarItemAddReason>(4)) {
        if (*(0, m_traderCharac) != 0)
            m_user->LogHistory("Avatar+,%d,%d,%s,%d,\"%s\",\"%s\"", itemIdx, avatarUid, agency,
                               static_cast<int>(reason), m_traderCharac, m_traderAccount);
    } else {
        m_user->LogHistory("Avatar+,%d,%d,\"%s\",%d", itemIdx, avatarUid, agency,
                           static_cast<int>(reason));
    }
