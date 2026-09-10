bool DB_AvatarItemMove::dispatch(int userIdx, int slot, Stream* stream)
{
    bool r;
    SIG_AVATAR_MOVE* sig;
    sig = stream->GetOutBuffer<SIG_AVATAR_MOVE>();
    r = MoveAvatarItem(sig);
    if (r != 1)
        return 0;
    return 1;
