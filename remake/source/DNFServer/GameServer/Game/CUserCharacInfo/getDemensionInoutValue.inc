// [b28] 已消：旧「4.4 恒二段拷贝（N18a）」系指针值 cast 伪差，CUR 改具型
// 视图直读（文件头）后复现 ORIG 单段装载，本函数 AE。
unsigned char CUserCharacInfo::getDemensionInoutValue(char index)
{
    if (index > 5 || index < 0)
        return 0;
    return CUR->m_demensionInout[index];
}