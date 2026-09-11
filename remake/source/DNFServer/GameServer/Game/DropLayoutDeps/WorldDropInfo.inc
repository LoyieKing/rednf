// ============================================================================
// WorldDropInfo 生命周期（本 TU 唯一 owner；无独立报告，objdump 直转）：
//   C1 0x08374ec8：仅构造 +0x04 的 vector<WorldDropItemChunck>；
//   D1 0x08374ede：仅析构该 vector；标量成员 m_first/m_total 无动作。
// ============================================================================
WorldDropInfo::WorldDropInfo() : m_chunks() {}
