// ORIG _ZN13user_creature12CCreatureMgr16RegistTempPeriodEiii T 0x0833d7de。
// 汇编实证 key=a(0xc)、value=b(0x10)（0x833d7ed find(&a)、0x833d83b/0x833d842
// make_pair(&a,&b)）；形参 c(0x14) 未引用。查中 → false；插入后 true。
bool CCreatureMgr::RegistTempPeriod(int a, int b, int c)
{
    (void)c;
    std::map<int, int>::iterator it = m_field30.find(a);
    if (it != m_field30.end())
        return false;
    m_field30.insert(std::make_pair(a, b));
    return true;
}
