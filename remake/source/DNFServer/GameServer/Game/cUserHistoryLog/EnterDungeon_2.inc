// ORIG T 0x0868491a（EnterDungeon.md 主报告）：
// 'AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d'——char 形参经 movsbl 符号扩展后打印。
void cUserHistoryLog::EnterDungeon(int dungeonIdx, int a, int b,
                                   const char* memberNames, char flag, int state)
{
    m_user->LogHistory("AdvanceDungeonEnter,%d,%d,%d,%s,%d,%d",
                       dungeonIdx, a, b, memberNames, static_cast<int>(flag), state);
}