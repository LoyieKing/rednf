// [b28d-L1] 修法（结构重写）：按 ORIG 0x0808ca7a 还原「形参 len 作循环变量 + 单一 rem 累加 + 分步就地 op= + 逐字节 movzbl 组装 + 加法 lea」后 ndiff 147→70。残余 = callee-saved 寄存器分配（data[1] 落 %edx vs %eax）与 tmp 槽位（ORIG -0x4 vs -0x8），N7/N82 封顶。P167 CLEAR（0/0）语义等价。
int DoSuperFastHash(char* data, unsigned int len, unsigned int& hash);

// ---- MakeHash 前置（P210 callee 定义顺序定律：调用者定义须在 DoSuperFastHash
// 定义之前，否则 4.4 按 callee 帧精确继承 outgoing，0x38 保守大帧塌缩为 0x20）----
unsigned char MakeHash(unsigned char* data, unsigned int len, unsigned char seed)
{
    unsigned int hash;   // ORIG 无 init store（DoSuperFastHash 出参槽直传）
    DoSuperFastHash(data, len, hash);
    unsigned char* digest = reinterpret_cast<unsigned char*>(&hash);
    return static_cast<unsigned char>(digest[0] ^ digest[1] ^ digest[2] ^
                                      digest[3] ^ seed);
}