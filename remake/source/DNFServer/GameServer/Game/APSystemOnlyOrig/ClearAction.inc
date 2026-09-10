bool CActionPointManager::ClearAction(CUser& user, _ActionGroupIndex group,
                                      int itemIdx, ENUM_PACKETCLASS packetClass)
{
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121dd3：bool 汇聚变量 -0xd（初值 0，
    // 成功置 1，尾部 movzbl 返回）+ 行号 -0xc；**无 groupOut 具名局部**（ORIG 每
    // 次调用点重算地址并直装 ecx）。早返 `return true` 会改块布局并多槽。
    bool result = false;
    for (int row = 0; row <= 299; ++row)
    {
        if (m_sig.m_actionPoints[row].m_groupIndex == 0)
        {
            break;
        }
        if (m_sig.m_actionPoints[row].m_groupIndex != (int)group)
        {
            continue;
        }
        if (ClearActionOne(m_sig.m_actionPoints[row], itemIdx))
        {
            result = true;
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       (_ActionGroupIndex)m_sig.m_actionPoints[row].m_groupIndex,
                                       true, packetClass);
            break;
        }
        if (packetClass == 1)
        {
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       (_ActionGroupIndex)m_sig.m_actionPoints[row].m_groupIndex,
                                       false,
                                       static_cast<ENUM_PACKETCLASS>(1));
        }
    }
    return result;
