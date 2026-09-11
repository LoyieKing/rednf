int WarRoom::WalkOutUser(CUser* user)
{
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    return WalkOutUserBySlot(slot);
}