bool CDeathTower::CStage::moveNextStage(unsigned int endStage)
{
    // ORIG 0x84617da：checkClearStage() 且 m_currentStage < endStage 时
    // ++m_currentStage 返回 true，否则 false。
    // [b28d-D13] P0 清绑（N96）：ORIG 0x84617eb xor $0x1 + mov$1/$0 物化由裸短路
    // 表达式自然产生，无绑定。
    if (((int)checkClearStage() ^ 1) || (unsigned int)m_currentStage >= endStage)
        return false;
    ++m_currentStage;
    return true;
}