STAIInfoScript::STAIInfoScript()
{
    // ORIG C1 W 0x08366f48：vptr + 成员默认构造（编译器生成）→ 调 clear()。
    // 强符号覆盖 STMonsterScript.h 的 inline 弱定义（后者内联 clear 展开，
    // 与 ORIG 的“成员构造 + call clear”结构不一致）。
    clear();
