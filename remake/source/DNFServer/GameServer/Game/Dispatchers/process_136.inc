// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cee64 orig/ours=138/138 ndiff-blocks=8
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %al,-0xd(%ebp) | movzbl -0xd(%ebp),%eax | xor    $0x1,%eax
int Dispatcher_SaveGameOption_2::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b23S3C] ORIG 0x081cee64：pm(-0x18)/go(-0x14)/type(-0xd)/sig(-0xc) 四槽 + guard(-0x24)，
    // job 无槽直传实参（P0：ORIG `call get_charac_job; mov %eax,0x4(%esp)` 两段）；
    // option 地址每用点现取 `mov -0x18; lea 0x11`（去 hotkeyOption 槽）；changed 判定
    // X1 形（P194 `register uchar v asm("eax")` + `v^=1` → ORIG `xor $1;test;je`）；
    // Acquire 直落 guard ctor 实参（去 stream 槽）；解引用显式 operator*()/operator->
    // （S23A 统一口径，ORIG 3 处 op*/op-> 前置 call）；sigBody alias 删，直接 m_pad。
    int ret = check_error(user, msg);
    if (ret > 0)
        return ret;
    if (ret < 0)
        return -1;
    MSG_BASE* const pm = &msg;                      // ORIG -0x18（option 源 msg+0x11）
    CGameOption* gameOption = user->GetGameOptionRef();
    unsigned char hotKeyType =
        (unsigned char)user->GetCharacHotKeyType(user->get_charac_job());
    struct __attribute__((packed)) HotkeyMsgView
    {
        char m_head[0x11];
        CHotkeyOption m_option;    // +0x11（ORIG read 落点，process 传引用）
    };
    // ORIG 0x081ceedx：`call IsHotKeyOptionChanged; xor $0x1,%eax; test %al,%al; je`
    // —— 结果无栈槽往返（P0）⇒ 原码是内联条件；消费端 3 条形按 P292/T3 证
    // callee 返回 bool（头声明改动已报 Main，本波先落裸形）。
    if (!gameOption->IsHotKeyOptionChanged(
            hotKeyType, reinterpret_cast<HotkeyMsgView*>(pm)->m_option))
        return 0;
    gameOption->SetHotKeyOption(
        hotKeyType, reinterpret_cast<HotkeyMsgView*>(pm)->m_option);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x28e1),
        true);
    guard.operator*()->operator<<(0x131);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_LOAD_CHARAC_HOTKEY_OPTION* sig =
        guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC_HOTKEY_OPTION>();
    sig->m_uid = user->get_acc_id();
    sig->m_pad[4] = (char)user->GetCharacHotKeyType(user->get_charac_job());
    memcpy(&sig->m_pad[5], &reinterpret_cast<HotkeyMsgView*>(pm)->m_option, 0xbc);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}