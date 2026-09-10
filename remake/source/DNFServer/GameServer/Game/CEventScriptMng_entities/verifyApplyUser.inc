bool CUsedFatigueGiveItem::verifyApplyUser(CUser* user)
{
    // ORIG 0x08110756：job 门（-1 通配；两次独立重载 m_script1c，无具名指针/值槽，
    // P0 律）；user type 经 get_user_type() 取值。[b22判词·N-BOOL] ORIG 中
    // 不匹配标志经 eax 两段式流转（mov $1/$0,%eax; test %al; je）不留栈槽，
    // C++ bool 必物化槽位，寄存器流转为编译器结构不可源码复现，语义等价。
    // 重写：去具名 bool（4.4 必槽），改直接 return 链消 setne/cmpb 物化。
    if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_jobType >= 0 &&
        static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_jobType
            != user->get_charac_job())
        return false;

    switch (static_cast<unsigned char>(get_user_type()))
    {
        case 0:
            return true;
        case 1:
            // ORIG case1 汇聚形（call; test %al; je; mov$1; jmp; mov$0）：
            // if-return 形复现（具名 bool 透传为 4.4 直传=形不符）。
            if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)))
                return true;
            return false;
        default:
            return false;
    }
