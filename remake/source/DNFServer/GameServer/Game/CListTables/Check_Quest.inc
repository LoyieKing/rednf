bool QuestList::Check_Quest(Quest* quest, stSelectQuestParam& param,
                           const WongWork::CQuestClear& clear)
{
    // [b28d-32] ORIG 0x08355474：函数头把 param 7 字段逐条拷入栈局部
    // （-0x24..-0xd，+0x14 为 char），随后各检查用负形早退
    // `call; xor $1,%eax; test %al,%al; je fail; mov $0,%eax; jmp end`
    // （exposeQuest/check_possible 仍用原 param，其余用副本字段），尾 return 1。
    // [b28d-solo-CListTables] ORIG 0x08355474：逐字段重载 param 基址拷贝
    // （+0x14 单字节 movzbl/mov %al），4.4 隐式拷贝折成基址复用 + dword 块
    // （+0x14 按 4 字节搬）⇒ 写显式逐字段赋值。
    int level = param.m_level;
    int job = param.m_job;
    int firstGrow = param.m_firstGrow;
    int secondGrow = param.m_secondGrow;
    int pvpGrade = param.m_pvpGrade;
    char powerSide = param.m_powerSide;
    int questIdx = param.m_questIdx;
    if (!quest->exposeQuest())
        return 0;
    if (!quest->check_possible(param))
        return 0;
    if (!check_ahead_quest(questIdx, clear))
        return 0;
    if (!check_anti_quest(questIdx, clear))
        return 0;
    if (!quest->check_power_side(powerSide))
        return 0;
    return 1;
