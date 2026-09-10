void CLuckPoint::sendItemDropRate(char const* msg)
{
    for (unsigned int i = 0; i < m_users.size(); ++i)   // [seta 无符号循环]
    {
        CUser* user = m_users[i];
        if (user != 0)                                  // [0x8550f16 两判]
        {
            if (user->getCurCharacR())                  // [0x8550f22 setne 物化]
            {
                user->GetGameMasterCharacter()->sendServerState(user, msg);  // [0x8550f50 虚槽+0]
            }
        }
    }
