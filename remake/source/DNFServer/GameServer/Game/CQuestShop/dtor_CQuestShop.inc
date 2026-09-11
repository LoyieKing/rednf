// ORIG D1 0x85eec60 size 0x89：vptr 重置 → ~map(+0x20) → ~map(+0x08) → ~CData；
// D0（deleting）由编译器按虚析构规则产出（+operator delete，D1 后 delete this）。
CQuestShop::~CQuestShop()
{
}