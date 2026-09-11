// ---- ORIG T 0x081318e2（getClearStageSummary.md）：按 _Stage.m_field0
//      （difficulty 计数）分组累计 out[0..2]。----
// [b28d-solo-advancealtar_Manager AE] 本次修：switch(cast int) + fall-through 共享
// 叶块（P46/P251，case 2→1→0）+ it++ 后置（P28 ppEi）⇒ 66=66 逐条全等。
void CharacAdvanceAltarManager::getClearStageSummary(int out[3]) const
{
    const AdvanceAltarData* data = m_currentAdvanceAltarData;
    if (data == 0)
    {
        return;
    }
    // [b28d-solo-advancealtar_Manager] ORIG 单次 operator-> 后 `cmp $1/$2/test` 三路
    // （无 diff 具名槽）⇒ P251/P261 switch(cast int)（窄枚举 P305 禁裸 switch）；
    // 增量段为 ppEi ⇒ 源写 it++（P28 增量侧推论）。
    for (std::map<int, _Stage>::const_iterator it = data->m_stages.begin();
         it != data->m_stages.end(); it++)
    {
        // ORIG 叶块共享形：case2 块(+0x50) fall 到 case1 块(+0x5d) 再 fall 到
        // case0 块(+0x6a)（P46 叶块序=case 书写序）⇒ fall-through switch。
        switch (static_cast<int>(it->second.m_field4))
        {
        case 2:
            ++out[2];
        case 1:
            ++out[1];
        case 0:
            ++out[0];
            break;
        }
    }
}