int StageTimeLineParameter::importStageTimeLineParameter(const char* path)
{
    // b28d-35: DIFF→N 封顶（N82 帧/槽 + N78 内联清零族）。ORIG @0x08899f26。
    std::string token;
    int unused18 = 0;                   // ORIG -0x18 死存储（保留以对齐栈帧）
    bool unused11 = false;              // ORIG -0x11 死存储（同上）

    reset();
    if (!loadRDARScriptFile("", path))
        return 0;                       // ORIG 08899f69: ebx=0

    for (;;)
    {
        if (!ScanType(token, true))     // ORIG 08899f88
            return 0;                   // 08899ff4..ffa: ebx=0

        if (token == "[summon object]")
        {
            int r = importSummonObject();
            if (r != 0)
                return r;
            continue;                   // ORIG 08899f73 → 循环头
        }
        if (token == "[action define]")
        {
            int r = importActionDefine();
            if (r != 0)
                return r;
            continue;                   // ORIG 08899f79 → 循环头
        }
        // 未知 token：直接回循环头（ORIG 无额外分支）。
    }
