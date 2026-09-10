int DoSuperFastHash(unsigned char* data, unsigned int len, unsigned int& hash);
// [b28d-L1] 修法（结构重写）：按 ORIG 0x0808ca7a 还原「形参 len 作循环变量 + 单一 rem 累加 + 分步就地 op= + 逐字节 movzbl 组装 + 加法 lea」后 ndiff 147→70。残余 = callee-saved 寄存器分配（data[1] 落 %edx vs %eax）与 tmp 槽位（ORIG -0x4 vs -0x8），N7/N82 封顶。P167 CLEAR（0/0）语义等价。
