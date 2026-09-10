bool RestrictInputtingNameData::importUnicodeRangeData(
    const char* path, std::vector<std::pair<int, int> >* out)
{
    // [b28c-L15 真语义 bug] 权威签名 loadRDARScriptFile(directory, path)；ORIG
    //   0x08ac2ecf：`mov 0xc(%ebp),%eax; mov %eax,0x4(%esp)`（path→第 2 参）
    //   `movl $0x8e2e1a0,(%esp)`（空目录→第 1 参）⇒ 原码为 ("", path)。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;                       // ORIG +0x53（外层循环前构造一次）
    for (;;)
    {
        try
        {
            // seek "[allow unicode range]"（ORIG jmp 回 +0x61）
            for (;;)
            {
                if (!ScanType(token, true))
                    throw "[allow unicode range] 태그가 아님\n";
                if (token == "[allow unicode range]")
                    break;
            }
            std::vector<std::pair<int, int> > ranges;   // ORIG +0xc1 每轮新建
            for (;;)
            {
                if (!ScanType(token, true))
                    throw "태그가 와야 할 자리에 다른 값이 파싱 됨\n";
                if (token == "[/allow unicode range]")
                {
                    *out = ranges;                  // ORIG vector::operator= 复制
                    break;                          // 重回 seek（ORIG jmp +0x61）
                }
                // [b28d-D9] ORIG 0x08ac2f3d 为 `call operator==; je <range 处理体>`
                // （正极性、命中直落处理体），非 `operator!=`+continue 反形（P222 正向包裹）。
                if (token == "[range]")
                {
                    int left = 0;
                    int right = 0;
                    if (!ScanInt(&left))
                        throw "range의 left값을 읽어 오다 실패\n";
                    if (!ScanInt(&right))
                        throw "range의 right값을 읽어 오다 실패\n";
                    ranges.push_back(std::make_pair(left, right));
                }
            }
        }
        catch (const char* message)
        {
            // [b28d-D9] ORIG 0x08ac30c2 只压 5 个实参（0, file, func, 0x48, message）——
            // message 直接作 logFormat 的格式串，无 "%s" 中间实参（我方多压 1 条
            // `movl $"\"%s\"",0x10(%esp)` 使 message 落到 0x14(%esp)）。
            LogManager::logFormat(0, "../../RestrictInputtingName.cpp",
                                  __FUNCTION__, 0x48, message);
            return false;
        }
    }
    // ORIG 无正常退出路径（正常返回仅经 catch→return false）。
    return false;
