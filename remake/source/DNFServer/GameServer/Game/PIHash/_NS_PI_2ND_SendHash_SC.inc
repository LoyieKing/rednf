// ---- _NS_PI_2ND_SendHash_SC ORIG @ 0x082751ee ----
// [SweepPIHash] 修法（ORIG 0x082751ee 51 条逐条对齐）：
//   ① ORIG 具名局部按槽深序：bodyLen(-0x20,int)、body(-0x1c,uchar*)、
//      first(-0x15,char)、index(-0x14,short)、bNonClient(-0x11,bool)、
//      hash(-0x10,uint)、pCksum(-0xc,uint*)——旧版把 body/bodyLen/pCksum
//      内联进调用（少 3 个槽、帧 0x38 vs 0x44）；
//   ② bNonClient 用**单表达式 `bool b = (a && b)`** 物化（[P1T5 已翻 AE]）：
//      ORIG 0x082751fd 起 `mov $1,%eax; jmp; mov $0,%eax; mov %al,-0x11(%ebp)`
//      = 单一 store 点的 store_flag 形；旧 if/else 形 4.4 产 `movb $1,-0x11`×2
//      （探针 t1/t2/t3 即该形，故误判封顶）；改单表达式后 4.4 逐条同形 ⇒ 51/51 AE。
//   ③ receiver 走链式 `user->GetPISenderManager()->MakeHashForSending(...)`
//      （ORIG 无 sender 具名局部，GetPISenderManager 结果直接作 this）。
// ⇒ 本件已无残余（n1_v AE 51/51）。
void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user)
{
    unsigned int bodyLen = len - 0xf;
    unsigned char* body = reinterpret_cast<unsigned char*>(data + 0xf);
    char first = data[0];
    short index = _NS_PI_2ND_GetPacketIndex(data);
    // [P1T5 probe s1] 试单表达式形（ORIG 24-26 = mov $1,%eax / jmp / mov $0,%eax
    // + 单一 store 点 ⇒ 疑为 4.1 对 `bool b = cond;` 的 store_flag 形）。
    bool bNonClient = (first == '\0' && index == 1);

    unsigned int hash =
        reinterpret_cast<PISenderManager*>(user->GetPISenderManager())
            ->MakeHashForSending(body, bodyLen, bNonClient);
    unsigned int* pCksum = reinterpret_cast<unsigned int*>(data + 0xb);
    _NS_PI_2ND_ReconstructCheckSumRandomkey(*pCksum, hash);
}