// this+0x4 为当前 key 字节（ORIG asm：movb $0x27,0x4(%eax) / movzbl 0x4(%eax)）；
// bNewKey 时先写固定种子 0x27，再以 rand() 刷新。
// [SweepPIHash] 修法（ORIG 0x0808be6e 79 条逐条对齐，79/79）：
//   ① 原码 rnd 是 uchar 字节槽（ORIG -0x15，`mov %al,-0x15`），非 uint 具名局部；
//   ② 原码 nHash(-0x14)/ret(-0x10)/h(-0x9) 三个具名局部声明即初始化 0
//      （ORIG 三条 `movl $0x0,-0x14`/`movl $0x0,-0x10`/`movb $0x0,-0x9` 实证）；
//   ③ 合成式书写序 = ORIG 计算序：h 项在前、rnd 项在后，末项 `(rnd & 0xc0) >> 6`
//      复现 ORIG `shr $0x6,%al`（`(rnd>>6)` 4.4 恒产 sar）；
//   ④ `ret = (nHash << 8) | (rand() & 0xff)`（左操作数先算）复现 ORIG
//      `mov nHash,%ebx; shl $8,%ebx; call rand; and $0xff,%eax; or %ebx,%eax`
//      的 callee-saved 跨 call 保活（右操作数先写则 rand 先 call，无 %ebx）；
//   ⑤ 原码第 8 项是 rnd>>6（非 key>>6）——旧源码误写 key>>6（真语义 bug，已修）；
//   ⑥ m_pad 为 char[4]，`char = uchar` 符号性异型 ⇒ 4.4 RHS-first；
//      三处改 uchar 左值视图 `*reinterpret_cast<unsigned char*>(m_pad)` 复现
//      ORIG LHS-first（P211 档③）。
// 残余仅 `and $0xc0,%ax`(ORIG 16 位) vs `and $0xc0,%eax`(4.4 32 位) 1 行 ⇒ NEAR。
unsigned int PISenderManager::MakeHashForSending(unsigned char* data,
                                                 unsigned int len,
                                                 bool bNewKey)
{
    if (bNewKey)
        m_pad[0] = 0x27;
    unsigned char key = m_pad[0];
    unsigned char rnd = static_cast<unsigned char>(rand());
    m_pad[0] = rnd;

    unsigned int nHash = 0;
    unsigned int ret = 0;
    unsigned char h = 0;
    if (len == 0)
        h = static_cast<unsigned char>(rand());
    else
        h = MakeHash(data, len, key);

    nHash = static_cast<unsigned short>(
        ((h & 3) << 6) | ((h & 0xc) << 12) | ((h & 0x30) >> 2) |
        ((h & 0xc0) << 4) | ((rnd & 3) << 12) | ((rnd & 0xc) << 2) |
        ((rnd & 0x30) << 4) | ((rnd & 0xc0) >> 6));
    ret = (nHash << 8) | (rand() & 0xff);
    return ret;
}