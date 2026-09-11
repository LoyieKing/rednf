// ORIG 0x83ffdba：GetOutBuffer<SIG_AVATAR_INSERT> → InsertAvatarItem 失败返 0；
// 通过后 _SendResult(sig, slot)（无基类 dispatch 前置检查）。
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
}