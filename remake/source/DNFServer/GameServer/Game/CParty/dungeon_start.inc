int CParty::dungeon_start(int dungeonIdx, char type, ENUM_DUNGEON_TYPE dtype)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。本波已补 7 处 P167 MISSING 调用
    //（get_member_count/get_min_level×2/get_charac_level/get_standard_level/
    // GetInstanceDungeonEntranceLog+IncrementDungeonEntrance/SetWeekendEvent），
    // 现仅 EXTRA memset（P78/P116 内联差）。@0x85a0954
    // P183：ORIG 本函数 bf/bd 每使用点重取址（无引用缓存槽）⇒ 宏直用。
