// ---------------------------------------------------------------------------
// addChampionFeature — ORIG 0x88b9f66：同组(fc/ fd/fe/ff)冲突拒绝 → 重复拒绝 →
// 点数足够才 push_back 并扣点。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=AE ORIG 0x088b9f66 orig/ours=121/121。
// 修复：表符号 _ZL21CHAMPFEAT_POINT_MINUS（P204③ 文件级 static）、形参直读
// （P0：去 f/fGroup/group 局部）、每条件独立重调 operator[]（P157/P48）、含调用的
// || 链由 4.4 物化为 mov$1/mov$0/test/jne 岛（探针 q1/q2）、表值直算两次不缓存、
// FAIL/END 尾标签形。
void addChampionFeature(CHAMPION_FEATURES feature, int& count,
                        std::vector<int>& list)
{
    // ORIG 0x088b9f66：形参直读（P0：无 f/fGroup/group 具名局部），每个条件
    // 独立重调 operator[]（P157/P48），含调用的 || 链由 4.4 物化成
    // mov $1/mov $0/test/jne 岛（探针 q1/q2 实证）；末尾表值直算两次（不缓存）。
    for (std::vector<int>::size_type i = 0; i < list.size(); ++i)
    {
        if (((((list[i] & 0xff00) >> 8) == 0xff) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xff)) ||
            ((((list[i] & 0xff00) >> 8) == 0xfe) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xfe)) ||
            ((((list[i] & 0xff00) >> 8) == 0xfd) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xfd)) ||
            ((((list[i] & 0xff00) >> 8) == 0xfc) &&
             (((static_cast<int>(feature) & 0xff00) >> 8) == 0xfc)))
            return;
        if (list[i] == static_cast<int>(feature))
            return;
    }

    if (count <
        CHAMPFEAT_POINT_MINUS[(static_cast<int>(feature) & 0xff0000) >> 16])
        goto FAIL;
    list.push_back(static_cast<int>(feature));
    count -= CHAMPFEAT_POINT_MINUS[(static_cast<int>(feature) & 0xff0000) >> 16];
    goto END;
FAIL: ;
END: ;
}