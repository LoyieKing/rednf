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
    if (argc != 2)
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