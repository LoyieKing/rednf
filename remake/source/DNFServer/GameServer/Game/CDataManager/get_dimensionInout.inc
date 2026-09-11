char CDataManager::get_dimensionInout(int idx)
{
    // ORIG 0x0822b612：返回类型 char（movzbl 单条读取，无 32 位扩展）；
    // 越界检查顺序与 ORIG 一致（cmp $5 在前、cmp $0 在后）。
    if (idx > 5 || idx < 0)
        return 0;
    return m_dimensionInout[idx];
}