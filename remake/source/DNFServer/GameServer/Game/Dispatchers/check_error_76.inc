int Dispatcher_GrowthWeaponEventChangeInfinity::check_error(CUser* user, MSG_BASE& msg)
{
    // [G1Frest 2026-09-10] ORIG 0x081e7f35 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    //   出现在 user==0 检查**之前**（P28 声明序即槽序）⇒ msg 引用绑定在最前。
    MSG_GROWTH_EQUIPMENT& req = reinterpret_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req.m_0d), req.m_11))
    {
        return 0xd5;
    }
    if (*reinterpret_cast<int*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x04) !=
        user->get_charac_no(-1))                                    // P48 无 CSE：每字段重调
    {
        return 3;
    }
    if (*reinterpret_cast<int*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x0c) != 0)
    {
        return 3;
    }
    // ORIG 0x081e7ff4 `movl $0x0,-0xc(%ebp)` ⇒ 原码带 `= 0` 初始化
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = (int)user->GetServerGroup();
    }
    else
    {
        // ORIG *(int*)(G_CEnvironment()+0x378)（SPremiumInfo.h:33 同址证据：目标服务器代码）
        serverGroup = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    if ((int)*reinterpret_cast<short*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x12) != serverGroup)
    {
        return 3;
    }
    if (*reinterpret_cast<short*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x10)
        < (short)G_CDataManager()->get_GrowthWeaponMaterialMax())
    {
        return 0x11;
    }
    // ORIG 0x081e8091 区：`mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; call *%edx`
    //   —— GetRepeatEvent 结果**无具名局部**（链式直调虚槽 +0x34）；尾极性为
    //   `xor $1; test; je` ⇒ `if (!IsEventing(0)) return 1; return 0;`
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x64)->IsEventing(0))
    {
        return 1;
    }
    return 0;
