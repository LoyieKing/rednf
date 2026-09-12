// [b28d-32 N(4.1 死块): ORIG cmp+无条件 jmp 跳过 argc 错误块（死代码），4.4 恒条件化并 DCE 死块，合法 C++ 不可造（探针 /tmp/lane32_probe1.cpp） | ORIG 0x0817de00]
// [S5AradDisp 2026-09-11 复核维持 N：直接读 ORIG 原始字节 —— 0x0817de2e `83 7d 08 02`
//  cmpl $0x2,0x8(%ebp) 后跟 0x0817de32 `eb 1b` **jmp** 0x0817de4f（跳过 0x0817de34..
//  0x0817de4d 错误块；全函数无任何跳转指向 0x34 ⇒ 该块不可达 = 死块）。
//  本波探针 /tmp/probe_ai.cpp（v1 if(argc!=2){} / v2 if(argc==2){}else{} / v3 switch(argc)）
//  同旗标同头集实测恒发 `74 1b` **je**（cmp 后必条件化），合法 C++ 在 -O0 下不可能产出
//  「cmp + 无条件 jmp + 保留死块」⇒ 维持 N。我方 gap 0（orig 42 / our 42），
//  残差仅该 1 处 opcode（je vs jmp）+ .o 重定位占位（§6.3 假阳性）。]
int AradAppSystem::AradAppInit(int argc, char** argv)
{
    std::cerr << "[!] Server Starting...\n";
    srand(time(0));
    // [SemA05 2026-09-12 · Main 裁决「跟随 ORIG 运行时可观察行为」（§P368⑨ 运行时热补族）]
    // ORIG 0x0817de2e `83 7d 08 02` = cmpl $0x2,0x8(%ebp)，紧随 0x0817de32 `eb 1b` = **无条件 jmp**
    // 0x0817de4f，跳过 0x0817de34..0x0817de4d 的 argc 错误块（打印 "[!] Check Args Fail!!\n" + return 0）；
    // 全函数无任何跳转指向 0x0817de34 ⇒ 该块**不可达（死块）** ⇒ ORIG 二进制的可观察行为 = **不检查 argc**。
    // 不可造性：4.4 -O0 对 `if` 恒发条件跳转（探针 /tmp/probe_ai.cpp v1 `if(argc!=2){}` / v2
    // `if(argc==2){}else{}` / v3 `switch(argc)` 同旗标同头集实测恒发 `74 1b` je；b28d-32 探针
    // /tmp/lane32_probe1.cpp 同结论）⇒ 合法 C++ 在 -O0 下不可能产出「cmp + 无条件 jmp + 保留死块」。
    // ⇒ 按 ORIG 行为删除该检查，原源码形保留备查（下方注释）。
    // 【运行风险 · 即 ORIG 的真实行为，非我方缺陷】argc≠2（如 argc==1）时本函数**不再**返回 0，
    // 而是继续执行 open_log(argv[1])（argv[1] 可能为 NULL）—— 与 ORIG 二进制逐位一致。
    //   if (argc != 2)
    //   {
    //       std::cerr << "[!] Check Args Fail!!\n";
    //       return 0;
    //   }
    (void)argc;   // 保留形参（mangled/签名不变），ORIG 行为不使用其做校验
    // [R4a20-v4 #16/P337 2026-09-12] 上述被热补跳过的错误块在 ORIG 镜像中**仍然存在**
    //   （0x0817de34..0x0817de4d：第 2 条 `cerr << "[!] Check Args Fail!!\n"` + return 0），
    //   而原实现直接删除该块 ⇒ P167 缺 1 个 `_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc`
    //   调用点 + 缺 `[!] Check Args Fail!!\n` 串 + gap −7（42 vs 35，本轮第四轮核验实测）。
    //   按用户口径「完全复刻 ORIG（含热补后行为）」+ P337 配方（恒 0 volatile 守卫）保留块与调用点：
    //   守卫恒假 ⇒ 运行时可观察行为与 ORIG 逐位一致（不校验 argc）。
    volatile bool deadGuard = false;
    if (deadGuard)
    {
        std::cerr << "[!] Check Args Fail!!\n";
        return 0;
    }
    if (!open_log(argv[1]))
        return 0;
    if (!HistoryLog::Init())
        return 0;
    if (!script_load())
        return 0;
    return 1;
}