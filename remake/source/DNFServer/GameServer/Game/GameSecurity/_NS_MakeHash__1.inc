// ===========================================================================
// _NS_MakeHash_  ORIG T 0x808c8b5（报告 _lt_global_gt_/_NS_MakeHash_.md）
// len 越界（<=0 或 >0x190）返回 0；否则一步 md5() 后按 ORIG 逐字节 xor 折叠：
//   d[0] ^= d[8]^d[10]^d[13] ^ 0x81   d[1] ^= d[5]^d[9]^d[12]  ^ 0x78
//   d[2] ^= d[4]^d[6]^d[14]  ^ 0x1a   d[3] ^= d[7]^d[11]^d[15] ^ 0xbf
// 结果以小端 dword 写 *out（ORIG mov (%eax),%edx 读 4 字节）。
// ===========================================================================
// [b23S3C] ORIG 尾 `mov $0x1,%eax; leave; ret`+调用点 xor$0x1 直测低字节 ⇒ 返回 char；
// 体 ORIG 单 call _Z3md5PhiS_（0x80b3d57），勿展开 starts/update/finish 三 call。
// [b28d-24] DIFF 残20: xor 链中转寄存器差异（ORIG 用 %ecx 逐段 `xor %eax,%ecx`；4.4 用 %edx 且多 1 条 xor）。ORIG 0x0808c8b5
bool _NS_MakeHash_(char* data, short len, int& out)
{
    if (len <= 0 || len > 0x190)
        return 0;

    unsigned char digest[16];
    md5(reinterpret_cast<unsigned char*>(data), len, digest);

    // [b28d-solo-GameSecurity] ^ 结合分组照 ORIG：`d[i] ^ (d[a]^d[b]^d[c]) ^ K`
    // 使 4.4 用 %ecx 累积三元组、d[i] 留 %edx 最后异或（探针 /tmp/mh_probe.cpp h2
    // 与 ORIG 0x0808c8b5 逐条同构）；左结合平铺形 4.4 改 %edx 累积且多 1 条 xor。
    // 三元组内操作数序按 ORIG 求值序（首项进 %ecx、次项进 %eax）：10-8-13 / 12-5-9。
    digest[0] = static_cast<unsigned char>(
        (digest[0] ^ (digest[10] ^ digest[8] ^ digest[13])) ^ 0x81);
    digest[1] = static_cast<unsigned char>(
        (digest[1] ^ (digest[12] ^ digest[5] ^ digest[9])) ^ 0x78);
    digest[2] = static_cast<unsigned char>(
        (digest[2] ^ (digest[4] ^ digest[14] ^ digest[6])) ^ 0x1a);
    digest[3] = static_cast<unsigned char>(
        (digest[3] ^ (digest[7] ^ digest[11] ^ digest[15])) ^ 0xbf);

    // ORIG lea digest,%eax; mov (%eax),%edx; mov %edx,(out) —— 小端 dword 直读，非逐字节 or。
    out = *reinterpret_cast<int*>(digest);
    return 1;
}