int Inter_MonitorCeraUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 14 封顶。
    Inter_MonitorCeraUpdatePktView* pPkt = reinterpret_cast<Inter_MonitorCeraUpdatePktView*>(data);
    register unsigned int accId = pPkt->m_a;   // ORIG 0x084ceaa2 mov 0xa(%eax),%ebx（callee-saved 跨 call 缓存）
    CUser* target = G_CGameManager()->GetUserByAccId(accId);   // ORIG 0x084ceab6 结果落 -0xc 具名槽
    if (target == 0)                           // ORIG 0x084ceabd cmpl $0; jne body —— null 岛 fall-through 在前（技巧17）
        return 0;                              // ORIG 0x084ceabf mov $0; jmp end
    // ORIG 0x084ceae1 call *%ecx：vptr+0x2c = IIPGHelperInterface slot11 = IPGQuery(CUser*)
    // （TaiwanCash.h 声明序 slot11 注释 +0x34 基表偏移 = vptr+0x2c 同槽）
    GlobalData::s_pIPGHelper->IPGQuery(target);
    return 0;
