// ORIG 0x08649b44：验证三段 CleanPad 种子和是否等于上报校验值。
bool CUser::VerifyCleanPadVeriData(unsigned int value)
{
    // ORIG 0x08649b44：sum 槽先 `movl $0x0,-0x4(%ebp)` 预清，再 A+B+C 存回。
    // P197③双槽置0：声明=0 + 独立赋值两段式（合并初始化式 4.4 丢预清行）。
    unsigned int sum = 0;
    sum = static_cast<unsigned int>(m_mcap.m_cleanPadVeriA) +
          static_cast<unsigned int>(m_mcap.m_cleanPadVeriB) +
          static_cast<unsigned int>(m_mcap.m_cleanPadVeriC);
    if (value == sum)
        return true;
    return false;
}