// b28d-solo-InitScriptProducers_1: DIFF→AE（0x1e0=0x1e0）。修：5 个独立 if 改为
// if/else-if 链并删显式 continue —— ORIG 每 case 尾 `je <独立 nop;jmp 循环头>`
// （0x08911249/4c/4f/52/55）是 4.1 对「else-if 链出口」的独立中继；4.4 对显式
// continue/goto 恒 jump-thread（5 标签串 135/单标签 140），而 else-if 链直落
// 逐岛复现（144 条 vs ORIG 143，AE 归一后全等）。
// ORIG T 0x0891120c。五命名段任意顺序；[capsule limit lv] 读入成功后继续主循环，
// EOF 返回 0；各字段断流按 ORIG 行号常量返回。
int ImportGrowthCapsuleScript(const char* path, stGrowthCapsuleScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x2ba2;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto failEof;                        // ORIG 失败块 0x089113b5 落在循环体之后

        if (token == "[gage percent]")
        {
            script.m_gagePercent = ScanInt(&ok);
            if (!ok) return 0x2bb0;
        }
        else if (token == "[max gage exp]")
        {
            script.m_maxGageExp = ScanInt(&ok);
            if (!ok) return 0x2bb6;
        }
        else if (token == "[capsule id]")
        {
            script.m_capsuleId = ScanInt(&ok);
            if (!ok) return 0x2bbc;
        }
        else if (token == "[capsule exp]")
        {
            script.m_capsuleExp = ScanInt(&ok);
            if (!ok) return 0x2bc2;
        }
        // b28d-lane25：P167 差集实证 ORIG 调 operator==（非 !=）；正向包裹。
        else if (token == "[capsule limit lv]")
        {
            script.m_capsuleLimitLv = ScanInt(&ok);
            if (!ok)
                return 0x2bc8;
        }
    }
failEof:
    return 0;
}
