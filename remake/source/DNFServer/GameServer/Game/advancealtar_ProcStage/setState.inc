void ProcStage::setState(State::T state)
{
    // P95 先 this 后值：4.4 对普通成员赋值恒「先备值(mov 0xc,%edx)后装 this」，
    // ORIG 为先 this(0x8→eax)后值(0xc→edx)——经 pack(1) 具名视图访问翻转取数序
    //（/tmp/probeF.cpp sd 形实证）。偏移/宽度不变。
    struct V_P95
    {
        char m_pad4[4];   // +0x00（m_characData 槽）
        int m_state;      // +0x04
    };
    ((V_P95*)this)->m_state = static_cast<int>(state);
