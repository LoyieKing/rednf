// [b28d-solo-STQuestScript · P69] ORIG C1 0x0836e4e2（487 条）：40+ 成员子对象隐式
// 构造（含 EH cleanup 链）后**单一 `call QuestScript::clear()`**（836e738），ctor 体
// 无任何 memset ⇒ 原码 `QuestScript::QuestScript() { clear(); }`。
QuestScript::QuestScript()
{
    clear();
}