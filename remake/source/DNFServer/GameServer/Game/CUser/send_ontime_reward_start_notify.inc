bool CUser::send_ontime_reward_start_notify()   // b28c-L5 SIGFIX（P174）
{
    // 0x0868c170 主分支（send_ontime_reward_start_notify.md）：事件 0x33 空 →
    // 日志+0；非活跃 / 奖励已开始（get_reward_state!=0）→ 0；日志块（文件名
    // ./log/<env>/OnTimeEvent + "On Charac Login"）后，ontime_id > 本地
    // last_recv_idx 且剩余时间 >=0 才发包 header(0,0x105) 单 int，返回 1。
    CEventBase* eventRaw = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    if (eventRaw == 0)
    {
        cMyTrace("bool CUser::send_ontime_reward_start_notify()", 0xa7f7, 5)
            ("[ON_TIME_EVENT] send_ontime_reward_start_notify() error");
        return 0;
    }
    if (!eventRaw->IsEventing((CUser*)0))
        return 0;
    COnTimeEvent* ev = (COnTimeEvent*)eventRaw;
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
    CMyFileLog log("bool CUser::send_ontime_reward_start_notify()", 0xa803);
    log(fileName, "On Time Event : On Charac Login(%s, %u)",
        NumberToString(get_acc_id(), 0), get_charac_no(-1));
    if (ev->get_ontime_id() <= get_ontime_last_recv_idx())
        return 0;
    int remain = (int)ev->get_reward_end_time() - (int)time(0);
    if (remain < 0)
        return 0;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x105);
    ((InterfacePacketBuf*)&packet)->put_int(remain);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
    return 1;
