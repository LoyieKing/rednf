// ORIG 0x841a056：GetOutBuffer<SIG_SAVE_DUNGEON_CLEAR> 空指针返 0；
// SaveDungeonClear 失败返 0（无 SendResult 环节）。
bool DB_SaveDungeonClear::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 首局部占最低槽）。
    bool r;
    SIG_SAVE_DUNGEON_CLEAR* sig = stream->GetOutBuffer<SIG_SAVE_DUNGEON_CLEAR>();
    if (sig == 0)
        return 0;
    r = SaveDungeonClear(sig);
    if (!r)
        return 0;
    return 1;
}