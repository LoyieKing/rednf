// 0x0868c170（返回 1 已通知 / 0 未满足条件）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868c170 | _ZN5CUser31send_ontime_reward_start_notifyEv
bool CUser::send_ontime_reward_start_notify()   // b28c-L5 SIGFIX（P174）
{
    // 0x0868c170 主分支（send_ontime_reward_start_notify.md）：事件 0x33 空 →
    // 日志+0；非活跃 / 奖励已开始（get_reward_state!=0）→ 0；日志块（文件名
    // ./log/<env>/OnTimeEvent + "On Charac Login"）后，ontime_id > 本地
    // last_recv_idx 且剩余时间 >=0 才发包 header(0,0x105) 单 int，返回 1。
    // b28d-GrpD 按 ORIG 0x0868c170 直证改写（4 处）：
    //  ① ORIG 只有**单个**事件指针槽 -0x24（GetRepeatEvent 结果直接落槽），+0x095
    //     `mov -0x24(%ebp),%eax` 直接作 this 调 get_reward_state ⇒ 无 ev 二次拷贝
    //     （我方旧 `CEventBase* eventRaw` + `COnTimeEvent* ev` 多占 -0x20 且多 2 mov）。
    COnTimeEvent* ev = (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // 2026-09-11 N8CUser P214①（正条件包裹）：ORIG 0x0868c194 `cmp DWORD PTR
    //   [ebp-0x24],0x0; je 0x0868c1c5` ⇒ fall-through 是 ev!=0 主体，而 trace
    //   早退块落 if 体之后（0x0868c1c5，紧跟 IsEventing 的 return-0 出口）；
    //   旧早退形（`if (ev==0) { trace; return 0; }`）4.4 产 `jne` + fall-through
    //   = trace 块 ⇒ 条件方向与块序双反。
    if (ev != 0)
    {
    if (!ev->IsEventing((CUser*)0))
        return 0;
    }
    else
    {
    cMyTrace(__PRETTY_FUNCTION__, 0xa7f7, 5)
        ("[ON_TIME_EVENT] send_ontime_reward_start_notify() error");
    return 0;
    }
    if (ev->get_reward_state() != 0)
        return 0;
    // b28d-solo-CUser p167：ORIG 0x0868a7f7 内联 rep stos（我方 EXTRA memset）。
    // [b28d-tu-P253] REAL-BUG：ORIG 0x0868c223-0x0868c237 `lea -0x148(%ebp),%ebx
    //   ; mov $0x0,%eax; mov $0x40,%edx; mov %ebx,%edi; rep stos` ⇒ %edx 是
    //   **dword 个数**：0x40×4 = **256 字节**（基 -0x148..-0x48），我方原
    //   `char fileName[0x40]` 只清 64B 且 sprintf("./log/%s/OnTimeEvent",
    //   get_file_name()) 的拼接路径可越过 64B ⇒ 少清 192B + 栈溢出风险。
    //   [取证 §3.2 兜底] CUser/send_ontime_reward_start_notify.md 为基线参考，
    //   rep stos 长度以 objdump 0x0868c223 直证为改据；[修正版 p167 复判]
    //   24=24 零差集（无 clone 别名）。
    char fileName[0x100] = {0};
    sprintf(fileName, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
    //  ④ ORIG +0x0fe..+0x138 顺序 = get_charac_no → get_acc_id → NumberToString →
    //     CMyFileLog ctor(-0x2c)，即实参先于 ctor 求值 ⇒ 源码是**全表达式临时链**
    //     `CMyFileLog(...)(...)`（P227/P37）；我方旧具名 log 在 sprintf 后立即构造。
    CMyFileLog(__PRETTY_FUNCTION__, 0xa803)(
        fileName, "On Time Event : On Charac Login(%s, %u)",
        NumberToString(get_acc_id(), 0), get_charac_no(-1));
    //  ② ORIG +0x17c `cmp %eax,%ebx; setg %al; test; je 0868c3ac`，而 +0x23c(0x0868c3ac)
    //     是 `mov $0x1,%eax`（公共出口）⇒ **id <= last_recv_idx 时函数返回 1**；
    //     我方旧 `if (id <= last) return 0;` 返回值相反（真语义 bug）。改正向包裹。
    if (ev->get_ontime_id() > get_ontime_last_recv_idx())
    {
        //  ③ ORIG +0x187 `movl $0x0,(%esp); call time; mov %eax,-0x20(%ebp)` 先于
        //     get_reward_end_time，随后 `sub -0x20(%ebp),%eax` ⇒ 源码具名 now（P0）；
        //     我方旧形 time 在右侧、gcc 求值序相反（且无 -0x20 槽）。
        int now = (int)time(0);
        int remain = (int)ev->get_reward_end_time() - now;
        if (remain < 0)
            return 0;
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x105);
        ((InterfacePacketBuf*)&packet)->put_int(remain);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
    }
    return 1;
}