int Dispatcher_GoldTakeIncreasingAmount::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d55a4
    int err = check_error(user, msg);
    if (err < 1)
    {
        if (err < 0)
        {
            return -1;
        }
        stGoldTakeIncreasingAmount& p = static_cast<stGoldTakeIncreasingAmount&>(param);
        MSG_GOLD_TAKE_INCREASING_AMOUNT& req = reinterpret_cast<MSG_GOLD_TAKE_INCREASING_AMOUNT&>(msg);
        p.m_4 = *(int*)((char*)&req + 0xd);
        if (p.m_4 == 0xf68a)
        {
            CInventory* inven = (CInventory*)user->getCurCharacInvenW();
            user_creature::CCreatureMgr* mgr = inven->GetCreatureMgrW();
            int ret = mgr->SetOverSkillTime((const int&)p.m_4);
            if (ret == 0)
            {
                p.m_8 = 1;
            }
            else
            {
                p.m_8 = 0;
            }
            return 0;
        }
        return 0x15;
    }
    return err;
