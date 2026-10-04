// [b28d-lane2] verdict=DIFF ndiff=58（ORIG 0x084c8dca）：槽序/帧差(N82/P212/P221)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=78（ORIG 0x84c8dca）：N22 cmp 操作数方向+N18a 单段 regmove。
int Inter_TcpServerConnect::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TcpServerConnectPktView* pPkt = reinterpret_cast<Inter_TcpServerConnectPktView*>(data);
    if (pPkt->m_cmd == '\xcb')
    {
        // [b28c-30] ORIG 0x084c8dd0 段：index 槽 -0x18 预清、循环变量 i 独立槽
        // -0x10（`movl $0x0,-0x18; call GetStartIndex; mov %eax,-0x10`）；单变量形
        // 把 startIndex 存回 -0x18 且 GetServerProxy 结果落临时槽（+6 insn）。
        int index = 0;
        // [b28d-solo-InterDispatcher] CServerProxyMgr<CGuildServerProxy>::GetStartIndex
        // verdict=AE ndiff=0（ORIG 0x084c8dd0 段）：本波已归零（AE）。
        int i = GlobalData::s_guild_proxy_mgr->GetStartIndex();
        // ORIG 084c8e3a `mov -0x18(%ebp),%edx; cmp %edx,%eax; setg`：条件测 index
        //（GetNextIndex 引用更新者，取址 ⇒ 载入寄存器），非 i。while 形复现
        // ORIG 084c8e44 `jne <body 后向>` + 084c8e46 `jmp <跳过 else 分支链>`。
        while (GlobalData::s_guild_proxy_mgr->GetEndIndex() > index)
        {
            // ORIG 084c8e09/084c8e0e：GetServerProxy 结果直落 (%esp) 调 InitTcp，
            // 无具名 proxy 槽（旧具名形多出 -0x14 临时槽 +2 insn）。
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)i)->InitTcp();
            i = GlobalData::s_guild_proxy_mgr->GetNextIndex(index);
        }
    }
    else if (pPkt->m_cmd == '\xc9')
    {
        int index = 0;
        // [b28d-solo-InterDispatcher] CServerProxyMgr<CMonitorServerProxy>::GetStartIndex
        // verdict=AE ndiff=0（ORIG 0x084c8dd0 段）：本波已归零（AE）。
        int i = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (GlobalData::s_monitor_proxy_mgr->GetEndIndex() > index)
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)i)->InitTcp();
            i = GlobalData::s_monitor_proxy_mgr->GetNextIndex(index);
        }
    }
    return 0;
}