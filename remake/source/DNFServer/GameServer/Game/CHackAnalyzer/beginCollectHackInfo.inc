void CHackAnalyzer::beginCollectHackInfo(CUser* user)
{
    // ORIG 0x080f79e8：+0x00 收 user、+0x20=系统当前秒、+0x24 清零、
    // +0x14 vector<int> clear（m_checkTypes）。
    *(CUser**)&m_baseHackType = user;  // ORIG *(int*)this = user（槽位复用，指针赋值序）
    m_field20 = GlobalData::s_systemTime_.getCurSec();
    m_field24 = 0;
    m_checkTypes.clear();
