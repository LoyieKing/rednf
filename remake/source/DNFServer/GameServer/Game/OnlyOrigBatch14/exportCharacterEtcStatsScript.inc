void exportCharacterEtcStatsScript(FILE* fp, CharacterEtcStats* stats)
{
    if (!stats->m_category.empty())
    {
        fwrite("[category]\n", 1, 0xb, fp);
        for (unsigned int i = 0; i < stats->m_category.size(); ++i)
        {
            // [b28d-solo-OnlyOrigBatch14] P0：ORIG 0x08a10136 段
            // `mov (%eax),%eax; mov %eax,-0x10(%ebp); lea -0x1c(%ebp),%eax;
            // mov -0x10(%ebp),%edx; mov %edx,0x4(%esp)` ⇒ 原码有具名
            // ENUM_CATEGORY 局部（槽往返），非内联 cast 直传。
            ENUM_CATEGORY cat = (ENUM_CATEGORY)stats->m_category[i];
            std::string s = getObjectCategoryString(cat);
            fprintf(fp, "`%s`\n", s.c_str());
        }
        fwrite("[/category]\n", 1, 0xc, fp);
    }
    // [T1b 槽复用配方·负结论（本件不适用）] 试把第二循环变量改独立具名（k）：
    // 4.4 仍给它 -0x10（复用第一循环槽位），槽图与改前逐值相同、四数不变
    // ⇒ 4.4 的槽复用非「同名」所致（lifetime 复用），本件维持 N82 封顶。
    int i = 0;
    int j;
    for (i = 0; i <= 3; ++i)
    {
        if (stats->m_moduleDamageRate[i] != 0.0f)
            break;
    }
    if (i != 4)
    {
        fwrite("[module damage rate]", 1, 0x14, fp);
        // [b28d-solo-OnlyOrigBatch14] j 已在函数头声明；for-init 单点赋值
        // （4.4 对 `for (int j = 0;…)` 多发一条 `movl $0,-0x18`，ORIG 仅一条）。
        for (j = 0; j <= 3; ++j)
        {
            fprintf(fp, " %.3f", (double)stats->m_moduleDamageRate[j]);
        }
        fputc(0xa, fp);
    }
