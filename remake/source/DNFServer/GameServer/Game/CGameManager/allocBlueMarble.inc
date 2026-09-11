void CGameManager::allocBlueMarble()
{
    // P0/P48：ORIG 0x082a339c 将 G_CDataManager() 结果 `mov %eax,(%esp)` 直传
    // getBlueMarbleScript（无 dm 槽）；仅 getBlueMarbleScript 结果存 -0x18 槽
    // （script 具名局部）。故勿命名 dm，内联 cast 进调用点。
    BlueMarbleScriptManager* script =
        (BlueMarbleScriptManager*)
        ((CDataManager*)G_CDataManager())->getBlueMarbleScript();
    for (int i = 0; i < 300; ++i)
    {
        BlueMarble* marble = getBlueMarble();
        marble->getMemoryPoolIndex();
        for (int j = 0; j < script->m_info.m_playerMax; ++j)  // ORIG *(script+0x30)
        {
            marble->setUserInfo(script);
        }
        marble->setScript(script);
    }
}