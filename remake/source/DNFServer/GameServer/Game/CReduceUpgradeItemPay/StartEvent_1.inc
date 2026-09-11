// ---------------------------------------------------------------------------
// ORIG 0x081171e2：param 两 ushort 写入基类 stADDINFO（+0x04/+0x06：
// movzwl 0xc(%ebp) → mov %dx,0x4(%eax)；movzwl 0xe(%ebp) → mov %dx,0x6(%eax)）
// → 虚调 StartEvent() → trace 行 0x1c，变参为两字段经 cwtl 符号扩展的
// (short) 值（movswl %ax,%esi/ebx；压栈序 0xc=info1、0x8=info0）。
// [b28c-30] N7 封顶：虚调段 ORIG 间接目标 %edx vs 4.4 恒 %eax（同族三件）；
// 其余行级差为 cMyTrace 字符串/函数地址的 .o 重定位伪差（AE 口径已归一）。
void CReduceUpgradeItemPay::StartEvent(Word_Param param)
{
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    StartEvent();
    cMyTrace(__PRETTY_FUNCTION__, 0x1c, 0)
        ("CReduceUpgradeItemPay(AddInfo:%d,%d) Event Start!",
         (short)m_addInfo.m_info0, (short)m_addInfo.m_info1);
}