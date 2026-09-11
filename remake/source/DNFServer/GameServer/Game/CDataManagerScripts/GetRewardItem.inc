// ---- CNPCRelationEventManager::GetRewardItem（ORIG T 0x0890d326，sret
//      stNPCRewardItemInfo）：find(idx) 未命中 → {0,0}；命中 →
//      node->m_favor[kind].m_rewardA / m_rewardB（+0xc/+0x10）。----
// [b28d-solo-CDataManagerScripts-2] DIFF→：两个不同返回对象 + 早返（g3 探针
// /tmp/probe_cdms11.cpp 实证）复现 ORIG 的「result 落栈 -0x18/-0x14 + 早返拷贝」
// 与「命中分支 ret 直写 sret」双出口（单对象形被 4.4 NRVO 成整段 sret 写）。
// 残余 = N107 成员数组寻址折叠（ORIG `mov 0xc(%eax,%ecx,1)` 单条 scaled
// vs 4.4 `lea (%ecx,%eax,1); add $0xc`）；ORIG 0x0890d326。
// [GW20Run 2026-09-11 复核] fresh 54/55；ORIG 0x0890d3b0 起单条 scaled/两级寻址
// vs 我方 `add -0xc(%ebp),%eax; add $0xc,%eax` + 多次装载（寄存器角色族）；
// p167 4/5 唯一实质差 = EXTRA `memmove`×1（= 本条判词所述「4.4 对 sret 8B POD
// 拷贝发 call vs 4.1 内联两条 mov」的 N78/N84），余无 ⇒ 维持 N107/N7+N78。
stNPCRewardItemInfo CNPCRelationEventManager::GetRewardItem(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind)
{
    stNPCRewardItemInfo result;
    result.m_rewardA = 0;
    result.m_rewardB = 0;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;
    stNPCRelationEventInfo* node = it->second;
    // g6 探针：`return T{a,b}`（列表初始化临时）令 4.4 走 NRVO 直写 sret，
    // 复现 ORIG 命中分支的 `mov %eax,(%ebx); mov %edx,0x4(%ebx)`。
    // [b28d-J10 复核] 单次访问形：ORIG 只发一组 kind*0x14 地址计算
    // （0x0890d3b0..0x0890d3b7）后接两条 `mov 0x10(%eax,%ecx,1),%edx` /
    // `mov 0xc(%eax,%ecx,1),%eax` 读 8 字节 ⇒ 原码为**一次** 8 字节结构读取
    // （+0xc 起 m_rewardA/m_rewardB 与 stNPCRewardItemInfo 同布局）。
    // 实测：旧的「两次 `m_favor[kind]` 表达式 + braced-init」形重算两组
    // kind*0x14 乘法（68 insns）；引用绑定形 57、局部视图结构形 56（均劣化已回滚）
    // ⇒ 保留本形 55 insns。残余 1 条 = 4.4 对 sret 的 8B POD 拷贝发 `call memcpy`
    // （源址经 `add -0xc(%ebp),%eax; add $0xc,%eax` 两步），4.1 内联为
    // `mov 0x10(%eax,%ecx,1),%edx; mov 0xc(%eax,%ecx,1),%eax` + 两次写 ⇒ N78/N84
    // （4.1 内联小拷贝 vs 4.4 call）；证据 ORIG 0x0890d3ba..0x0890d3c4。
    return *reinterpret_cast<const stNPCRewardItemInfo*>(
        &node->m_favor[kind].m_rewardA);
}