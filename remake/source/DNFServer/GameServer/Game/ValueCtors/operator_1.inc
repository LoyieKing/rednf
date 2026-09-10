void stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor::operator()(
    const std::pair<const LevelUpRewardItemTableKey,
                    LevelUpRewardItemData>& p) const
{
    // b28c-L20: AE — 真语义修正：ORIG 0x081b893d `cmp %ax,%dx; jg 函数尾`
    // 跳过 push_back ⇒ 命中条件为 `p.first.m_lv <= m_key->m_lv`（与
    // get_data_when_growup 0x081b895a 注释「pair.lv <= key.lv 才 push」一致）；
    // 原写 `>` 为方向反（P196 真反族）。三早退共享函数尾，嵌套正向形（P63）。
    if (m_key->m_grow == p.first.m_grow)
    {
        if (m_key->m_job == p.first.m_job)
        {
            if (p.first.m_lv <= m_key->m_lv)
            {
                m_out->push_back(p.second);
            }
        }
    }
