bool OneADayItemShopEvent::isQuestId(int questId)       // 0x081ba1e2
{
    // ORIG：TU 级表 correctQuestIds（D 0x09370168, size 0xc）=
    // {0x1567, 0x1568, 0x1569}；命中返回 1。定义见文件前部（file-scope，非 const）。
    for (int i = 0; i < 3; ++i)
    {
        if (correctQuestIds[i] == questId)
            return true;
    }
    return false;
}
