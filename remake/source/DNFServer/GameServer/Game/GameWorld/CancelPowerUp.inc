void GameWorld::CancelPowerUp()
{
    std::vector<CUser*> list;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        // b28c-lane3：P3——ORIG 0x86cf… 直测 `test %al,%al`（无 setne 归一化），
        // 说明被调实际返回单字节非 bool；头声明 bool 时 4.4 会加 test+setne。
        if ((unsigned char)user->IsPowerUp())
        {
            user->SetPowerUp(false);
            list.push_back(user);
        }
    }
    SendPowerUpList((ENUM_POWER_UP_ACTIVATE_TYPE)0, list);
}