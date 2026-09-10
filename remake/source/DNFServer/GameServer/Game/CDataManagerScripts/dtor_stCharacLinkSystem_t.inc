stCharacLinkSystem_t::~stCharacLinkSystem_t() {}
// ---- CNPCRelationEventManager::GetRewardItem（ORIG T 0x0890d326，sret
//      stNPCRewardItemInfo）：find(idx) 未命中 → {0,0}；命中 →
//      node->m_favor[kind].m_rewardA / m_rewardB（+0xc/+0x10）。----
// [b28d-solo-CDataManagerScripts-2] DIFF→：两个不同返回对象 + 早返（g3 探针
// /tmp/probe_cdms11.cpp 实证）复现 ORIG 的「result 落栈 -0x18/-0x14 + 早返拷贝」
// 与「命中分支 ret 直写 sret」双出口（单对象形被 4.4 NRVO 成整段 sret 写）。
// 残余 = N107 成员数组寻址折叠（ORIG `mov 0xc(%eax,%ecx,1)` 单条 scaled
// vs 4.4 `lea (%ecx,%eax,1); add $0xc`）；ORIG 0x0890d326。
