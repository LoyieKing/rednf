// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
void CUserCharacInfo::subDemensionInoutValue(char index)
{
    if (index > 5 || index < 0)
        return;
    m_selected->m_fieldEB8 = 1;
    --m_selected->m_demensionInout[index];
}