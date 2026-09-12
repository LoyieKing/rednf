// ---------------------------------------------------------------------------
// [SemA04 2026-09-12] DES3_KeySchedule —— 按 ORIG 反汇编（0x080addf8, 0x174 字节）
// 手写等价实现：
//   · keyLen 必须 0x10(2-key 3DES) 或 0x18(3-key) 否则 0x1002；
//   · 依次对 K1=key[0..7]、K2=key[8..15]、K3=(0x18 ? key[16..23] : key[0..7])
//     调 DES_KeySchedule(key_i, 8, &tmp)，把 tmp.rk[0..0x7f] 拷到 alg->rk 的
//     第 1/2/3 段（字节偏移 +0x00/+0x80/+0x100，DES3_ALG_INFO::rk[0x180]）；
//   · 任一次失败（弱密钥 0x1006）立即以其返回值返回。
// 验证：Unicorn 原址执行 ORIG 的 DES3_KeySchedule（keyLen=0x10/0x18 各 3 组）⇒
//       我方 alg->rk 全 0x180 字节逐字节相等（见 des_done.md §DES3）。
// ---------------------------------------------------------------------------
int DES3_KeySchedule(unsigned char* key, unsigned long keyLen,
                     DES3_ALG_INFO* alg)
{
    DES_ALG_INFO tmp;
    unsigned char* k = (unsigned char*)alg + 0x24;
    int ret;
    int i;
    if (keyLen != 0x10 && keyLen != 0x18)
        return 0x1002;
    ret = DES_KeySchedule(key, 8, &tmp);
    if (ret != 0)
        return ret;
    for (i = 0; i < 0x80; ++i)
        k[i] = tmp.rk[i];
    ret = DES_KeySchedule(key + 8, 8, &tmp);
    if (ret != 0)
        return ret;
    for (i = 0; i < 0x80; ++i)
        k[0x80 + i] = tmp.rk[i];
    ret = DES_KeySchedule((keyLen == 0x18) ? (key + 0x10) : key, 8, &tmp);
    if (ret != 0)
        return ret;
    for (i = 0; i < 0x80; ++i)
        k[0x100 + i] = tmp.rk[i];
    return 0;
}
