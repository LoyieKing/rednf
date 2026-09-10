void HistoryLog::WriteLearnSkill(FILE* file, int skillIdx, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_LEARN_SKILL, time, skillIdx, level);
