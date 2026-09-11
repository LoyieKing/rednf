// ---- IsPcRoomUser —— T 0x08270fea（IsPcRoomUser.md）------------------------
bool IsPcRoomUser(CUser& user)
{
    return user.GetPCRoomNo() != 0;
}