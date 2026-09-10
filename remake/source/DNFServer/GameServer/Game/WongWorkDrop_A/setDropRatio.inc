void CMonsterDrop::setDropRatio(float ratio)
{
    m_probsSub = m_probs;                              // +0x24 ← +0x18（operator=）
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        // [b28d-solo-WongWorkDrop_A] ORIG 外层单次 operator[] 取具名元素
        //（-0x10 槽），内层两次索引直读同一元素（P157 例外：引用缓存）；
        // 直写 m_probsSub[i].v[2+r] 会每轮多两次 operator[] 调用。
        stGenItemProb_t& e = m_probsSub[i];
        for (int r = 0; r < 5; ++r)
        {
            // v[2+r]（元素 +8 + r*4）：五档稀有度权重 ×ratio 后回写
            e.w[r] =
                static_cast<int>(roundFloat(static_cast<float>(e.w[r]) * ratio));
        }
    }
    m_special.setDropRatio(ratio);                     // Special (+0x4706c) 先
    m_hell.setDropRatio(ratio);                        // Hell (+0x238ac) 后
