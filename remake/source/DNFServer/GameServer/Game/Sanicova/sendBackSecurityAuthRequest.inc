// ===========================================================================
// CUser::sendBackSecurityAuthRequest（报告 CUser/sendBackSecurityAuthRequest.md，
// ORIG T 0x08680706 sz 0x38f）：生成安全卡问题 → 随机加密算法号 → Pad 随机
// 数据 → Cipher 加密 → 0x98 包下发 → 置请求状态。
// ===========================================================================
void CUser::sendBackSecurityAuthRequest()
{
    // [P2S6 2026-09-11] ORIG 0x08680706+0x00c..+0x052：三处
    //   `mov %ebx,%edi; mov $N,%edx; mov $0,%eax; mov %edx,%ecx; rep stos`
    //   ⇒ 源码为聚合初始化 `= {0}`（gcc -O0 实测：`= {0}` → rep stos；
    //   memset → call@plt；for 循环 → 逐元素）。原 for 清零是 4.4 循环形。
    short question[32] = {0};   // ORIG -0x90 单块 0x40（question[3]+answers[29] 合一共清）
    question[0] = -1;

    // [P2S6 2026-09-11 试验] ORIG 0x08680731+0x008..+0x03c 有第二块
    //   `lea -0x50(%ebp),%ebx; mov $0x0,%eax; mov $0x9,%edx; mov %ebx,%edi;
    //    mov %edx,%ecx; rep stos`（36 字节零初始化），其后 objdump 全程**无任何
    //   -0x50(%ebp) 引用**（Ghidra: undefined4 local_54[9]，仅出现在清零循环）
    //   ⇒ ORIG 源码里存在一个未被使用的 36 字节聚合初始化局部。
    int spare24[9] = {0};       // ORIG -0x50（死对象，逐条对齐用）

    unsigned char encrypted[2048] = {0};            // ORIG -0x890 单块 0x800

    // [P2S6 2026-09-11] ORIG 0x0868075a/0x0868079e：`call getSecurityCard` 后 eax
    //   直接压栈给 generateQuestion（**无 card 栈槽**）⇒ 右值临时直用。
    getSecurityCard()->generateQuestion(question[0], question[1], question[2], question[3]);

    // [P2S6 2026-09-11] ORIG 0x0868079e `movl $0x800,-0x20(%ebp)` 在 generateQuestion
    //   之后（我方原先声明在 question 之后）⇒ 源码顺序：encryptedLen 后置。
    int encryptedLen = 0x800;                       // ORIG -0x20
    char algo = 1;                                  // -0x1e
    do
    {
        algo = (char)get_rand_int(0xe);
    } while (algo == 0 || algo == 5);

    // [P2S6 2026-09-11] ORIG 0x086807e1/0x086807f3/0x08680805/0x086808a1/0x086808c6/
    //   0x086808f4..0x0868099x：每次 `mov 0x8(%ebp),%eax; mov %eax,(%esp); call
    //   CUser::getPad` 后 eax 直用（**无栈槽**，p167 差集：ORIG getPad ×5 我方 0）
    //   ⇒ 源码为右值临时直用，禁具名 `CPad* pad` 局部。
    getPad()->GenerateRandomData();
    // [P2S6 2026-09-11] ORIG 0x08680805..0x08680834：`movzwl %ax,%eax; shl $0x4,%eax;
    //   mov manager_(%eax),%eax` 后直接压栈调 Encrypt（无 cipherIdx/cipherKey/cipher
    //   槽）。Encrypt 第 2 实参 = `movsbl -0x1a(%ebp),%ebx`（(int)algo 符号扩展）。
    GlobalData::s_secu_cipher_keystring_manager_[getPad()->GetSelectedCipherIdx() & 0xffff]
        .cipher->Encrypt((int)algo, (unsigned char*)question, 8, &encrypted[0],
                         &encryptedLen);
    // ORIG 0x08680847：`call getSecurityCard` → 直用 eax 调 getCertFlag。
    char certFlag = getSecurityCard()->getCertFlag();   // ORIG -0x19

    PacketGuard packet;
    packet.put_header(0, 0x98);
    packet.put_byte((int)algo);
    // ORIG 0x086808a3/0x086808d0 为 `movzwl %ax,%eax`（零扩展转换，非 `and $0xffff`）
    // ⇒ 源码用 (unsigned short) 截断（put_short 形参是 int，故 and 形不符）。
    packet.put_short((unsigned short)getPad()->GetKeyIdx(0));
    packet.put_short((unsigned short)getPad()->GetKeyIdx(1));
    // ORIG 0x086809xx：四次 GetShakeData 逐条展开（非 for 循环），每次重新 getPad()。
    // ORIG 0x086809xx 为 `movzbl %al,%eax`（零扩展，非 `and $0xff`）⇒ (unsigned char)。
    packet.put_byte((unsigned char)getPad()->GetShakeData(0));
    packet.put_byte((unsigned char)getPad()->GetShakeData(1));
    packet.put_byte((unsigned char)getPad()->GetShakeData(2));
    packet.put_byte((unsigned char)getPad()->GetShakeData(3));
    packet.put_int(encryptedLen);
    packet.put_binary((char*)encrypted, encryptedLen);
    packet.put_byte((int)certFlag);
    packet.finalize(true);
    Send(packet);

    if (getSecurityCard()->isActivate())
    {
        getSecurityCard()->setRequestState(WongWork::CSecurityCard::eRequestStateAuth);
    }
    else
    {
        if (getPad()->isActivate())
            getPad()->setRequestState(Sanicova::CPad::eRequestStateAuth);
    }
}