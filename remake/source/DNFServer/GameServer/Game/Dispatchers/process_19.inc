int Dispatcher_BlueMarble::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e5fdc：`mov 0x14(%ebp),%eax; mov %eax,-0x14(%ebp)` —— p 先落槽，
    // 再 call check_error，结果免槽直用（`call; mov -0x14(%ebp),%edx; mov %eax,0x4(%edx)`）。
    stBlueMarble* const p = static_cast<stBlueMarble*>(&param);
    p->m_4 = check_error(user, msg);
    if (p->m_4 >= 1)
    {
        return 0;
    }
    if (p->m_4 < 0)
    {
        return LineFunc(0x60a9,
                        __PRETTY_FUNCTION__,
                        p->m_4, 0);
    }
    const MSG_BLUE_MARBLE& req = reinterpret_cast<const MSG_BLUE_MARBLE&>(msg);
    BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
    // ORIG 0x081e60xx：`cmpl $0x0,-0xc(%ebp); je <return 0 岛>` —— 非空才 fall 进
    // switch（技巧17：ORIG fall-through 块在前），空指针路径跳尾岛。
    if (marble != 0)
    {
        switch (req.m_0d)
        {
        case 0: marble->startBlueMarble(); break;
        case 1: marble->userThrowDice(user); break;
        case 2: marble->enterDungeon(user); break;
        case 3: marble->giveupDungeon(user); break;
        case 4: marble->clearDungeon(user); break;
        case 5: marble->exitDungeon(user); break;
        case 6: marble->leaveUser(user); break;
        case 7: marble->giveBonusItem(user); break;
        default: break;
        }
    }
    return 0;
