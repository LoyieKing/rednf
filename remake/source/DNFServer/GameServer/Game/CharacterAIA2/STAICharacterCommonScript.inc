STAICharacterCommonScript::STAICharacterCommonScript()
{
    // ORIG C1 0x088c8a78：vptr + 逐成员构造后调 clear()。
    // 成员构造由编译器生成；m_status 无用户 ctor，由 clear 内 Clear() 补齐。
    clear();
