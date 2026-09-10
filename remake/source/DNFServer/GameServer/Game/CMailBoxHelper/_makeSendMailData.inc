void CMailBoxHelper::_makeSendMailData(SIG_MAILBOX_SEND_NEW_MAIL* msg,
                                       const char* pSender, int nSenderCharacNo,
                                       Inven_Item const& item, unsigned int nDestCharacNo,
                                       unsigned int nAmount, const char* pTitle,
                                       int nTitleLen, unsigned int nB, int nType)
{
    // ORIG 0x08555756 逐字段转写。不缓存 msg/item 局部（O0 落栈槽导致帧差），
    // 每字段经宏直读参数槽，同 ORIG `mov 0x8(%ebp),%eax` 形态。
    // [b28d-solo-CMailBoxHelper] register 提示试验（/tmp/probe_reg.cpp g 形）：
    // 全 register 版 108 insn（少 ORIG 23 条，ORIG 仅段内 CSE 缓存），已回退。
    // [2026-09-09 Lane20] SIG_MAILBOX_SEND_NEW_MAIL 已在 SigTypes_D.h 字段化
    // （§8.3，sizeof 0x168/偏移/mangled 不变）⇒ 直接用形参具名类型访问，
    // 消掉赋值点的视图 cast（cast 会使 4.4 提前把 msg 装 callee-saved 保活：
    // ORIG 0x085557xx 各点均为 `call; mov 0x8(%ebp),%edx; mov %eax,disp(%edx)`）。
