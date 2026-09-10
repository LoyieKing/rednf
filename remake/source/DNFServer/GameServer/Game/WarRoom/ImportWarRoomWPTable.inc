int ImportWarRoomWPTable(stBloodWarGrowTable* table)
{
    // [b28d-solo-WarRoom-4] ORIG 0x08902543：126/128（gap 2）⇒ **128/128（gap 0）**。
    //   本轮两旋钮（P231 + P223 多岛）：
    //   ① 循环条件变量 `register bool again` → **`register int again`** 并写 0/1
    //      （ORIG 08902669/08902688 `mov $0/$1,%ebx`、089026af 需 32 位 test）
    //      且 do-while 条件写 `again != 0` ⇒ 消 4.4 的 `setne %al; test %al,%al`
    //      两步物化，改出 ORIG 的 `test %ebx,%ebx` 直判。
    //   ② do 体后与 else 分支各发一个 goto（LOOP_TAIL / ELSE_ISLAND 物理相邻），
    //      复现 ORIG 两个不同落点（089025ad nop+089025ae jmp 与 089025b0 nop）。
    //   残余（N 封顶，附 ORIG 指令形）：ORIG 089025ab/089025ae 两处 `jmp 头` 与
    //   089025b0 nop 岛；4.4 对单源尾岛恒 jump-thread（本轮已试双标签串/删 else 直落/
    //   同层 continue 三形，P223 单源负例）⇒ 我方少 1 个 jmp+nop、并在 do-while 尾
    //   多 2 条中继。语义与 ORIG 一致（p167 CLEAR 16/16，MISSING 仅 C1/C2 镜像）。
    if (!loadRDARScriptFile("", "Etc/WarRoomWPTable.tbl"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0xdb8,
                              "CRITICAL ERROR : \xc0\xef\xbf\xaa \xc4\xab\xbf\xee\xc5\xcd \xbd\xba\xc5\xa9\xb8\xb3\xc6\xae \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return 0;
    }
    std::string token;
    bool ok = false;
    // [b28d-solo-WarRoom P283/P164] ORIG 0x0890259x 循环头为
    //   `call ScanType; xor $0x1,%eax; test %al,%al; jne <body>`（否定跳转）+
    //   两处 nop 中继岛；4.4 对 while(cond) 恒正向 `test;je` ⇒ while(true)+break 试翻。
    while (true)
    {
        if (!ScanType(token, true))
            break;
        // [b28d-solo-WarRoom P223] ORIG 0x089025e4 `je 0x089025ad`（nop; jmp 循环头）
        //   = else 占位岛；4.4 对纯正向 if 恒 jump-thread ⇒ 空 else + goto 尾空
        //   标签复现（保留 operator== 调用点，P167）。
        if (token == "[WP TABLE]")
        {
            // [b28d-solo-WarRoom P231] ORIG 0x08902669/0x08902688 两臂 `mov $0/$1,%ebx`
            //   + 0x089026af `test %ebx,%ebx; je` ⇒ 具名循环条件变量落在 %ebx
            //   （纯 register 存储类提示，无 asm 绑定，P0 合规）。
            // [b28d-solo-WarRoom P223] ORIG 循环头双岛：089025ad nop + 089025ae
            //   jmp HEAD（if 假分支落点）、089025b0 nop（do-while again==0 落点）。
            //   试 continue 形（同层 continue + 循环尾自然 fall）复现双岛。
            register int again;
            do
            {
                std::vector<std::pair<int, int> > row;
                for (int i = 0; i < 5; ++i)
                {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok)
                        break;
                    p.second = ScanInt(&ok);
                    if (!ok)
                        break;
                    row.push_back(p);
                }
                if (!ok)
                    again = 0;
                else
                {
                    if (table != 0)
                        table->push_back(row);
                    again = 1;
                }
            } while (again != 0);
            // [b28d-solo-WarRoom-4 判据④/P223 多岛] ORIG 循环尾两个物理相邻但不同
            //   地址的落点：+0x6a（operator== 假分支）与 +0x6d（do-while again==0）。
            //   两个**不同源**的 goto 跳物理连续尾标签 ⇒ 4.4 不 jump-thread（P223 可造形）。
            goto LOOP_TAIL;
        }
        else
        {
            goto ELSE_ISLAND;
        }
ELSE_ISLAND:
        ;
LOOP_TAIL:
        ;
    }
    return 1;
