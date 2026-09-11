// ----------------------------------------------------------------------------
// ORIG T 0x08146722 _ZN6Global14CBossTowerFunc13SelectDungeonEP6CPartys17ENUM_DUNGEON_TYPE
// party 判空 → 0；getBossTower() 判空 → cMyTrace(PRETTY_FUNCTION,0x34,5) +
// "StaticPool BossTower NULL" → 0；按 get_member_count 映射 (1→0,2→1,3→2,4→3，
// 其它保持 0)；handleStartGame 失败时 _destroy()；返回其结果。
// ----------------------------------------------------------------------------
// [b28d-D9] N82/槽差：帧/槽序差（74 vs 66）（ORIG 0x08146722 sz 0x00000102）
bool Global::CBossTowerFunc::SelectDungeon(CParty* party, short dungeonIdx,
