void CGameManager::insertUserByAccID(CUser* user)
{
    unsigned int accId = user->get_acc_id();
    m_userByAccId[accId] = user;
    unsigned int accId2 = user->get_acc_id();
    if (accId2 == 0)
    {
        // b28d-lane12: 试 P227 全表达式临时链（N80 突破口）——ORIG 0x082945xx 在
        // cMyTrace ctor call 前取 get_acc_name 值并跨 ctor 保活 %ebx。
        cMyTrace(__PRETTY_FUNCTION__, 0x91d, 5)(
            "[QQID_ERROR]CGameManager::insertUserByAccID  qq_id:%s", user->get_acc_name());
    }
    else
    {
        m_userByAccId2[accId2] = user;
        GlobalData::s_pLogGameChannel->IncInUser();
    }
