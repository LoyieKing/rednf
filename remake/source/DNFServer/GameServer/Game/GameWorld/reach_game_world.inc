bool GameWorld::reach_game_world(CUser* user)
{
    if (user == NULL)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x509,
                              "param pUser is null");
        return false;
    }

    errno = 0;
    // ORIG 0x086c4ea0 iterator C1Ev（**先于** get_unique_id，P50 声明点即发射点）
    // + key 具名 -0x4a + find(&key)。
    std::map<unsigned short, CUser*>::iterator userIt;
    unsigned short uniqueId = (unsigned short)user->get_unique_id();
    userIt = m_UserInWorld.find(uniqueId);
    if (userIt != m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x515,
                              "m_UserInWorld.find() fail. uid(%u)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    // ORIG 0x086c4f5b iterator C1Ev（先于 get_acc_id）+ accId 具名 -0x44 + find(&key)
    std::map<unsigned int, unsigned short>::iterator sessionIt;
    unsigned int accId = user->get_acc_id();
    sessionIt = m_sessionList.find(accId);
    if (sessionIt != m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x51d,
                              "session_list.find() fail accid(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    // ORIG 0x086c5032/0x086c5040 具名 size(-0x10)/maxUserNum(-0xc)
    int worldSize = (int)m_UserInWorld.size();
    int maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    if (worldSize + 1 > maxUserNum)
    {
        return false;
    }

    // ORIG 0x086c50f8..0x086c5149 真 try/catch（landing pad → __cxa_begin_catch
    // → strerror(errno) → logFormat 0x52d → __cxa_end_catch → return false）。
    try
    {
        // ORIG 0x086c505e 第二次 get_unique_id 存 -0x2a 供 insert；0x086d3860 直接
        // 调 pair<const ushort,CUser*>::pair<ushort,CUser*&>(ushort&&,CUser*&)
        // （make_pair 被内联）⇒ value_type 直构（P192）。
        m_UserInWorld.insert(std::map<unsigned short, CUser*>::value_type(
            (unsigned short)user->get_unique_id(), user));
        m_sessionList[user->get_acc_id()] = (unsigned short)user->get_unique_id();
    }
    catch (...)
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x52d,
                              "[EXCEPTION insert_user In GameWorld] - code = %s\n",
                              strerror(errno));
        return false;
    }

    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        user->set_area(village->get_gate_area());
    }
    else
    {
        // ORIG 0x086c5185 getCurCharacVill 在 ctor **之前**求值存 %ebx（N80 族），
        // P227 全表达式临时链复现。
        cMyTrace(__PRETTY_FUNCTION__, 0x538, 5)(
            "getUserVillage %d", (int)user->getCurCharacVill());
    }

    user->set_state((ch_state)3);
    EraseLoginUser(user);
    send_AllBasicInfo(user);
    if (user->IsPermissionPrivateStore())
    {
        // ORIG 0x086c520d 单次 GetInstancePrivateStoreMgr（eax 复用为 this）。
        // [b28d-sweep] 真虚调用（P5/P113）：ORIG `mov (%eax),%edx; add $0x4,%edx;
        // mov (%edx),%ecx; call *%ecx` = IUserPos 槽1；CPrivateStoreMgr vtable
        // 0x8cbeb38 槽0=Exit(0x85ca994) / 槽1=Enter(0x85caa8a) ⇒ 目标是 Enter。
        // 旧实现 `((void**)storeMgr)[1]` 取的是对象 +0x04 成员值（非 vtable 槽），
        // 会把成员数据当函数指针调用 = 真语义 bug。
        // P0/P52：结果直作 receiver（ORIG 0x086c520d 单次 call 后 %eax 复用为 this，
        // 无具名 storeMgr 槽）。
        private_store::GetInstancePrivateStoreMgr()->Enter(user);
    }
    else
    {
        private_store::GetInstancePrivateStoreMgr()->LoadPrivateStore(user);
    }
    if (user->isHangameUser())
    {
        m_hangameUserCount++;
    }
    else
    {
        m_userCount++;
    }
    return true;
