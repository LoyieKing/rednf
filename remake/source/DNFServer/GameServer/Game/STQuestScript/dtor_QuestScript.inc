// ORIG D1 0x08a70700：先 destroy()（delete 四组 11 指针），再成员自动析构。
QuestScript::~QuestScript()
{
    destroy();
}