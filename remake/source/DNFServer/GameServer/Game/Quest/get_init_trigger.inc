// b28d-solo-Quest AE：ORIG 0x083530d6。`*it++` 后置形 + `{0}` 两写清零 + 拆嵌套
// if + 两处 state 局部 `__attribute__((aligned(4)))`（复现 4.1 对 8 字节聚合的
// 4 对齐槽 -0x4c/-0x54；4.4 默认按 short 自然对齐 2 落 -0x4a/-0x52）⇒ 168=168。
int Quest::get_init_trigger() const
{
    if ((m_triggerType == 2) || (m_triggerType == 6))
    {
        std::vector<int>::const_iterator it = m_triggerData.begin();
        std::vector<int>::const_iterator end = m_triggerData.end();
        short values[3] = { 0 };
        int idx = 0;
        while (it != end)
        {
            // ORIG 0x8353145：每次迭代前进 4（type 6 为 5）个 int，取最后一项；
            // ORIG 形为 `*it++`（后置递增的临时副本再解引用，sret 槽 -0x34 等）。
            int v1 = *it++;
            int v2 = *it++;
            int v3 = *it++;
            if (m_triggerType == 6)
            {
                *it++;
            }
            int v4 = *it++;
            values[idx] = (short)v4;
            idx++;
        }
        // b28d-solo-Quest：ORIG state 槽基 -0x4c（4 对齐；4.1 对 8 字节聚合按 4 对齐），
        // 4.4 按 short 自然对齐落 -0x4a。局部 aligned(4) 只影响该对象（不动类型布局）。
        stQuestTriggerState_t state __attribute__((aligned(4)));
        state.setTrigger(values[0], values[1], values[2]);
        return state.getTrigger();
    }
    if (m_triggerType == 0x19)
    {
        stQuestTriggerState_t state __attribute__((aligned(4)));
        state.setTrigger(1, 1, 0);
        return state.getTrigger();
    }
    if ((m_triggerType == 1) && (m_triggerSubType == 6))
    {
        // ORIG 0x8353309：`call size; cmp $3; sete %al; test %al,%al; je`
        // ⇒ 拆嵌套 if 让末条件单独物化（P225 推广）。
        if (m_triggerData.size() == 3)
        {
            int v = m_triggerData.at(2);
            if (v > 0)
            {
                return v;
            }
        }
    }
    return 1;
}