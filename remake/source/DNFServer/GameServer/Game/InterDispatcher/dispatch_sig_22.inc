// ORIG 0x084c8a70 T dispatch_sig —— 截断服索取用户信息。
// 报告 docs/class_func_reports/Inter_CutOffCallUserInfo/dispatch_sig.md 全文转写。
int Inter_CutOffCallUserInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_CGameManager() == 0)
    {
        // ORIG 0x084c8a84-084c8aa2：null 支 trace 槽=-0x2c（深）、send 支=-0x1c（浅）
        // ⇒ 临时对象注册逆序分配（技巧3；probe cutoff w3：具名局部则正序、反 ORIG）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2249, 5)(
            "Inter_MonitorCallUserInfo::dispatch_sig()  G_CGameManager() is null");
    }
    else
    {
        char* p = data;
        G_CGameManager()->send_userinfos_to_cutoff_server();
        // [L9·2026-09-11 §6.5 串盲区] ORIG 0x084c8af2 `mov [esp+0x4],0x8c87878` ⇒ 串为
        //   "Cut Off Server Call UserInfo Arrived"（**无尾 \n**；.rodata 原字节已核，
        //   ORIG 0xc3f878 处 NUL 紧跟 'd'）。旧形多一个换行 ⇒ 日志输出与 ORIG 不一致。
        cMyTrace(__PRETTY_FUNCTION__, 0x2252, 0)(
            "Cut Off Server Call UserInfo Arrived");
    }
    return 0;
}