// ORIG W 0x08ac2eca sz 0x29e 忠实转写（成员版；结构与 throw/catch/logFormat
// 路径全保留）：外层 for(;;)：seek 循环丢 token 直至 "[allow unicode range]"；
// body 循环里 "[/allow unicode range]" → out=tmp 并重回 seek；"[range]" →
// ScanInt 左右值入 tmp；其它 token 忽略。任一 ScanType 失败/越界抛 const char*，
// 被 catch 记日志后返 false。
// [b28d-D9] 修复：P222 正向包裹（token=="[range]"）+ logFormat 5 参（去 "%s"）后 AE 172/172（ORIG 0x08ac2eca sz 0x0000029e）
// [b28d-solo-RestrictNameDeps] verdict=AE（compare_tu clean 口径 172/172 逐条相等）。
//   清单旧值 NEAR/ndiff13 与 game_func_report 的 DIFF 均为 P264 假报（本件无
//   class_func_reports 报告 ⇒ md_text=='' 分支不做 AE 归一）：.o 内 .rodata 串/
//   _ZTIPKc 立即数尚未重定位显示为 0，且 4.4 在 ranges 构造前多一条循环对齐 nop
//   （strip_nop_padding 剥离后两侧等长）。ORIG sz 0x29e / 我方 0x29f 同因。
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
}