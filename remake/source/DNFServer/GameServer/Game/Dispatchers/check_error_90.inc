// ==== Dispatcher_ExpertExtraction::check_error (ORIG 0x081d962e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081d962e orig/ours=67/64 ndiff-blocks=11
//   虚调用目标寄存器 N7：ORIG `mov (%eax),%ebx; call *%ebx`（push ebx ⇒ 全槽深 4）；
//   4.4 恒目标 %eax 无 ebx ⇒ 帧差 4。已修段：uchar 读 + (char) 显式转换复现 ORIG
//   0x081d966d `movzbl; movsbl` 两段（原缺 movsbl）。N7 属后端族，路由表由 Main 管。
int Dispatcher_ExpertExtraction::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    MSG_EXPERT_EXTRACTION& m = reinterpret_cast<MSG_EXPERT_EXTRACTION&>(msg);

    // b28d-D0: ORIG 0x081d966d `movzbl 0xd(%eax),%eax; movsbl %al,%eax` 两段
    //   ⇒ 源为 uchar 读 + 显式转 signed char 再入 enum（直接 (ENUM)m_0d 只出 movzbl）。
    const int type = expert_extraction::GetExtracterType(
        (ENUM_EXPERT_JOB_TYPE)(char)m.m_0d);
    if (type == 3)
    {
        return 1;
    }
    expert_extraction::CExtraction* ex =
        GlobalData::g_ExtractionMgr->get_extracter((unsigned int)type);
    if (ex == 0)
    {
        return 1;
    }
    // ORIG 0x081d96xx：虚调用之后 `mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax`
    // —— 结果有**栈槽往返**（P0）⇒ 原码先存具名局部再 return；该槽同时把
    // m/type/ex 的槽基整体推深 4（ORIG -0x18/-0x14/-0x10）。
    const int ret = ex->check_error(user, (char)m.m_0d, (short)m.m_0e,
                                    (short)m.m_10);
    return ret;
}