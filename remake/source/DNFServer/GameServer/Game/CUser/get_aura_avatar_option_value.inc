int CUser::get_aura_avatar_option_value(int idx)
{
    // ORIG：判空 = 4.1 bool 物化 `test eax;setne al;test;je`（技巧 22 形，指针
    // 结果免槽）；界检查单条无符号 `cmp $2; jg`（(unsigned)idx>2 吸收负数）；
    // 取值**二次** getCurCharacR call（结果直用 `add $0x4a0; mov 0x1(%eax,%edx,4)`
    // scaled 单级，scaled 分解残差记 N）。
    // b28 重构（P0/N96 去绑定）：界检查 `>=0 && <=2` 正向包裹 ⇒ 4.4 产
    // `cmpl $0,mem; js BAD; cmpl $2,mem; jg BAD` 双跳共享尾 -1 块（P222 族）。
    if (getCurCharacR() == 0)
        return -1;
    if (idx >= 0 && idx <= 2)
        return getCurCharacR()->m_auraValues[idx];
    return -1;
}