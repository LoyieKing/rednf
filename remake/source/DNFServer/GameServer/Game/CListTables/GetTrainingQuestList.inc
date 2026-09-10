void QuestList::GetTrainingQuestList(std::vector<int>& out, short level)
{
    // [b28c-21] ORIG 两个 jmp+nop 中继岛（P223 多岛形：goto 尾标签串）。
    if (level >= 0x47)
        goto END1;
    if (level <= 0)
        goto END2;
    {
        out = *G_CDataManager()->getDailyTrainingQuest(level);
    }
END2:;
END1:;
