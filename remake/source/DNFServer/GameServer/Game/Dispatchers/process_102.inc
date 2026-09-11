// ORIG 0x081e0ff8 T _ZN26Dispatcher_LimitNpcBuyItem7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_LimitNpcBuyItem/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stLimitNpcBuyItemParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// local_3a = LimitNpcBuyItemInfo（ORIG 0x26 字节，Monitor/LimitNpcBuyItem.h 权威布局）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e0ff8 orig/ours=73/71 ndiff-blocks=15
//   帧差 sub $44 vs $54 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x44,%esp | mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%edx
int Dispatcher_LimitNpcBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [G4-102 2026-09-11 · P0/P183] ORIG 0x081e1002 `mov 0x14(%ebp),%eax;
    //   mov %eax,-0x10(%ebp)` = param **具名引用槽**，其后每处 `mov -0x10(%ebp),%eax;
    //   mov 0x4(%eax),%eax` 现读 disp 0x4；无 err 具名槽（旧形另存 int err 且
    //   `&param+4` 被 4.4 缓存成 `lea 0x4(%eax),%ebx`，帧 +0x10）。
    // [G4-102b 2026-09-11 · 技巧14/P183] v1 的 `(char*)&outBase + 4` 实测仍产
    //   `lea 0x4(%eax),%ebx` + 每处 `add $0x4,%eax; mov (%eax),%eax`（探针
    //   /tmp/n7d/G4Analysis/probe_v2.s a_form 实证；.o 读数 78 vs ORIG 72）⇒ 改
    //   typed 视图成员，disp 折叠为 `mov %eax,0x4(%edx)` / `mov 0x4(%eax),%eax`
    //   （probe b_form/c_form 实证）。残余 1×保活 mov 属 N7 后端（4.4 跨 call 用
    //   callee-saved，ORIG 4.1 回读槽），不可由源码消除。
    struct LnbiParamView
    {
        int m_pad0;     // +0x00（ParamBase vptr）
        int m_4;        // +0x04 check_error 结果直存
    };
    ParamBase& outBase = param;
    reinterpret_cast<LnbiParamView*>(&outBase)->m_4 = check_error(user, msg);
    if (reinterpret_cast<LnbiParamView*>(&outBase)->m_4 >= 1)
    {
        return 0;
    }
    if (reinterpret_cast<LnbiParamView*>(&outBase)->m_4 < 0)
    {
        return LineFunc(0x55ae,
                        __PRETTY_FUNCTION__,
                        reinterpret_cast<LnbiParamView*>(&outBase)->m_4, 0);
    }
    LimitNpcBuyItemInfo info;   // local_3a（栈对象 0x26）
    info.m_itemIdReq = user->GetUID();             // local_30（+0xa）
    info.m_dbid = user->get_acc_id();              // local_2c（+0xe）
    info.m_charNo = (unsigned int)user->get_charac_no(-1);   // local_28（+0x12）
    info.m_itemId = *(unsigned int*)((char*)&msg + 0xd);     // local_24（+0x16）
    info.m_count = 1;                              // local_20（+0x1a）
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&info), 0x26);
    return 0;
}