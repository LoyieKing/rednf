// ---- CRYPTO_add_lock（ORIG T 0x087f5cd0）：
//      CRYPTO_lock(9,type,..) → *pointer += amount 存 ret → CRYPTO_lock(10)，
//      返回新值。ORIG 先测 lock_fn 全局指针，本 TU 单线程恒走顺序路径。----
int CRYPTO_add_lock(int* pointer, int amount, int type,
                    const char* file, int line)
{
    (void)file; (void)line;
    CRYPTO_lock(9, type, 0, 0);
    const int ret = *pointer + amount;
    *pointer = ret;
    CRYPTO_lock(10, type, 0, 0);
    return ret;
}