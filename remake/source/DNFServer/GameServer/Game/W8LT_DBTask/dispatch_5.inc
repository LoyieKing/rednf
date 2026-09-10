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
