// b28d-solo-OnlyOrigBatch7_Force verdict=DIFF（N 封顶）ORIG 0x086d9330
// orig/ours=30/29。同 test_and_set：get_atomic_mutex + 活跃检查 + 互斥 +
// true/false 双路清零（ORIG 0x086d935b/0x086d936f 两处 movb $0）。残余同上
// （prologue/epilogue、`int __e` 槽、4.1 unlock 尾调用 jmp vs 4.4 call+ret）。
// 语义逐条等价。
// [b29-R28 复核] p167：EXTRA `pthread_mutex_unlock`×1（ORIG 以 `jmp
// pthread_mutex_unlock@plt` **尾调用**收尾，4.4 因 push 保存寄存器只能 call+ret，
// ORIG 0x086d936a vs 我方尾部 `add $0x20; pop...; ret`）⇒ 跨版本 sibcall 差异，
// 非语义缺口 ⇒ N 封顶。
void atomic_flag::clear(memory_order) volatile
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    if ((void*)&pthread_cancel != 0)
    {
        if (int __e = pthread_mutex_lock(&__mutex))
            __throw_system_error(__e);
        self->_M_i = false;
        pthread_mutex_unlock(&__mutex);
    }
    else
    {
        self->_M_i = false;
    }
}