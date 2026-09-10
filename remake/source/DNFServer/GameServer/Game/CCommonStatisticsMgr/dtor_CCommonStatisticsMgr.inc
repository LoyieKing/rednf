CCommonStatisticsMgr::~CCommonStatisticsMgr()
{
    for (int i = 0; i <= 2; ++i)
        delete m_container[i];
