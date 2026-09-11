// ORIG 0x8400388：GetOutBuffer<SIG_AVATAR_MOVE>（null 不检查，ORIG 直传）→
// MoveAvatarItem 恒返 1 → 显式 if/return 分支（ORIG xor/test/je 形态）。
bool DB_AvatarItemMove::dispatch(int userIdx, int slot, Stream* stream)
{
    bool r;
    SIG_AVATAR_MOVE* sig;
    sig = stream->GetOutBuffer<SIG_AVATAR_MOVE>();
    r = MoveAvatarItem(sig);
    if (r != 1)
        return 0;
    return 1;
}