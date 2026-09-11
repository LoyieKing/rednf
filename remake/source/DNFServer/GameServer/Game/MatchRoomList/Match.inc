// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool CLeagueMatchRoomList::Match(IMatch* me, IMatch** out)
{
    // b28d-solo-MatchRoomList VERDICT: DIFF（447/446 均 136 vs 133）| 修法：
    // found/bestGap 前置 size 检查、bestIt!=end 收口、赋值序 bestGap→bestIt |
    // N7 + 浮点比较 lowering（ORIG flds/fxch/fucompp/fnstsw/sahf vs 4.4 恒
    // fucomip/fstp，TU 旗标不可动 ⇒ 封 N）
    // b28d-M2S3 推翻「旗标不可动」：本 TU 路由 = 446h47 后端 + -march=i486
    // （dnf_game_cxx.sh），x87 比较 lowering 已达 ORIG 形。实测残余只剩
    // 「操作数求值向」：ORIG 0x08567dd7 `flds -0x14(bestGap); flds -0xc(gap);
    // fxch; fucompp; fnstsw; sahf; seta` = `>` 形（探针 w2/w3/w5 直证），
    // 旧源 `gap < bestGap` 发 `fucompp; test $0x45,%ah; sete`（`<` 形）。
    // 两者语义等价（ST0>ST1 @ fxch 后 = bestGap>gap），改写法即可对齐。
    // 复测：135/136，唯一残差 = 尾部 1 条 `ret` 后对齐 nop（§6.1 豁免）⇒ identical 级。
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
        if (bestGap > gap) {
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
}