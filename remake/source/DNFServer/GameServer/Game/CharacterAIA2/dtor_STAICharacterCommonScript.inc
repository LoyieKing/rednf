STAICharacterCommonScript::~STAICharacterCommonScript()
{
    // ORIG D1 0x088c903c：vptr + destroy() + 成员逆序析构（编译器生成）。
    destroy();
}