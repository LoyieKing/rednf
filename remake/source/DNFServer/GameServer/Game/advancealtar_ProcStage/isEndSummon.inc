bool ProcStage::ProcSummonObjectMs::isEndSummon() const
{
    // b28c-L5：ORIG 0x0812d50a 两处约束——
    //   ① `mov (%eax),%ebx`（m_currentMs 一步 32 位直落 callee-saved、跨 size()
    //      调用保活）⇒ 源侧**不得**写 static_cast（显式 cast 使 4.4 退化成
    //      `mov → mov %eax,%ebx` 两段并丢 push %ebx；int 与 size_t 的隐式 usual
    //      arithmetic conversion 即 ORIG 的 `cmp %eax,%ebx / setae` 无符号形）；
    //   ② 尾形 `test %al,%al; je; mov $1; jmp; mov $0` 是 if-return 分支物化，
    //      非 `return expr;` 的 setae 直返（P56）。
    if (m_currentMs >= m_summons.size())
    {
        return true;
    }
    return false;
