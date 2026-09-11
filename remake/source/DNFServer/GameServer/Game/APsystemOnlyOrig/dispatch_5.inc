// ORIG 0x83ffcac：基类 DBDispatcher::dispatch(userIdx, slot, stream) 前置检查
// 失败返 0；通过后 get_binary(buf, 0x70c) 失败返 0；DelAvatarItem 失败返 0，
// 成功返 1。
bool DB_AvatarItemDel::dispatch(int userIdx, int slot, Stream* stream)
{
    bool r;
    char buf[0x70c];
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    r = stream->get_binary(buf, 0x70c);
    if (r != 1)
        return 0;
    r = DelAvatarItem(*reinterpret_cast<SIG_AVATAR_DELETE*>(buf));
    if (r != 1)
        return 0;
    return 1;
}