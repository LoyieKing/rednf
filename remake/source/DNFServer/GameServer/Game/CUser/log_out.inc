// 0x08658910
// ORIG：登出清理——清登录时间/播放经验/多开彩票失败计数、移除 GM 标记、
// 复位 uid/槽位/自增 ID/账号名、断网并整体 reset。
void CUser::log_out()
{
    m_tradeCount = 0;                     // CUser+0x8d10c（ORIG movl $0）
    resetPlayExpAdd();
    set_multiboxLotteryItemFailCnt(0);
    // ORIG 0x8658948/0x8658955 两次 call get_acc_id：右实参先评估存 %ebx→0x8(%esp)，左→%eax→0x4(%esp)
    // （gcc 实参右起评估；0x8 槽值非 0 常量而是第二调用结果，ORIG cmp 语义等价——原码即双调用）
    GlobalData::s_pGMAccounts_->removeGM(get_acc_id(), get_acc_id());
    m_historyLogFp = 0;
    m_chState = (ch_state)0;
    m_accId = 0;
    CUserGlobalInfoHandleInstance()->reset_uniqueid_flag(m_uniqueId);
    m_uniqueId = 0;
    SetSlotIDX(0);
    SetIncreID(0);
    memset(m_accName, 0, 0x15);
    m_connectCount = 0;
    m_onDisconnectState = 0;               // 状态标志字节（ORIG movb $0,0x8cfdc）
    m_network.disconnect();
    reset();
}