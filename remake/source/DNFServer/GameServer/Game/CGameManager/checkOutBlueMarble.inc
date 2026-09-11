void CGameManager::checkOutBlueMarble(CUser* user)
{
    // ORIG 同 CheckOutTrade 族：空 then + else → xor$1;test;jne + 双 jmp 尾岛
    if ((bool)(user->checkInBlueMarble() ^ 1))
    {
    }
    else
    {
        BlueMarble* marble = getBlueMarble(user->getBlueMarbleIndex());
        if (marble != 0)
        {
            marble->leaveUser(user);
        }
        return;
    }
    return;
}