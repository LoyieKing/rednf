CExpertJobList::CExpertJobList()
    : m_jobMap(), m_etc() {}

// [b28d-Main·检查点21] CExpertJobList/STExpertJobEtcScript 的 dtor 已删（ORIG 无符号）。

// ============================================================================
// init（ORIG 0x896d8ec）
// b28d-solo-CExpertJobList 判词（NEAR / N82附 封顶）：133=133 助记符序列全等；
// 残差仅 job 槽位（ORIG -0x24 vs 本 TU -0x40）与其后 make_pair/pair/insert/end
// 四个临时槽整体差 4——4.4 标量分区排布 vs 4.1 按生命周期穿插（N82附；job 在
// 函数头/循环体首/循环尾三形实测均落 -0x40..-0x48，it/script/begin 临时槽已对齐）。
// ============================================================================
