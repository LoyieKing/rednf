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
    // [T1b 历史注记] 曾试把第二循环变量改独立具名（k）：4.4 仍给 -0x10（复用
    // 第一循环槽位）。本波以「j 移入 if 块」翻转块内外深度关系后槽序已对齐
    // ORIG（见函数头判词②），旧「N82 封顶」结论作废。
    int i;
    for (i = 0; i <= 3; ++i)
    {
        // [P1T1 本波 A/B·旗标申请] ORIG 0x08a10202-0x8a10211 `fld [eax+edx*4+0xc];
        // fldz; fucompp; fnstsw %ax; sahf; jne; jp`（x87 4.1 lowering）；4.4.6 恒发
        // `fucomip %st(1),%st; fstp %st(0)`（P230/N85 族）。实测给本 TU 叠
        // `-march=i486 -mtune=generic`（同带）⇒ 本件翻 AE（119=119），且非 weak 件零
        // 劣化；但同 TU 13 个 weak 模板实例（std::uninitialized_copy）转 DIFF ⇒
        // TU 四数 396/799/1/12 → 396/786/1/24（劣化）。⇒ 需 Main 按 weak 口径裁决；
        // 函数级 `#pragma GCC target("arch=i486"/"no-sse")` 在 4.4.6 实测无效
        //（/tmp/p1/probe_x87.cpp f1/f2/f3 三形全 fucomip）⇒ 本件维持 DIFF。
        if (stats->m_moduleDamageRate[i] != 0.0f)
            break;
    }
    if (i != 4)
    {
        fwrite("[module damage rate]", 1, 0x14, fp);
        int j;
        // [H10-b14wave14] j 在块内声明（占浅槽 -0xc，ORIG 0x08a1024e）；for-init
        // 仍单点赋值（4.4 对 `for (int j = 0;…)` 多发一条 `movl $0,-0x18`）。
        for (j = 0; j <= 3; ++j)
        {
            fprintf(fp, " %.3f", (double)stats->m_moduleDamageRate[j]);
        }
        fputc(0xa, fp);
    }
}
