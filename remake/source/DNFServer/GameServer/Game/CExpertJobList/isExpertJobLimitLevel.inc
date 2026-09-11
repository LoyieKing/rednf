// ============================================================================
// STExpertJobEtcScript::isExpertJobLimitLevel（ORIG W 0x0849f47c）
// 遍历 m_levelMap（map<uchar,short>），任一 mapped short == level 返回 true。
// 供 CExpertJob::IncreaseExpertJobExp 判断当前等级是否为边界上限。
// ============================================================================
bool STExpertJobEtcScript::isExpertJobLimitLevel(int level)
{
    // [b28c-L29] ORIG W 0x0849f47c：循环尾用后置 it++（ppEi 带 dummy int 实参
    // 0x0 + sret 旧值槽 -0xc），非前置 ++it。
    for (std::map<unsigned char, short>::iterator it = m_levelMap.begin();
         it != m_levelMap.end(); it++)
    {
        if ((int)it->second == level)
            return true;
    }
    return false;
}