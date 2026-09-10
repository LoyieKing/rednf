int DisPatcher_Fish::dispatch_sig(CUser* user, PacketBuf& /*packet*/)

{
    if (user->get_state() != 4)
    {
        return 0;
    }
    G_GameWorld()->fishing(user);
    return 0;
}
