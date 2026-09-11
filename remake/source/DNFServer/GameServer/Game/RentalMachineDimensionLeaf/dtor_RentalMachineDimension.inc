// ----------------------------------------------------------------------------
// C1 —— ORIG T 0x0826eafc
//   call IRentalMachineC1Ev; movl $vtable,(%eax)
//   movl $0x11c,(%esp); call _Znwj; mov %eax,%ebx
//   call ImplC1Ev; mov %ebx,%eax
//   lea 0x4(%ebp 处 this),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)
//   call auto_ptr<Impl>C1EPS1_
//   call initLoggingEnvironmentEv; ret（异常路径 _Unwind_Resume）
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// D1 —— ORIG T 0x0826eba2
//   movl $vtable,(%eax); lea 0x4(%eax); call auto_ptr<Impl>D1Ev
//   call IRentalMachineD1Ev; mov $0x0,%eax; test %al,%al; je; ret
// ----------------------------------------------------------------------------
RentalMachineDimension::~RentalMachineDimension()
{
}