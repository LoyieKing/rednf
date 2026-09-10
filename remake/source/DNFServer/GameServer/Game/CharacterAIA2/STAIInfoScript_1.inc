STAIInfoScript::STAIInfoScript(const STAIInfoScript& other)
{
    // ORIG C1ERKS T 0x08a38bc6：vptr + 成员默认构造（编译器生成）→ clear() → operator=()。
    clear();
    *this = other;
