int DisPatcher_UseCoin::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stUseCoinParam& out = static_cast<stUseCoinParam&>(param);
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1f4d,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    out.m_8 = req.m_0d;  // movzwl 0xd(%eax) -> mov %dx,0x8(%eax)
    // [Lane5] P251/P279：ORIG 0x081ca8a6 `call get_state; cmp $0x5,%eax; je;
    // cmp $0x8,%eax; je; jmp default` = 无槽 switch(call结果)，if 链会带状态槽。
    switch (user->get_state())
    {
    case 5:
    {
        CParty* party = (CParty*)user->GetParty();  // ORIG thunk 0x865514c = CUser::GetParty
        if (party == 0)
        {
            return LineFunc(0x1f5a,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        // ORIG 0x081ca929 `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax; xor $1`（具名 bool 槽）
        bool freeRevival = party->checkFreeRevivalCondition((CDungeon const*)0);
        if (!freeRevival)
        {
            // ORIG 无具名局部（`call getCurCharacInvenW; movl $0; mov %eax,(%esp); call UseCoin`）
            user->getCurCharacInvenW()->UseCoin(eCoinSubReason_0);   // asm push $0x0
            user->getCurCharacQuestW()->set_authen_data(QUEST_CONDITION_3, 1, 0);  // asm (3,1,0)
            party->IncreaseUsedCoinCount();   // thunk 0x822d8ce = CParty::IncreaseUsedCoinCount
            // ORIG 首参 0x25（_ActionGroupIndex 枚举，直压立即数）
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0x25, 0, (ENUM_PACKETCLASS)0);
        }
        break;
    }
    case 8:
        // ORIG 同样无具名局部（结果直传实参槽）
        user->getCurCharacInvenW()->UseCoin((eCoinSubReason)4);  // asm push $0x4
        user->getCurCharacQuestW()->set_authen_data(QUEST_CONDITION_3, 1, 0);
        user->IncreaseUsedCoinCount();      // thunk 0x823082a = CUser::IncreaseUsedCoinCount
        break;
    }
    return 0;
}
