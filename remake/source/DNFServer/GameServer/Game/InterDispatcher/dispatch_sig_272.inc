// ORIG 0x084e4d90 T dispatch_sig —— 绝望之塔随机选择完成上报监控服。
// 报告 docs/class_func_reports/Inter_TOD_ReturnToMonitor_APCSettingComplete/dispatch_sig.md 全文转写。
int Inter_TOD_ReturnToMonitor_APCSettingComplete::dispatch_sig(
    CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG +0x11 装参序 `mov <mgr>,%eax` 先于 `mov -0xc(%ebp),%edx` ⇒ 实参为 enum 同型 lvalue
    // （call 处 (ENUM)x cast 会反转成 edx 先载；probe use1/use2 实证）。
    const ENUM_SERVER_GROUP group =
        (ENUM_SERVER_GROUP)G_CEnvironment()->get_server_group();
    // ORIG +0x11：值先落 -0xc(%ebp)（普通 const 局部），再 `mov -0xc(%ebp),%ebx` 取别名
    // 跨 cMyTrace ctor 缓存 ⇒ 两级写法（register 别名；后续 GetServerProxy 仍读 -0xc 原槽）。
    register int tracedGroup = group;
    cMyTrace(__PRETTY_FUNCTION__,
        0x61f7, 5)("TOD Random Select Complete. send->monitor group(%d)", tracedGroup);
    Packet_TOD_DoRandomSelect packet;
    // ORIG 0x084e4df2/df7：this 先载 eax、group 后载 edx（enum 同型直传）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        group)->SendTcpPacket(reinterpret_cast<char*>(&packet), 10);
    return 0;
}