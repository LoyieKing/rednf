// [b28d-sweep] **真语义 bug 修复**：私商分支虚调用目标还原为真实虚成员调用
// `GetInstancePrivateStoreMgr()->Enter(user)`（P5/P113）。ORIG 0x086c5212
// `mov (%eax),%edx; add $0x4,%edx; mov (%edx),%ecx; call *%ecx` = IUserPos 槽1；
// CPrivateStoreMgr vtable 0x8cbeb38 槽0=Exit(0x85ca994)/槽1=Enter(0x85caa8a)。
// 旧实现 `((void**)storeMgr)[1]` 取对象 +0x04 成员值当函数指针 = 真 bug。
// 271/273 → **271/271**（slotnorm 零残差）；p167 46/46（仅 C1/C2 别名）。
// [b28d-solo-GameWorld] _ZN9GameWorld16reach_game_worldEP5CUser verdict=DIFF 271/271
// 本轮修法（在既有 P50/P61/P192/try-catch 之上）：①P50 声明点 = 发射点 —— 两个
// iterator 声明移到 key（uniqueId/accId）**之前**，复现 ORIG 0x086c4ea0/0x086c4f5b
// 的 C1Ev 先于 get_unique_id/get_acc_id；②P227 全表达式临时链 —— getUserVillage
// 失败分支 cMyTrace 实参 getCurCharacVill 在 ctor 前求值存 %ebx（ORIG 0x086c5185）；
// ③**真语义 bug 修正**：私商分支虚调用改真实虚成员调用（见上方 b28d-sweep 判词）。
// 余 N82：槽组平移（slotnorm 零残差）。p167 46/46（C1/C2 别名对）。
// [A11-round5 复核] P190 撑帧反证：本件 ORIG userIt=-0x50 vs 我方 -0x48（我方浅 8）
// ⇒ 实测 2 个取址 double framePad 撑帧 +8：verdict NEAR 271/271 → **DIFF**（near 4→3 /
// diff 12→13）已回退 ⇒ 残差非帧尺寸差，系 4.1/4.4 槽基与槽组方向差（N82/N126 族）。
// 既有修法（保留）：P50 复活
// （ORIG 0x086c4ea0/0x086c4f5b iterator C1Ev 先声明后赋值）+ P61/P236 具名 key
// （ORIG uniqueId=-0x4a/accId=-0x44/-0x28）+ P192 value_type 直构 +
// **真 try/catch 补缺**（ORIG 0x086c50fb `__cxa_begin_catch` + 0x086c510a
// `strerror(errno)` + 0x086c513a logFormat 0x52d + 0x086c5144 `__cxa_end_catch`，
// 原先缺失 ⇒ p167 MISSING）+ 具名 size/maxUser（-0x10/-0xc）。
// [H8B 2026-09-11 复核·终判] 271/271 等长 + p167 46/46（仅 C1/C2 别名）⇒ 无真缺口；
// 残差 = 槽基差 8（ORIG 0x086c4ea0/0x086c4f5b userIt -0x50 vs 我方 -0x48；iterator C1Ev
// 先于 get_unique_id/get_acc_id 的顺序已对齐）。A11-round5 已实测 P190 撑帧 +8（2×double
// framePad）使 verdict NEAR→DIFF（near 4→3/diff 12→13）回退 ⇒ 残差非帧尺寸差，系 4.1/4.4
// 槽基与槽组方向差；P331/P212 声明序对整体槽基平移无效（N126 已证）⇒ N82/N126 封顶。
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

    // [Main 2026-09-11 · PoolE 旋钮实测（本件无效，已回退）] `Village* village` 改
    // `unsigned int villageAddr` 整型承载：槽组/verdict 逐值不变（NEAR 271/272、
    // ndiff 15）⇒ 本件残差非指针分区所致（系 4.1/4.4 槽基差 8 的 N82/N126 族）。
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
}