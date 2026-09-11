void GameWorld::ResetAllCleadpadPoint()
{
    // N82/P112 封顶：残差全为 it/end临时/ppEi/cMyTrace/user 槽排布跨版本差
    //（ORIG it=-0x2c,trace=-0x20；4.4 it 族占低位、trace=-0x24），逐条指令
    // 与调用序全等，槽号集不等；声明序/提前声明均已试，不可翻。
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        user->m_mcap.reset();
    }
    // b28c-lane3：P221 全表达式临时链（单次调用），复现 ORIG trace 浅槽 -0x1c。
    cMyTrace(__PRETTY_FUNCTION__, 0x1385, 0)(
        "RESET ALL USER CLEANPAD POINT OK!!!!");
}