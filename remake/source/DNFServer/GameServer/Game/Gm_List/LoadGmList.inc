// ORIG 0x081094ea：push_back({mid, level})。
void WongWork::CGMAccounts::LoadGmList(unsigned int mid, int level)
{
    // ORIG 0x081094ea：POD 直接两 store（mid 先、level 后），无 ctor call
    // （ORIG nm 无 stGMInfo_t C1 符号，聚合 POD）。
    stGMInfo_t info;
    info.mid = mid;
    info.type = static_cast<unsigned int>(level);
    m_gmList.push_back(info);
}