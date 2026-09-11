// ORIG T 0x0845fcc4：4 队伍槽金钱信息，尾部统一 finalize(true)。
void CDeathTower::CPacketHandler::appendPickupMoney(int* moneyA, int* moneyB, CParty* party)
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG：|| 短路判定（get_user==0 直接置 1，否则 !checkValidUser 取反）
        if (party->get_user(i) == 0 || !party->checkValidUser(i))
        {
            put_byte(0);
            put_int(0);
        }
        else
        {
            put_byte(1);
            put_int(moneyA[i]);
            put_byte(1);
            put_int(moneyB[i]);
        }
    }
    finalize(true);
}