bool CLeagueMatchRoomList::Match(IMatch* me, IMatch** out)
{
    // b28d-solo-MatchRoomList VERDICT: DIFF（447/446 均 136 vs 133）| 修法：
    // found/bestGap 前置 size 检查、bestIt!=end 收口、赋值序 bestGap→bestIt |
    // N7 + 浮点比较 lowering（ORIG flds/fxch/fucompp/fnstsw/sahf vs 4.4 恒
    // fucomip/fstp，TU 旗标不可动 ⇒ 封 N）
    // b28d-solo-MatchRoomList: 还原 ORIG 0x08567d00——
    // ①found/bestGap 初始化前置于 size() 检查（ORIG movb$0/mov$0 在 call size 之前）；
    // ②循环尾用 `bestIt != end()` 收口（ORIG 无 `if(!found)` 物化）；
    // ③两处汇聚块赋值序 = bestGap 先、bestIt 后（ORIG mov gap→-0x14 在前）。
    bool found = false;
    float bestGap = 0.0f;
    if (m_roomMap.size() == 0)
        return false;
    std::map<IMatch*, int>::iterator bestIt = m_roomMap.end();
    for (std::map<IMatch*, int>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it) {
        IMatch* cand = it->first;
        if (cand == me)
            continue;
        if (!me->CheckMatchable(cand))
            continue;
        float gap = me->GetGap(cand);
        if (!found) {
            bestGap = gap;
            bestIt = it;
            found = true;
        }
        if (gap < bestGap) {
            bestGap = gap;
            bestIt = it;
        }
    }
    if (bestIt != m_roomMap.end()) {
        *out = bestIt->first;
        m_roomMap.erase(bestIt);
        return true;
    }
    return false;
