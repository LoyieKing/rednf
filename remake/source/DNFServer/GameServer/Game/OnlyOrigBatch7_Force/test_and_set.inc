// b28d-solo-OnlyOrigBatch7_Force ORIG 0x086d92d0。**最终读数：our 34→33（gap −1）**。
// 已按 ORIG 形恢复：TU-local get_atomic_mutex() + gthr posix
// 活跃检查（&pthread_cancel 弱符号，ORIG 0x086d92e9 同形）+ pthread_mutex_lock/
// unlock + true/false 双路写 _M_i（ORIG 只在 true 臂调 unlock，0x086d9307）。
// 已消 1 条：`int __e` 条件声明 → `register int __e` 提到 if 之前（见内注 P0）。
// 残余 1 条 = `mov %eax,%edi`（4.4 把 register __e 落到 callee-saved %edi，%ebx/%esi
// 已被 self/&mutex 占用；ORIG 4.1 让 __e 留 %eax 直接 `test %eax,%eax; jne`）——
// register 变量是 -O0 下唯一免栈槽手段（非 register ⇒ 恒落槽 +2 条，实测 34），
// 其寄存器选择不可由源码控 ⇒ N 封顶。另 prologue `sub $0x28; mov %ebx,-0xc(%ebp)`
// vs 4.4 `push %edi/%esi/%ebx; sub $0x1c` 为 N82 族（指令数相同）。语义逐条等价
// （锁保护 + 返回旧值；libpthread 存在时恒加锁，与 ORIG 的 gthr 判定一致）。
// [b29-R28 复核] p167 **CALLSITE CLEAR**（ORIG 4 calls = 我方 4 calls）⇒ 无缺口；
// [b29-S3 复核] 残余 1 条全为跨版本 codegen（register 变量寄存器类）⇒ N 封顶。
bool atomic_flag::test_and_set(memory_order) volatile
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    register bool __ret;
    if ((void*)&pthread_cancel != 0)
    {
        // [P0 消槽] ORIG 0x086d92fa 为 `test %eax,%eax; jne 0x086d9323`
        // （__e 留 %eax 无栈槽，throw 臂直接 `mov %eax,(%esp)`）；4.4 对条件
        // 内声明的 __e 恒落槽（-0x1c 三指令：mov/cmpl/mov）⇒ our 34 vs orig 32。
        // register 让 4.4 也走寄存器（语义不变：仅存储类说明符）；C++ 的
        // condition 声明不允许存储类说明符，故声明提到 if 之前。
        register int __e = pthread_mutex_lock(&__mutex);
        if (__e)
            __throw_system_error(__e);
        __ret = self->_M_i;
        self->_M_i = true;
        pthread_mutex_unlock(&__mutex);
    }
    else
    {
        __ret = self->_M_i;
        self->_M_i = true;
    }
    return __ret;
}