// ---- _checkValiedPassword(pwd, clientHash, cipher)  ORIG 0x08602bd0
// [P6S5 2026-09-11 本波落地 · P139 prett 轴] ORIG 0x08602bd0 引用
// `_ZZN8WongWork10CSimpleSSO20_checkValiedPasswordEPcPKcR9CRijndaelE19__PRETTY_FUNCTION__`
// （0x8cdd180 size 0x50 = "bool WongWork::CSimpleSSO::_checkValiedPassword(char*,
// const char*, CRijndael&)"）；旧译两处 LogManager::logFormat 第 3 参用**手写串**
// （逐字节相同）⇒ 我方不发 prett 符号。改成 `__PRETTY_FUNCTION__` 后 `nm` 实测符号
// 与 ORIG **同名同 size 0x50**，件读数 123/123（gap +0）不变、TU 四数 4/15/1/2/1
// **不变** ⇒ 保留（纯符号/串轴，零 codegen）。
bool WongWork::CSimpleSSO::_checkValiedPassword(char* pwd, char const* clientHash,
                                                CRijndael& cipher)
{
    // [2026-09-11 N4Free191to210.S6 复核] 本件由 DIFF 123/122（gap −1）推至
    // **123/123（gap 0）**，三处结构性偏差已闭环：
    //  ① paddedLen 三分支：ORIG 0x08602c06 为 `cmpl $0,-0xc; js <尾块>`
    //    （blk<0 前向跳）+ 内层 `idivl; test %edx,%edx; je <另一份 paddedLen=srcLen>`，
    //    故源形必须是 `if (blk >= 0) { if (len % blk != 0) … else … } else …`
    //    （旧 if/else-if/else 令 4.4 发 `jns` + then 落 fall-through = 镜像布局）。
    //  ② EH 区间：ORIG try 止于 `hexBuf[srcLen]=0`（Ghidra: 0x08602bfe..0x08602cbc，
    //    handler @0x08602ceb）⇒ strncmp/log 在 try 之外（旧形把日志块放进 try 内，
    //    使 catch handler 与块序错位）。
    //  ③ catch 的 logFormat 第一个 %s 实参是 **hexBuf**（ORIG 0x08602cfa
    //    `lea -0x98(%ebp),%eax; mov %eax,0x14(%esp)`），非空串；且 diff 分支
    //    `if (rc == 0) return true;` 前置（ORIG 0x08602ce7 `je <true 块>`，true 块
    //    地址 0x08602d51 < 日志块 0x08602d58）。
    // 残差 3 处（N7 封顶，实测）：Pad 调用首条实参装载次序、Encrypt 间接 call 目标
    //   （ORIG %edx vs 我方 %eax）、Binary2Hex 三实参求值序 —— 全为 4.4 寄存器分配产物。
    unsigned int srcLen = strlen(pwd);  // ORIG -0x14(%ebp)
    unsigned int paddedLen = 0;         // ORIG -0x10(%ebp)

    char hexBuf[132];  // ORIG -0x98(%ebp)（catch 分支同样读此缓冲，故放 try 外）
    try
    {
        // IMethod::GetBlockSize(this, NULL)（ORIG 0x08602bfe `call 080b4416` **真调用**；
        // 旧形按等价字段访问内联 ⇒ 我方 171 vs ORIG 123）。语义等价（未初始化返 0 /
        // 初始化返 m_blockSize），实现见 DBMW/Method.cpp:72；同域先例 Arad_Misc.cpp
        // 即用 `cipher->IMethod::GetBlockSize(NULL)` 形。
        int blockSize = cipher.IMethod::GetBlockSize(NULL);

        // [2026-09-11 结构修正] ORIG 0x08602c06..0x08602c3a 的分支布局为
        // `cmpl $0x0,-0xc(%ebp); js <B2>`（blk<0 时**前向跳**到尾部 “paddedLen=srcLen”
        // 块，0x08602c3c），fall-through = blk>=0 的余数判定；内层
        // `idivl; test %edx,%edx; je <B1>`（余数==0 前向跳到 0x08602c34 的另一份
        // “paddedLen=srcLen”）。⇒ 源形必须是 `if (blk >= 0) { 内层 } else { ... }`
        //（旧 if/else-if/else 让 4.4 发 `jns` + then 落 fall-through = 镜像布局）。
        if (blockSize >= 0)
        {
            if (static_cast<int>(srcLen) % blockSize != 0)
            {
                paddedLen =
                    (static_cast<int>(srcLen) / blockSize + 1) * blockSize;
            }
            else
            {
                paddedLen = srcLen;
            }
        }
        else
        {
            paddedLen = srcLen;
        }

        // IMethod::Pad(pwd, len, NULL)（ORIG 0x08602c42 起 `call 080b44a6` **真调用**；
        // 旧形把 Pad 体整体内联）。ORIG 守卫 = `if (-1 < blockSize)`
        //（Ghidra C：`if (-1 < local_10) { IMethod::Pad(…) }`）。
        if (blockSize >= 0)
        {
            cipher.IMethod::Pad(pwd, static_cast<int>(srcLen), NULL);
        }

        unsigned char encBuf[132];  // ORIG -0x120(%ebp) 区（块缓冲 0x84 使用域）
        // ORIG 经 vtable+0xc 虚调用（slot=Encrypt）
        cipher.Encrypt(pwd, reinterpret_cast<char*>(encBuf), paddedLen);

        // ORIG 0x08602cde 段 `mov -0x10(%ebp),%eax; add %eax,%eax; add $0x1,%eax;
        // mov %eax,-0x14(%ebp)` ⇒ **复用 -0x14 槽**（Ghidra C：`local_18 = local_14*2+1`）。
        // 我方旧形另开 `hexLen` 具名局部 ⇒ 本函数全帧槽位整体深 4（P0 律：原码无该局部）。
        srcLen = paddedLen * 2 + 1;
        DNFFLib::Binary2Hex(encBuf, static_cast<int>(paddedLen), hexBuf);
        hexBuf[srcLen] = '\0';
    }
    catch (...)
    {
        // [2026-09-11 结构修正] ORIG 的 EH 区间止于 `hexBuf[srcLen]=0`
        //（Ghidra：try from 0x08602bfe to 0x08602cbc，CatchHandler @ 0x08602ceb）
        // ⇒ strncmp/log 在 try 之外；且异常日志的第一个 %s 实参是 **hexBuf**
        //（ORIG 0x08602cfa `lea -0x98(%ebp),%eax; mov %eax,0x14(%esp)`），
        // 而非空串。
        LogManager::logFormat(
            1, "SimpleSSO.cpp",
            __PRETTY_FUNCTION__,
            0xe7,
            "_checkValiedPassword() exception server(%s), client(%s)", hexBuf,
            clientHash);
        return false;
    }

    // [2026-09-11 块序修正] ORIG 0x08602ce7 为 `je <true 块>` + `jmp <日志块>`，
    // 且 true 块（0x08602d51 `mov $1,%ebx`）地址低于日志块（0x08602d58）
    // ⇒ 源语句序为 “rc==0 先返回 true”，日志/return false 在后。
    if (strncmp(hexBuf, clientHash, 0x84) == 0)
    {
        return true;
    }
    LogManager::logFormat(
        1, "SimpleSSO.cpp",
        __PRETTY_FUNCTION__,
        0xee,
        "_checkValiedPassword() diff password server(%s), client(%s)", hexBuf,
        clientHash);
    return false;
}