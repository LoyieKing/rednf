// QuestList::getQuestList（CDataManager.h 类内内联，ORIG W 0x08374c7a：
// 8 字节 = mov this,%eax; ret）。Quest.cpp 现有代码不 odr-use 它
// （重建访问 QuestList 均经 G_CDataManager()->m_questList 成员直访）→
// 弱符号不发射；此处真实使用强制发射（不改语义、不造符号）。
void emitQuestListGetWeakSymbol()
{
    QuestList* ql = G_CDataManager()->m_questList;
    QuestList* p = ql->getQuestList();
    (void)p;
}