// ---- CRYPTO_lock（ORIG T 0x087f5b40：全局锁表读写）。单线程重建互斥空转
//      ——锁原语本身无返回语义，与 ORIG 在串行进程下行为一致。----
// [Q7S1 语义轴 · 复核 2026-09-11] 空转是**运行期等价**（直证，非"看起来像"）：
//   ① ORIG 0x087f5b6b `mov 0x9493450,%ecx; test %ecx,%ecx; je 87f5bb0`
//      ⇒ callback==NULL 时恢复寄存器直接 ret（不加锁）；
//   ② 0x9493450 落在 .bss（段 vma 0x93f56c0..0x95045e0）⇒ **加载期初值恒 0**；
//   ③ 其唯一写者 CRYPTO_set_locking_callback@0x087f5a20 在全 ELF 的
//      `call 0x87f5a20` 计数 = **0** ⇒ 运行期该全局恒 NULL。
//   唯一分歧：ORIG `mode<0` 的 OPENSSL_assert 岛（0x87f5ba0，file "cryptlib.c"）；
//   本 TU 全部调用点传 9/10，不可达。CRYPTO_new_ex_data/CRYPTO_free_ex_data 的
//   O-only 串 "ex_data.c" 同源于此（ORIG 侧那是 CRYPTO_lock(9/10,2,...,0xcb/0xce)
//   的 file 实参，非 ERR 调用）。
void CRYPTO_lock(int /*mode*/, int /*type*/, const char* /*file*/, int /*line*/)
{
}