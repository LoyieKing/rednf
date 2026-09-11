// ---- CNPCRelationEventManager::AddRewardItemInfo（ORIG 0x890cfda：
//      find(idx)，非 end 时写 node+kind*0x14+0xc / +0x10 两奖励值）----
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（4/4），gap=2、P=1（gap'=4）。
// 本轮复核：`(node->m_favor + kind)->m_rewardA/B` 数组名算术形（P214①）在 4.4 下
// 产出 `lea (%ecx,%eax,1),%edx; mov val,0xc(%edx)`（折 disp），ORIG 为两步
// `lea (%ecx,%eax,1),%eax; lea 0xc(%eax),%edx; mov val,(%edx)`（0x0890d052/0x0890d06d）；
// 另实参装载序（ORIG kind 先于 node）与 %ebx 角色差。属 N107（add-vs-lea 折 disp）
// + N7/N76（装载序）封顶；ORIG 0x0890cfda。
// [GW20Run 2026-09-11 复核] fresh 55/53；判词所述 ORIG 0x0890d04f/0x0890d052
// `lea (%ecx,%eax,1),%eax` + `lea 0xc(%eax),%edx` 仍逐条成立（我方 4.4 折 disp
// `mov %eax,0xc(%edx)`）；p167 4=4 CLEAR、enc_kr_scan 0 ⇒ 维持 N107。
void CNPCRelationEventManager::AddRewardItemInfo(
    unsigned short idx, ENUM_NPC_RELATION_EVENT_FAVOR_KIND kind,
    unsigned int rewardA, unsigned int rewardB)
{
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return;

    // ORIG 0x890cfda：每写各发一次 operator->/寻址（47<55 引用绑定会过度折叠，
    // node 两写形 57 最近）。
    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d052：`lea (%ecx,%eax,1),%eax; lea 0xc(%eax),%edx`（两步 lea）
    // ⇒ 数组名参与算术（P214①）而非下标形（4.4 下标形折 add $0xc）。
    // [b28d-solo-CDataManagerScripts-4 复核] 装载序（ORIG kind 先 / node 后 = RHS-first）
    // 与两步 lea 实测不可翻：删 `static_cast<int>`（异型化，P211 档③）与交换加法操作数
    // `(kind + node->m_favor)` 两形均仍产 node-first + `mov %eax,0xc(%edx)` 折 disp
    // ⇒ N107（add-vs-lea 折 disp）+ N7/N76（装载序）封顶，维持原形。
    (node->m_favor + kind)->m_rewardA = static_cast<int>(rewardA);
    (node->m_favor + kind)->m_rewardB = static_cast<int>(rewardB);
}