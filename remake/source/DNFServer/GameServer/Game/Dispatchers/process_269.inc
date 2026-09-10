int Dispatcher_UpdateContractOfCubeInfo::process(CUser* user, MSG_BASE& msg,
                                                 ParamBase& param)
{
    MSG_UPDATE_CONTRACT_OF_CUBE_INFO& m =
        reinterpret_cast<MSG_UPDATE_CONTRACT_OF_CUBE_INFO&>(msg);
    stUpdateContractOfCubeInfo& p = static_cast<stUpdateContractOfCubeInfo&>(param);
    p.m_8 = m.m_useFlag;
    p.m_9 = m.m_selectIdx;
    // ORIG 0x0825f900（报告 Dispatcher_UpdateContractOfCubeInfo/process.md 全文已读）：
    // check_error 返回值直存 p.m_4（无 err 局部）；随后从 param 槽回读 m_4 成员值
    // （mov 回读 + test，非 cmpl mem），!=0 真体 fall-through return 0。
    p.m_4 = check_error(user, msg);
    if (p.m_4 != 0)
    {
        return 0;
    }
    // ORIG：setConctractOfCubePremiumInfo(useFlag!=0, selectIdx) 后 SelectCubeForPremiumService。
    user->setConctractOfCubePremiumInfo(m.m_useFlag != 0,
                                        (unsigned char)m.m_selectIdx);
    user->m_historyLog.SelectCubeForPremiumService(
        (int)(unsigned char)m.m_selectIdx);
    return 0;
