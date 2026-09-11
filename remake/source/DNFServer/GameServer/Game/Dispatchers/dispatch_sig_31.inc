int Dispatcher_CodeCheckSum::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82044b0 (101 insn)：读 int + 0x20 字节校验块 → checkCodeChecksum；
    // 失败且 state>2 时记 hack(0x3c8)
    if (user->get_state() < 2)
    {
        return 0;
    }
    int seq = 0;
    if (!packet.get_int(seq))
    {
        return LineFunc(0x9bfd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char checksum[32];
    if (!packet.get_binary(reinterpret_cast<char*>(checksum), 0x20))
    {
        return LineFunc(0x9bff,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    char path[260] = { 0 };                            // ORIG rep stos 0x104 清零

    // ORIG 0x0820459a：checkCodeChecksum(checksum, path)；失败（返回 0）时
    // failKind = strstr(path,".exe.bak")?2:3 且 state>1 记 hack(0x3c8)。
    // [b28c-31] P0/N96 清绑：原 `register char* pp asm("eax")` 全域禁用 ⇒ 实参直写。
    // ORIG 0x0820459a 段实参序直证：先 `lea -0x138(%ebp),%eax; mov %eax,0x8(%esp)`（path，
    // 第二实参）后 `lea -0x34(%ebp),%eax; mov %eax,0x4(%esp)`（checksum）⇒ 4.1 从右往左；
    // 我方裸 `checksum` 形被 4.4 判为简单实参而先算，取 `&path[0]` 形复现右先。
    const bool checksumOk = user->checkCodeChecksum(
        *reinterpret_cast<CodeHackChecksum*>(&checksum[0]), &path[0]);   // [DispG1/31] P23 候选(A)
    user->IncChecksumAnswerCount();
    if (!checksumOk)
    {
        int failKind = 2;                                          // ORIG 先置 2
        // ORIG 0x82045d9：`call strstr; test %eax,%eax; sete %al; test %al,%al; je`。
        // [b28c-31] P0/N96 清绑：原 `register bool notBak asm("al")` 全域禁用。三形实测：
        // 具名 bool/uchar（两句式/初始化式）落槽 -0x9 且帧 0x150→0x160；内联表达式帧/槽
        // 全对（99/101，4.4 折 setcc 物化为 jne）⇒ 取内联形。
        // [b28d-solo-Dispatchers] P225/P295：ORIG 0x082045d9 `call strstr;
        //   test %eax,%eax; sete %al; test %al,%al; je`（4.1 sete 物化）⇒
        //   `while (expr == 0) { body; break; }` 复现（裸 if 被 4.4 折 `jne` 直跳）。
        while (dnf_std_strstr(path, ".exe.bak") == 0)              // strstr 命中保持 2
        {
            failKind = 3;
            break;
        }
        (void)failKind;                                            // ORIG 死存储保留
        if (user->get_state() > 1)
        {
            user->getHackAnalyzer()->addServerHackCnt(user,
                                                      static_cast<WongWork::ENUM_HACKTYPE>(0x3c8),
                                                      1, 0, 0);
        }
        return 0;   // ORIG 0x820463x 尾双岛：内层 return + 函数尾 return（技巧7）
    }
    return 0;
}