// check_argv(int, char**) —— ORIG 0x829ea5a（0x420 字节，逐指令转写）。
// [b28d-28] DIFF | ORIG 0x0829ea5a | N82：槽/寄存器分配差（263=263）。
// [b28d-solo-GameMain] N98 封顶 | 261/263 | p167 58/58 CLEAR | 修法：shellsKey 去
// 具名缓存（P0 表达式直用）+ P232 `char framePad[0x10]` 撑帧（0x424→**0x434 与
// ORIG 全等**）+ P244 op 声明/赋值分离提前（槽 -0x14→-0x18）；残差仅 op 槽
// -0x18 vs ORIG -0x1c（4.4 紧凑分配 vs 4.1 在 -0x18/-0x14 预留两槽，未使用标量
// 4.4 不占槽 ⇒ N98 不可翻；再补 int dead 反使帧 0x444 劣化，已回退）。
// [A18 本波] **263=263（gap 0）、ndiff 2**（旧 gap -13）。三修：
//  ①散布 `return 0;` 代 `goto L_RET0`（P325 反向）：4.4 对共享标签恒发
//    单一尾块（jmp 3acd），改散布 return 后逐点 `mov $0,%eax; jmp <epilogue>`，
//    与 ORIG 0x0829ea6a..0x0829ee6e 十八处就地 mov+jmp 全等；
//  ②stop/suspend 两分支保留 `goto L_RET0_TAIL;`（函数尾标签 + fall-through）：
//    ORIG 唯此两点共享 +0x41b 中继块（`jmp 0829ee75`），其余点就地 ⇒ 混合形；
//  ③`total = (int)((unsigned)total + (unsigned)strlen(...))` 双 cast 链
//    （P211/P274 异型化）：复现 ORIG `mov -0x14,%edx; add %edx,%eax;
//    mov %eax,-0x14` 读-加-写三段（裸 `total + strlen` 4.4 恒折 `add %eax,-0x14`）。
// 残差 = **N111**：ORIG 0x0829eb6f `test %al,%al; jmp 0829eb87` 为无入口死块
// （FAIL 段 0x0829eb71..0x0829eb82 永不可达、Ghidra C 亦无该分支），4.4 恒
// 条件化（我方 `je`，语义正确且可达）⇒ 合法 C++ 不可造，1 条指令差封顶。
bool check_argv(int argc, char** argv)
{
    // P244：op 提前声明（赋值仍在使用点）。
    const char* op;
    if (argc <= 2)
        return 0;

    G_CEnvironment()->set_file_name(argv[1]);
    if (argc == 4)
        G_CDataManager()->SetScriptPacks(argv[3]);

    if (!G_CEnvironment()->load())
        return 0;
    G_CEnvironment()->adjust_base_script_directory();

    if (*G_CEnvironment()->get_exchange_server_ip() == '\0')
    {
        std::puts("Exchange Server IP load FAIL, please check cfg file");
        return 0;
    }
    if (G_CEnvironment()->get_exchange_server_port() == 0)
    {
        std::puts("Exchange Server PORT load FAIL, please check cfg file");
        return 0;
    }

    // server_shells_key 校验（env+0x318 缓冲区，ORIG 原样裸偏移）
    int ncl_check_key(char* key);  // NclKeyTools.cpp（ORIG 0x8572cb5）
    bool keyOk;   // 声明/赋值分离：goto L_RET0 不得跨初始化（C++ 规则）
    // P0：ORIG 无 shellsKey 具名局部（实参表达式直用 `mov %eax,(%esp)`）。
    keyOk = ncl_check_key(
        reinterpret_cast<char*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x318)) != 0;
    // ORIG printf 再独立 G_CEnvironment()+0x318（P183 重物化，勿缓存 shellsKey）。
    std::printf("lls_key :: %s\n",
                reinterpret_cast<char*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x318));
    // ORIG 0x829eb66-0x829eb6f：keyOk 存栈后 movzbl/xor/test 紧跟无条件
    // jmp 0x829eb87——FAIL 路径（0x829eb71）无跳转引用，系 -O0 死块（P184），
    // 源码保留 if(!keyOk){puts(FAIL);return 0;} 以发射该 puts。
    if (!keyOk)
    {
        std::puts("[!] Server key Check FAIL");
        return 0;
    }
    std::puts("[!] Server key Check OK");

    op = argv[2];
    if (std::strcmp(op, "start") == 0 || std::strcmp(op, "nofork") == 0 ||
        std::strcmp(op, "start_all") == 0)
    {
        if (!check_cfg_file())
        {
            std::printf("There is not %s config_file\n",
                        G_CEnvironment()->get_file_name());
            return 0;
        }
        if (check_pid_file())
        {
            std::printf("Process(%s) is already operated.\n",
                        G_CEnvironment()->get_channel_name());
            return 0;
        }
        rdar_init(argv);
        if (std::strcmp(op, "start") == 0)
            G_CEnvironment()->set_operation_type(
                static_cast<eStartArgumentType_t>(0));
        else if (std::strcmp(op, "nofork") == 0)
            G_CEnvironment()->set_operation_type(
                static_cast<eStartArgumentType_t>(2));
        else if (std::strcmp(op, "start_all") == 0)
            G_CEnvironment()->set_operation_type(
                static_cast<eStartArgumentType_t>(1));
        return 1;
    }
    if (std::strcmp(op, "stop") == 0)
    {
        GetInstanceShutdowManager()->SendShutdownSignal();
        // ORIG 0x0829ed0b：stop/suspend 两分支共享 +0x41b 中继块
        // （`mov $0x0,%eax` + fall-through epilogue）——与其它失败点的就地
        // `mov $0,%eax; jmp` 不同 ⇒ 仅此两点用 goto 到函数尾标签。
        goto L_RET0_TAIL;
    }
    if (std::strcmp(op, "suspend") == 0)
    {
        send_suspend_signal();
        goto L_RET0_TAIL;
    }
    if (std::strcmp(op, "test") == 0)
    {
        G_CEnvironment()->set_operation_type(static_cast<eStartArgumentType_t>(3));
        return 1;
    }
    if (std::strcmp(op, "mess") == 0)
    {
        int total = 0;
        int ret;
        char buf[0x400];
        std::memset(buf, 0, sizeof(buf));
        for (int i = 3; i < argc; ++i)
        {
            std::strcat(buf, argv[i]);
            // ORIG 0x0829ede0：`mov -0x14,%edx; add %edx,%eax; mov %eax,-0x14`
            // 读-加-写三段（`+=` 在 4.4 恒折 `add %eax,-N(%ebp)` 两段）。
            total = static_cast<int>(static_cast<unsigned int>(total) +
                                     static_cast<unsigned int>(std::strlen(argv[i])));
            std::strcat(buf, " ");
            ++total;
        }
        // ORIG 0x0829ee28：`mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp); jns`
        // ⇒ 具名 int ret 局部（P0 槽往返），非直测返回值。
        ret = send_chat_msg(buf, total);
        if (ret < 0)
            std::puts("[ERROR]Fail to Send Notice Message");
        return 0;
    }
    if (std::strcmp(op, "info") == 0)
    {
        printGameServerInfo(false);
        return 0;
    }
    // ORIG 0x0829ee75：全部 return 0 路径共享函数尾 `mov $0x0,%eax` 块
    // （P325 多 return 共享尾出口）。
    return 0;
L_RET0_TAIL:
    return 0;
}