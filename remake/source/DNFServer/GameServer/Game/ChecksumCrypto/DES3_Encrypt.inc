void DES3_Encrypt(void* rkArea, unsigned char* block)
{
    // [b28d-solo-ChecksumCrypto] DES3_Encrypt IDENTICAL（ndiff 7→0，ORIG=OUR=24 条全等）：
    // ①P210——把 DES_Encrypt/DES_Decrypt 定义后置于本函数之后 ⇒ 4.4 恢复 ORIG 保守帧
    // （实测 sub $0x18→$0x28、rk 槽 -0x4→-0xc）；②把 rk 声明为 `unsigned char* volatile`
    // ⇒ volatile 读强制先求值 arg0，复现 ORIG 4.1「先载 rk 入 %eax 再载 block 入 %edx」
    // （rk+0 / &rk[0] / 语句表达式 / const 局部 / void* 局部 / 显式 cast 六形实测均被
    // 4.4.7 折叠，顺序不变）。P167 调用点差集 CLEAR（ORIG 3 = OUR 3）。语义不变
    // （-O0 普通局部本就每次从槽读；调用序列/参数同）。
    unsigned char* volatile rk = (unsigned char*)rkArea;
    DES_Encrypt(rk, block);
    DES_Decrypt(rk + 0x80, block);
    DES_Encrypt(rk + 0x100, block);
