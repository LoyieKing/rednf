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
