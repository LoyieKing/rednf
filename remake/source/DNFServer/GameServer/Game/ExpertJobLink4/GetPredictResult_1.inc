// ---- DisJoint::GetPredictResult（报告 GetPredictResult.md，ORIG T 0x08473116）：
//      rarity ∈ [0,5] → &m_resultVecs[rarity]（this+0x628+rarity*0xc）；
//      越界 → &m_resultVecs[0]（this+0x628）。rarity 按有符号比较（js/jle）。 ----
std::vector<int>* DisJoint::GetPredictResult(int rarity)
{
    if (rarity < 0 || rarity > 5)
        return &m_resultVecs[0];
    return &m_resultVecs[rarity];
}