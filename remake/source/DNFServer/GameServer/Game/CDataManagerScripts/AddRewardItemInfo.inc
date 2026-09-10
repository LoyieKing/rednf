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
