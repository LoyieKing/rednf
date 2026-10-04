// [b28d-lane2] verdict=DIFF ndiff=43（ORIG 0x080e2104）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=44（ORIG 0x80e2104）：P30/N4 add-vs-lea。
int Inter_LoadCerashopAddRestrictType::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x200, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x201;
    }

    SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* load = reinterpret_cast<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE*>(data);

    // [H2·2026-09-11 laneD P0] ORIG 080e2164 `cmpl $0x0,-0x10(%ebp)` ⇒ 判空用局部 pPkt（非形参）。
    if (load == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x207, 5)("[%s] internal data is null.",
              __PRETTY_FUNCTION__);
        return 0x208;
    }

    CerashopAddRestrict::UserInfo& info = user->m_ceraUserInfo;
    // [H2·2026-09-11 laneD P0] ORIG 080e21ba `mov 0x4(%eax),%eax; test`（无槽往返）与
    //   080e2164 `cmpl $0x0,-0x10(%ebp)`（判空用局部 p）⇒ 删 `const int type` 具名中转，
    //   第 3 实参经 pPkt 槽（非形参 data）。
    // [L9-C PATCH#1] ORIG 080e21ba..080e21d2 为 `mov 0x4(%eax),%eax; test %eax,%eax; je A;
    //   cmp $0x1,%eax; je B; jmp C`（值只读一次 + 32 位 cmp 链）⇒ 源形是 switch（P251/P261；
    //   同 TU MonitorRegistGameServerAck 先例 + /tmp/L9/C/p_switch.cpp 探针直证）；if/else-if
    //   链在 4.4 恒退化为逐次重读（`mov -0x10(%ebp),%eax; mov 0x4(%eax),%eax; cmp $1,%al`）。
    switch (load->m_type)
    {
    case 0:
        CerashopAddRestrict::Manager::LoadProcess(
            info.m_field30, info.m_accountDaily,
            load);
        break;
    case 1:
        CerashopAddRestrict::Manager::LoadProcess(
            info.m_field48, info.m_characDaily,
            load);
        break;
    }
    CerashopAddRestrict::Manager::SendSyncPacket(user);
    return 0;
}