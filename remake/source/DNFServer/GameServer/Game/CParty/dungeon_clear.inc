void CParty::dungeon_clear()
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85ab736
    SaveCoin((eCoinSubReason)1);
    // P183：ORIG 对 bd 每个使用点重取址（无引用缓存槽）⇒ 宏直用。
