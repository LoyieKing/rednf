// ----------------------------------------------------------------------------
// _ActionPoint::_ActionPoint —— ORIG W 0x0848f5b4（无独立报告，objdump 转写；
// range [0x0848f5b4,0x0848f5d7]）
//   push %ebp; mov %esp,%ebp; sub $0x18,%esp
//   movl $0xb,0x8(%esp); movl $0x0,0x4(%esp)
//   mov 0x8(%ebp),%eax; mov %eax,(%esp); call memset@plt
//   leave; ret
// ----------------------------------------------------------------------------
_ActionPoint::_ActionPoint()
{
    memset(this, 0, sizeof(*this));   // ORIG 常量 0xb（pack(1) 后 sizeof 即 0xb）
}