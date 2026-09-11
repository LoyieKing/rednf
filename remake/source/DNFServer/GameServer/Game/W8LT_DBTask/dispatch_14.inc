// ORIG 0x84198bc：基类 dispatch 放行后 GetOutBuffer<SIG_LOAD_DUNGEON_CLEAR>，
// 依次 LoadDungeonClear → SendResult，全绿返 1。
bool DB_LoadDungeonClear::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 对应 ORIG 栈槽 -0xd/-0xc（gcc O0 首局部占最低槽）。
    bool r;
    SIG_LOAD_DUNGEON_CLEAR* sig = stream->GetOutBuffer<SIG_LOAD_DUNGEON_CLEAR>();
    if (sig == 0)
        return 0;
    r = LoadDungeonClear(sig);
    if (!r)
        return 0;
    r = SendResult(userIdx, slot, sig);
    if (!r)
        return 0;
    return 1;
}