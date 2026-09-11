// ORIG D1 0x8610432 W：i<=2 循环 delete 三容器
//（delete 空指针 no-op，编译器生成判空 + 静态 ~CStatisticContainer 0x8610412）。
CCommonStatisticsMgr::~CCommonStatisticsMgr()
{
    for (int i = 0; i <= 2; ++i)
        delete m_container[i];
}