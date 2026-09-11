// ---------------------------------------------------------------------------
// 13) CUser::_processUnlimitChallenge(unsigned int, char)
//     报告：docs/class_func_reports/CUser/_processUnlimitChallenge.md。ORIG 0x0864a5aa。
// ---------------------------------------------------------------------------
void CUser::_processUnlimitChallenge(unsigned int level, char kind)
{
    CLevelRewardTableMgr* mgr = CLevelRewardTableMgr::GetInstance();
    if (mgr == 0) return;
    // b28d-Main(P0 清绑 + P292)：IsOn() 已 bool 化（S47），裸 `if (!IsOn()) return;`
    // 直产 ORIG 0x0864a5cf `xor $0x1,%eax; test %al,%al; jne`，无需 register asm 绑定。
    if (!mgr->IsOn()) return;
    if (mgr->IsOnServer(G_CEnvironment()->m_serverEnvir.m_serverGroup)
        == false) return;
    if (mgr->IsHaveStartLevelReward() == false) return;
    _processLevelReward(1, level, kind);
}