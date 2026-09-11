// ============================================================================
// 三个子类型构造器为头文件内联（CDataManager.h，ORIG 具名 C1 0x837c6be/6e6/70e）。
// ORIG 0x891b4a8 W：临时元素 {kind,a,b} 后 push_back。
void stLimitItemUsageInfo::Add(char kind, int a, int b)
{
    LimitInfo entry;
    entry.m_kind = kind;
    entry.m_a = a;
    entry.m_b = b;
    m_list.push_back(entry);
}