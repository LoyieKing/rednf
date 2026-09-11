// [b28d-solo-CLeagueMatch] AE：TU 口径 identical/AE（25=25 全等；game_func_report
// 单函数口径多 1 条循环对齐 nop，§6.1/N112 已在 TU 口径剥离）。
bool CLeagueMatch::_getSameSchool(unsigned int* schools, int selfIdx,
                                 int count, unsigned int schoolNo)  // ORIG T 0x0855e7a8
{
    for (int i = 0; i < count; ++i) {
        if (i == selfIdx)
            continue;
        if (schools[i] == schoolNo)
            return 1;
    }
    return 0;
}