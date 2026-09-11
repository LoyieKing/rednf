// [b28d-28] DIFF | ORIG 0x081956b0 | N86/P28：find_if sret 临时槽序 + 谓词实参装载序。
// [L4fEventAvatar 2026-09-11] P0/P16 复原：ORIG 0x08195749 `mov 0x10(%ebp),%eax;
//   mov %eax,-0xc(%ebp)` ⇒ 具名 time 局部占 -0xc（其余槽随之下移 4）；外层
//   `if (it != end)` 直接 test（0x08195745，无 xor 取反），内层 `if (A && B)`
//   由 ORIG 物化为 `mov $1 / mov $0 + test + je`（0x08195775/0x0819577c/0x08195781）。
bool Arad_EventPeriodDataManager::isApplied(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    // P167：ORIG 用 SearchEventPeriod 谓词 + find_if（非 const iterator）
    SearchEventPeriod pred(eventCode);
    std::vector<ARAD::SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    if (it != m_eventPeriodList.end())
    {
        unsigned int t = time;   // ORIG 0x08195749 赋值点（if 体内）
        if (t >= it->m_beginTime && t < it->m_endTime)
            return true;
    }
    return false;
}