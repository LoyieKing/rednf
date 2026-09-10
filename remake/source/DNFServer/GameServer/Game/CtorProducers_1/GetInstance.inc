CLevelRewardTableMgr* CLevelRewardTableMgr::GetInstance()
{
    // ORIG 0x0837505bW：判空 new 单例（无 __cxa_guard）。
    if (levelRewardTableMgr == 0)
        levelRewardTableMgr = new CLevelRewardTableMgr;
    return levelRewardTableMgr;
