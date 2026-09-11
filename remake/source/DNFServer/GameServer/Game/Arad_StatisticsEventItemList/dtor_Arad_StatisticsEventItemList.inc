Arad_StatisticsEventItemList::~Arad_StatisticsEventItemList()
{
    // ORIG D1 0x819ccac：置 vptr 后析构 +0x08 vector，再析构基类。
    // ORIG D0 0x819cd0e：转发 D1 后 operator delete。
}