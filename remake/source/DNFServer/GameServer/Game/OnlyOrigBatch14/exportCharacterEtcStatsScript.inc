// ORIG T 0x08a100f3 _Z29exportCharacterEtcStatsScriptP8_IO_FILEP17CharacterEtcStats。
// [b28d 判词·N 封顶（N82 槽复用 + N85 x87）] 本波修：①第一循环加具名
// ENUM_CATEGORY cat 局部（P0：ORIG 0x08a10151 槽往返 + 回读压参）。
// [H10-b14wave14 本波·槽序归零] 两处修复（均 ORIG 逐地址实证）：
//  ①`int i = 0;` → `int i;`：消我方多发的一条 `movl $0x0,-0x18(%ebp)`
//    （ORIG 0x08a101f3 段仅一条 for-init 写；旧形声明初始化 + for-init 各写一次）
//    ⇒ our_insns 118→117、ndiff 该 hunk 消失（P191/P328⑤）。
//  ②`int j;` 由函数头移入 `if (i != 4)` 块内（for-init 单点赋值保留）：4.4 按
//    内层块后声明占浅槽分配 ⇒ j@-0xc（ORIG 0x08a1024e）、i1@-0x14（ORIG
//    0x08a10134）、cat@-0x10（ORIG 0x08a10151）、i2@-0x18（ORIG 0x08a101f3）
//    四处与 ORIG 逐槽相同 ⇒ 原「N82 槽复用」两 hunk 全消。
//  p167 CLEAR（14=14）。残余唯一差异 = N85 x87 lowering（ORIG 0x08a10215 段
//  `fucompp;fnstsw %ax;sahf` vs 4.4 `fucomip %st(1),%st;fstp %st(0)`）。
// [b14 A/B 实测·路由不提] N85 x87 lowering：默认 mne_delta =
// {sahf:-1,fucompp:-1,fnstsw:-1,fucomip:+1,fstp:+1,movl:+1}（ndiff 55 / gap 0）；
// 追加 `-march=i486 -mtune=generic`（Main 判据，须同带 mtune）后 x87 差异全消，
// 但**新引入 1 条 movl**（gap 0→-1、ndiff 52），本件仍 DIFF，且整 TU 四数逐项
// 不变（396+800=1196 / near 0 / diff 13 / missing 197；30 个函数指令形变化、
// 0 件翻转）⇒ 依 Main 口径（仅整 TU 净收益才提路由）不提。
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
