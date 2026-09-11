// 删收集物（INVEN_TYPE 1 / reason 3），失败 LineFunc(0x5aa7)；成功把
// slot/count 回填 param，并向监控服代理上报 Packet_CollectItems(0x39 字节)。
// [b28c-31] P0 清绑（原 register int ec asm("edx") + register dst asm("eax") 已删）；
// 残差 N96+N18a：ec 中转 ORIG %edx vs 4.4 %ebx、m_count 窄化 ORIG 单段 movzwl→%edx
// vs 4.4 两段（0x081e34xx 实证）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e343c orig/ours=122/122 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    %edx,0x4(%eax) | movl   $&_ZZN23Dispatcher_CollectItems7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_CollectItems::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b23S3C] ORIG 0x081e343c：msg(-0x10)/param(-0xc) 双指针槽物化、去 alias 地址槽
    // （P0 律，ORIG 无 slot/count/inv/name/proxy 槽）；check_error 直存 out->m_4
    // （StartGame@1231 形：值 %edx/base %eax）；delete_item count/slot 实参直传升格
    // esi/ebx（P197②）；m_itemIdx 双 movzwl=(uint)(ushort) 两段 cast；strlen/strncpy
    // 各自重取 getName（P48）；proxy 链式右值。
    MSG_COLLECT_ITEMS& req = reinterpret_cast<MSG_COLLECT_ITEMS&>(msg);
    stCollectItems* const out = static_cast<stCollectItems*>(&param);
    // [b28d-solo-Dispatchers] P0：ORIG 0x081e345x 无 ec 槽（值 edx 直传）⇒ 删
    //   `register unsigned int ec` 载体，改内联实参 + callee unsigned（P211 callee 侧
    //   旋钮）；旧「去 register 落槽 -0xc」是 int callee 同型 LHS-first 形。
    out->m_4 = check_error(user, msg, param);
    if (out->m_4 >= 1)
        return 0;
    if (out->m_4 < 0)
        return LineFunc(0x5aa2, __PRETTY_FUNCTION__, out->m_4, 0);
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (int)req.m_itemIdx, (unsigned int)req.m_count,
            (eItemDelReason)3, true))
        return LineFunc(0x5aa7, __PRETTY_FUNCTION__, 0, 0);
    out->m_8 = req.m_itemIdx;
    // [b28d-solo-Dispatchers][N18a 封顶] ORIG 0x081e35xx `movzwl 0x10(%eax),%edx;
    //   mov %dx,0xa(%eax)`（4.1 直接零扩展入 edx）；4.4 恒 `movzwl …,%eax;
    //   mov %eax,%edx` 两段。探针 /tmp/probe_ushort.cpp f/g/h/i 四形（含
    //   `*(unsigned short*)&` 同型视图）全部两段或更劣 ⇒ 跨版本寄存器分配封顶。
    // [P211 档③ 试形记录] 同型视图 `*(unsigned short*)&out->m_a = ...` 反产 lea+add 两步
    // （ours 122 但结构更差）；直接成员赋值 = 唯一 1 条差（4.4 movzwl→eax 再中转 edx）。
    out->m_a = req.m_count;
    Packet_CollectItems notify;
    notify.m_itemIdx = (unsigned int)(unsigned short)req.m_count;
    notify.m_characNo = user->get_charac_no(-1);
    notify.m_accId = user->get_acc_id();
    notify.m_nameLen = (char)strlen(user->getCurCharacName());
    strncpy(notify.m_name, user->getCurCharacName(), 0x1d);
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)user->GetServerGroup())->SendTcpPacket(
        reinterpret_cast<char*>(&notify), 0x39);
    return 0;
}