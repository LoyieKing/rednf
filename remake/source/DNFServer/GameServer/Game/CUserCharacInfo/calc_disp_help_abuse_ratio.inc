// [S1Charac 2026-09-11 复核] 68/68、助记符逐条全等；真残差=匿名 8B 转换临时基准比 ORIG
// 低 8B（我方 -0x38/-0x34、cw -0x3a/-0x3c、除数槽 -0x4c；ORIG -0x30/-0x2c、-0x32/-0x34、
// -0x3c）连带帧 0x68↔0x58 ⇒ 4.4 聚合临时优先占深槽的分区排布（N95），无死局部可删。
// 本波 §6.5 串核查：本函数无 rodata 串引用（报告内那条字符串系解析噪声）⇒ CLEAR。
// [b28 BM8 P210] 定义序移到全部同 TU callee（get_level_up_exp/两 getter）之后：
// 4.4 按已见 callee 帧继承 outgoing，缩帧向 ORIG 0x58 靠拢；原「fildll 槽对齐
// 恒形 N」旧判词系未试 P210 前置。实测 P210 前置帧不变（仍 0x68）；具名 ull
// 局部形反劣 80 insn（reload 链），回滚匿名临时形（68=68 助记符全等）。
// [b28 BM8 判词 N82附/N95] 残差全为槽号平移：4.4 聚合(8B)临时优先占深槽且基址
// 比 4.1 低 8B（-0x38 vs -0x30，连带 cw -0x3a/-0x3c vs -0x32/-0x34、除数槽
// -0x4c vs -0x3c），帧 0x58↔0x68 差 16B——4.1 生命周期穿插 vs 4.4 分区排布，
// 无源形可控（ndiff 28 封顶）。
// [b28d-solo-CUserCharacInfo 复核] ORIG 符号实为非 const _ZN15CUserCharacInfo26
// calc_disp_help_abuse_ratioEi（P107，清单给的 _ZNK 版 ORIG 不存在）；68/68 助记符
// 全等，-0x14/-0x10/-0xc/-0x1c/-0x18 槽全同，仅 8B 聚合临时对齐（16 vs 8）连带
// cw/除数槽与帧差 ⇒ 维持 N82附/N95 封顶。
// [b28d-solo-CUserCharacInfo P210 实验] 前移 get_charac_level 定义（唯一不可见
// callee）后本件帧仍 0x68（无变化，已回退）⇒ 帧差非 callee 帧继承所致。
// [b28d-solo-CUserCharacInfo 复核] 68=68 助记符逐条全等；残差=ull 转换匿名 8B 临时
// 与 cw/除数槽相对 ebp 整体下移 8（-0x30/-0x32/-0x3c → -0x38/-0x3a/-0x4c）及帧
// 0x58↔0x68：4.4 对 (double)uint 的匿名 8B 临时优先占深槽（N95/N82附）；本件无死
// 局部可删（P232）、撑帧只会加深（P190 反向）⇒ 维持 NEAR 封顶（p167 CLEAR）。
// [P8 2026-09-11 第二遍回扫] 本波新钥匙复核：n1_v=NEAR 68/68（与旧一致）、
//   P139 prett 无缺口/无 size 差、strall 串 CLEAR、p167 call 差集 CLEAR
//   ⇒ 旧判词 N82附/N95 维持，无可翻案旋钮（未新增改动）。
int CUserCharacInfo::calc_disp_help_abuse_ratio(int param)
{
    int levelUpExp = get_level_up_exp(get_charac_level());
    float helpRate =
        (float)((double)getCurCharacDungeonHelpAbuseExp() / (double)levelUpExp);
    int ratio = (int)((float)getCurCharacDungeonHelpAbuseRatio() + 1000000.0f * helpRate);
    int result = 0;
    if (get_charac_level() + param > 0)
        result = ratio / (get_charac_level() + param);
    return std::min(1000000, result);
}