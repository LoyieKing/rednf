bool ProcStage::isEndStage(StageEndType::T& endType) const
{
    const bool allPhaseEnd = isEndAllPhase();
    const bool summonEnd = m_summonObjectMs.isEndSummon();
    const int aliveSummons = m_summonObjectMs.getAliveMonsterCount();
    int aliveOnMap = 0;
    for (std::map<short, _MapMonsterSummary>::const_iterator it =
             m_createdMapMonsters.begin();
         it != m_createdMapMonsters.end(); it++)
    {
        // ORIG `movzwl 0x8(...)` 读 _MapMonsterSummary+0x06 m_towerRefA（short）零扩 u16，
        //   与 CMap+0x110 m_110（int，`mov 0x110(%eax)` 全 dword）比较；P225 推广：
        //   && 链拆嵌套 if 才能让 4.4 对中间比较发 `sete; test; je` 物化。
        if (m_map != 0)
        {
            if (static_cast<unsigned>(static_cast<unsigned short>(it->second.m_towerRefA)) ==
                static_cast<unsigned>(m_map->m_110))
            {
                if (!it->second.m_padC)
                {
                    ++aliveOnMap;
                }
            }
        }
    }
    if (aliveOnMap == 0)
    {
        endType = StageEndType::END_N0;
        return true;
    }
    // b28d-solo-ProcStage：ORIG 0x812e3cd 为正向包裹——`cmpb $0,slot; je <尾 mov$0>`
    //   ×2 + `cmpl $0; jne`，即 4.1 对 bool 槽直接 cmpb（P222 正极性；4.4 对
    //   `!bool` 恒 movzbl+xor，N122）。it++ 为 P28 后置形（ORIG 增量段有独立临时槽
    //   -0x18，使 begin/end 槽深移 4 位对齐 -0x20/-0x1c）。
    if (allPhaseEnd && summonEnd && aliveSummons == 0)
    {
        endType = StageEndType::END_N3;
        return true;
    }
    return false;
