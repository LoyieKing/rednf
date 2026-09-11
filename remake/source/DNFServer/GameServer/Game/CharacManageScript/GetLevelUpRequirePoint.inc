// ---------------------------------------------------------------------------
// CharacManageScript::GetLevelUpRequirePoint —— ORIG T 0x088e1114
// max 为 0 或越界返 0；否则取第 max 个门槛值。
// ---------------------------------------------------------------------------
int CharacManageScript::GetLevelUpRequirePoint(unsigned int maxLevel)
{
    if ((maxLevel == 0) || (maxLevel > m_manageLevelPoint.size()))
        return 0;
    return m_manageLevelPoint[maxLevel - 1];
}