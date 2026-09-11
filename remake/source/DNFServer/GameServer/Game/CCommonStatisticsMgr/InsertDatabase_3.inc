// ORIG 0x0860e86e T：i<=2 循环逐容器虚调 InsertDatabase，无空指针分支。
void CCommonStatisticsMgr::InsertDatabase()
{
    for (int i = 0; i <= 2; ++i)
    {
        m_container[i]->InsertDatabase();
    }
}