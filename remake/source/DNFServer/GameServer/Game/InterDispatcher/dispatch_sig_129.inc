// P30/P248 R1 视图 disp 折叠族解法（ORIG 0x084d44xx 逐处实证）：
//   ORIG 对条目 5 个 unsigned short 发 `add $0x10,%eax` + `movzwl {0,2,4,6,8}(%eax),%eax`
//   ⇒ 原码里条目 +0x10 处是**子结构基址**（指针算术里带 +0x10，字段偏移按子结构自 0 起算）；
//   对 2 个 int 字段发 `add $0x1c,%eax`/`add $0x20,%eax` + `mov (%eax)` ⇒ 独立的直接指针算术。
//   我方原用扁平 EntryWinView 的绝对偏移（`movzwl 0x18(%eax)` 等），每处少 1 条 add。
//   EntryWinView 定义不在本件写域，故本件自带同布局子结构视图。
struct Inter_AutoPunishRuleHackTypeEntrySubView
{
    unsigned short m_0;            // 条目 +0x10  ORIG `add $0x10,%eax; movzwl (%eax),%eax`
    unsigned short m_2;            // 条目 +0x12  ORIG `add $0x10,%eax; movzwl 0x2(%eax),%eax`
    unsigned short m_4;            // 条目 +0x14  ORIG `add $0x10,%eax; movzwl 0x4(%eax),%eax`
    unsigned short m_6;            // 条目 +0x16  ORIG `add $0x10,%eax; movzwl 0x6(%eax),%eax`
    unsigned short m_8;            // 条目 +0x18  ORIG `add $0x10,%eax; movzwl 0x8(%eax),%eax`
};

// 条目基址 = pPkt + local_20 * 0x14；子结构基址另加 +0x10。必须逐处内联：
// 具名指针局部会在 ORIG 无槽处多出 `mov %eax,<S>` 往返（P0），故用宏保持表达式链。
#define G2_HKT_SUB(field) \
    (reinterpret_cast<Inter_AutoPunishRuleHackTypeEntrySubView*>( \
        reinterpret_cast<char*>(pPkt) + local_20 * 0x14 + 0x10)->field)

int Inter_AutoPunishRuleHackType::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    GlobalData::g_autoPunishRuleHackTypeMgr->reset();
    Inter_AutoPunishRuleHackTypePktView* pPkt = reinterpret_cast<Inter_AutoPunishRuleHackTypePktView*>(data);
    GlobalData::g_autoPunishRuleHackTypeMgr->setTotalTrade(pPkt->m_0,
                       pPkt->m_4);

    // P328⑩/P227：ORIG 0x84d4484..0x84d44b0 先求 m_0→%edi、m_4 低→%ebx/高→%esi，
    // 其后才 cMyTrace ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x3585, 0)(
        "HackType Load Start. (TotalGold: %llu)(TotalCount: %d)",
        pPkt->m_4,
        pPkt->m_0);

    for (int local_20 = 0;
         local_20 < (int)(unsigned int)(unsigned short)pPkt->m_c;
         ++local_20)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setHackCnt(
            (WongWork::ENUM_HACKTYPE)G2_HKT_SUB(m_0),
            G2_HKT_SUB(m_2),
            (WongWork::ENUM_HACKTYPE)G2_HKT_SUB(m_4),
            G2_HKT_SUB(m_6),
            G2_HKT_SUB(m_8),
            // ORIG `add $0x1c,%eax; mov (%eax),%eax`（直接指针算术，非 disp 折叠）
            *reinterpret_cast<int*>(reinterpret_cast<char*>(pPkt) + local_20 * 0x14 + 0x1c),
            // ORIG `add $0x20,%eax; mov (%eax),%edi`
            *reinterpret_cast<int*>(reinterpret_cast<char*>(pPkt) + local_20 * 0x14 + 0x20));
        // ORIG 0x84d45db..0x84d466f 先求 +0x16→-0x5c、+0x14→%edi、+0x12→%esi、+0x10→%ebx 再 ctor。
        cMyTrace(__PRETTY_FUNCTION__, 0x3596, 0)(
            "HackType Load (Type: %d -> %d)(SubType: %d -> %d)",
            (unsigned int)G2_HKT_SUB(m_0),
            (unsigned int)G2_HKT_SUB(m_2),
            (unsigned int)G2_HKT_SUB(m_4),
            (unsigned int)G2_HKT_SUB(m_6));
    }

#undef G2_HKT_SUB

    cMyTrace(__PRETTY_FUNCTION__,
        0x3599, 0)("HackType Load End");
    return 0;
