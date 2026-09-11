// ============================================================================
// ImportExpertJobEtcScript（ORIG 0x896ae21）。
// 加载专家职业「.etc」脚本：填充等级上限 map 与分解机大奖概率 map。
// b28d-solo-CExpertJobList 判词（AE）：P164 for(;;)+头部 if(!ScanType) break（复现
// ORIG cond 块在 body 之前）+ P16/P292 bool 形早返 + P198① ok 预清两句式 + P171 照抄
// ORIG 死局部（-0x30 std::string 仅构造/析构、-0xc int 清零未读）+ 两处预清声明分组
// 在循环体首；189=189。
// ============================================================================
bool ImportExpertJobEtcScript(STExpertJobEtcScript& script, const char* path)
{
    // b28d-solo [P16/P292：ORIG 0x0896ae3c `xor $1; test; je <主体>` 为「bool 形早返」]。
    if (!loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
        return false;
    script.Clear();
    std::string section;
    // b28d-solo [P171 照抄 ORIG 死局部：-0x30 std::string（0x0896ae69 构造、
    // 仅 EH/尾段 0x08707be0 析构，全程未使用）与 -0xc int（0x0896ae6e 清零后未读）]。
    std::string unusedSection;
    int unusedLine = 0;
    // b28d-solo [P198①：ORIG 0x0896ae75 `movb $0x0,-0x31` 为 bool 局部预清 ⇒ 两句式]。
    bool ok = false;
    // b28d-solo [P164：ORIG 0x0896ae7c cond 块在 body 之前（4.4 对 while 恒旋转到尾部）
    // ⇒ for(;;) + 头部 `if (!ScanType) break;`，循环外统一 return true]。
    for (;;)
    {
        if (!ScanType(section, true))
            break;
        if (section == "[expertjob level limit]")
        {
            while (true)
            {
                // ORIG 0x0896aeb8/0x0896aebc：循环体首 `movb $0`+`movw $0` 预清 ⇒ 声明=0+赋值。
                unsigned char level = 0;
                short needLevel = 0;
                level = (unsigned char)ScanInt(&ok);
                if (!ok) break;
                needLevel = (short)ScanInt(&ok);
                if (!ok) return false;
                if (!script.m_levelMap.insert(
                        std::make_pair(level, needLevel)).second)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptExpertJob.cpp",
                        __FUNCTION__, 0x4a,
                        "import expertjob etc script - [expertjob level limit] index duplicated");
                    return false;
                }
            }
        }
        else if (section == "[disjointer big win rate]")
        {
            while (true)
            {
                unsigned char key = 0;
                unsigned char val = 0;
                key = (unsigned char)ScanInt(&ok);
                if (!ok) break;
                val = (unsigned char)ScanInt(&ok);
                if (!ok) return false;
                if (!script.m_disjointerBigWinRate.insert(
                        std::make_pair(key, val)).second)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptExpertJob.cpp",
                        __FUNCTION__, 0x60,
                        "import expertjob etc script - [disjointer big win rate] index duplicated");
                    return false;
                }
            }
        }
    }
    return true;
}