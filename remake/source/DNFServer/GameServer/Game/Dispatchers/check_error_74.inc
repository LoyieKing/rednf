int Dispatcher_GrowthWeaponEventUseMaterial::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    // [G1Frest 2026-09-10] ORIG 0x081e83xx `mov %eax,-0x14(%ebp)` + 使用点
    //   `mov -0x14(%ebp),%eax; mov 0x11(%eax),%ebx` ⇒ msg 具名指针局部 -0x14，
    //   msg+0x11 在使用点直读（无 slotIdx 具名局部，P0/P183）。
    const char* mbase = reinterpret_cast<const char*>(&msg);
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
        1, *reinterpret_cast<const int*>(mbase + 0x11));
    if (item->m_addInfo == 0)
    {
        return 0x15;
    }
    if (item->m_addInfo != G_CDataManager()->get_GrowthWeaponMaterialItem())
    {
        return 0x11;
    }
    if (*reinterpret_cast<int*>(
            static_cast<char*>(user->getGrowthWeaponEventdata()) + 4) !=
        user->get_charac_no(-1))                                    // P48 无 CSE：每字段重调
    {
        return 3;
    }
    // ORIG `movl $0x0,-0xc(%ebp)` ⇒ 原码带 `= 0` 初始化
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() == 0)
    {
        serverGroup =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = user->GetServerGroup();
    }
    if (static_cast<signed short>(*reinterpret_cast<unsigned short*>(
            static_cast<char*>(user->getGrowthWeaponEventdata()) + 0x12)) !=
        serverGroup)
    {
        return 3;
    }
    if (static_cast<signed short>(*reinterpret_cast<unsigned short*>(
            static_cast<char*>(user->getGrowthWeaponEventdata()) + 0x10)) >=
        G_CDataManager()->get_GrowthWeaponMaterialMax())
    {
        return 0x11;
    }
    // ORIG 0x081e8469 区：`mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; call *%edx;
    //   xor $1; test; je` ⇒ 链式虚调（无 event 具名局部）+ `if (!IsEventing(0)) return 1;`
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x64)->IsEventing(0))
    {
        return 1;
    }
    return 0;
