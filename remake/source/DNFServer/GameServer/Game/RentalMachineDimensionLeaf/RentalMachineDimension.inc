// ----------------------------------------------------------------------------
// Impl C1 —— ORIG W 0x0826fa6c（objdump 转写）
//   mov 0x8(%ebp),%eax; add $0x104,%eax; mov %eax,(%esp); call setC1Ev
//   mov 0x8(%ebp),%eax; movl $0x104,0x8(%esp); movl $0x0,0x4(%esp)
//   mov %eax,(%esp); call memset@plt; leave; ret
// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0826eafc。
RentalMachineDimension::RentalMachineDimension()
    : m_impl(new RentalMachineDimensionImpl)
{
    initLoggingEnvironment();
}