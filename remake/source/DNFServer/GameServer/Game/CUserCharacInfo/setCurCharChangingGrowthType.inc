// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
// 同 setCurCharGrowthType（身位字节写逻辑逐条一致）。
void CUserCharacInfo::setCurCharChangingGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    CUR->m_growTypeChanging.first = first;
    CUR->m_growTypeChanging.second = second;
}