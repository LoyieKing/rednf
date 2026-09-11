void CGameManager::CheckOutWarRoom(CUser* user)
{
    // ORIG 族形态：空 then + else（xor$1;test;jne + 尾岛）
    if ((bool)(user->CheckInWarRoom() ^ 1))
    {
    }
    else
    {
        WarRoom* room = GetWarRoom(user->GetWarRoomIndex());
        if (room != 0)
        {
            // ORIG cmp$0x4;sete%al;test;je：具名 ret 槽比较生成 cmpl+jne（无 sete），
            // (bool)(call()==4) 复现物化形态
            if ((bool)(room->WalkOutUser(user) == 4))
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x117b,
                                      "CGameManager::CheckOutWarRoom Can't find user charno(%d)",
                                      user->get_charac_no(-1));
            }
            room->CheckState();
        }
        return;
    }
    return;
}