int ImportTileScript(STTileScript* script, const char* path)
{
    // [b28d-solo-OnlyOrigBatch14；gap 5→0 / DIFF→AE 本波收敛（208=208）]
    // 旧判词 loadRDARScriptFile 首参有误：ORIG 0x089dcbc5 实证为空串，已改
    // `loadRDARScriptFile("", path)`。本波三条真缺口（直方图 jmp-1/nop-3/xor-1）：
    //   (a) 主 token 循环 ORIG 089dcc3a-3f `xor $1,%eax; test %al,%al;
    //       jne <退出>` = 负极性 `for(;;){ if(!ScanType(token,true)) break; }`
    //       （4.4 对 while(ScanType) 恒发 test+jne 正极性，少 xor）；
    //   (b) ORIG 089dcc21-26 两个空 else 占位岛（`jmp 循环头; nop; jmp 循环头;
    //       nop`，4.1 if-join 岛）随 (a) 的 break 落点与 if/else 结构自然复现；
    //   (c) ORIG 089dce41 的 break 出口 nop 随 (a) 复现。
    // 另修 P0 槽合并：ORIG 089dcd80 `movl $0,-0xc` 与 089dcdcf `movl $1,-0xc`
    // 复用同一槽 ⇒ 原码是**同一个** `int i`（死初始化 + for(i=1;…)），
    // 拆成 rowIdx+i 两个局部会多占 4B 使全帧槽深移（旧判词 N98 槽序残差即此）。
    // 收敛后 208=208 助记符序列全等、槽号逐条对齐（AE）。
    if (!loadRDARScriptFile("", path))
    {
        printf("TILE LOAD FAIL! [%s]\n", path);
        return 0;
    }
    script->Clear();

    // ORIG 089dcc3f/089dcc47：Clear 后即写 bool ok=false、int n=0 ——
    // 两临时为**函数级**（[pass type] 段共用其槽 -0x11/-0x18）。
    bool ok = false;
    int n = 0;
    std::string token;
    std::string str;
    // [gap-5 修复] ORIG 089dcc35-cc3f：`call ScanType; xor $1,%eax; test %al,%al;
    // jne <退出>` = 负极性 `if(!ScanType) break`（4.4 对 while(ScanType) 恒发
    // test+jne 正极性，少 xor 一条）；退出点 089dce41 的 nop 为 4.1 break 出口占位。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        if (token == "[IMAGE]")
        {
            std::pair<std::string, int> entry;
            ScanStr(&entry.first);
            entry.second = ScanInt((bool*)0);
            script->m_actions.push_back(entry);
        }
        else if (token == "[tile type]")
        {
            ScanStr(&str);
            if (str == "[basic]")
            {
                script->m_flagC = 0;
            }
            else if (str == "[extended]")
            {
                script->m_flagC = 1;
            }
        }
        else if (token == "[img pos]")
        {
            script->m_field1c = ScanInt((bool*)0);
        }
        else if (token == "[pass type]")
        {
            script->m_collisions.clear();
            // ORIG 089dcd80：clear 后即 `movl $0x0,-0xc(%ebp)`，且 089dcdcf
            // `movl $0x1,-0xc(%ebp)`（for 初值）**复用同一槽** ⇒ 原码是同一个
            // 具名 int i（死初始化 + for(i=1;…) 复用），不是两个局部（P0）。
            int i = 0;
            (void)i;
            // ORIG 089dcd8d：row vector<int> 在循环**外**构造一次（跨轮
            // 复用，loop top 089dcd92 直落 ScanInt）；行内 089dcdfb 显式
            // row.clear()；首个 ok 检查 fail → je 089dce1b 跳统一 dtor 岛
            // （+0x265 nop;D1;jmp 尾），循环体不回跳 dtor。
            std::vector<int> row;
            for (;;)
            {
                n = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                row.clear();
                row.push_back(n);
                for (i = 1; i <= 0xd; ++i)
                {
                    row.push_back(ScanInt((bool*)0));
                }
                script->m_collisions.push_back(row);
            }
        }
    }
    return 1;
