bool DB_AvatarItemInsert::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_AVATAR_INSERT* sig;
    sig = stream->GetOutBuffer<SIG_AVATAR_INSERT>();
    r = InsertAvatarItem(sig);
    if (!r)
        return 0;
    _SendResult(sig, slot);
    return 1;
