// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e19ca orig/ours=64/68 ndiff-blocks=11
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp) | mov    0x10(%ebp),%eax
int Dispatcher_PrecheckSoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e19ca：check_error(三参)；
    // err>0 → 写 param+0x04（继续）；err<0 → 返回 -1；err==0 → 发 monitor 预检包。
    // ORIG 0x081e19d1: `mov 0x14(%ebp),%eax; mov %eax,-0x14(%ebp)` —— param 引用槽；
    //   0x081e19d7: `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— msg 引用槽；
    //   0x081e1a2f 长度读 `mov 0x2c(%eax),%eax` 走该 msg 引用常量偏移。
    stSoloTeleportParam& out = static_cast<stSoloTeleportParam&>(param);
    MSG_SOLO_TELEPORT& m = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        // ORIG 0x081e1a06: `mov -0x14(%ebp),%eax; mov -0xc(%ebp),%edx; mov %edx,0x4(%eax)`
        //（原 `*(int*)((char*)&param+0x4)=err` 产 `lea 0x4(%eax),%edx` 形）
        out.m_4 = err;
    }
    else if (err < 0)
    {
        return 0xffffffff;
    }

    if (err == 0)
    {
        // ORIG 0x0822c5d2 ctor；+0x0a 起 21 字节名字区 strncpy(msg+0xd, 长度
        // *(int*)(msg+0x2c))；+0x1f int = getCurCharacNo(0x080cbc4e)；SendPacket 0x23。
        Packet_Monitor_Request_Charac_Info pkt;
        strncpy(pkt.m_data, m.m_0d, m.m_2c);
        pkt.m_field1f = user->getCurCharacNo();

        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&pkt), 0x23);

    }
    return 0;
}