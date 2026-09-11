// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08678526 | _ZN5CUser13CheckMoveTownEi
// 2026-09-11 CuA 探针终判（/tmp/P5/p_cmp probe_cmp{,2}.cpp）：4.4 对 **(int 参数 + `(unsigned)` cast)**
//   恒产 `mov 0xc(%ebp),%eax; cmp $0xa,%eax`；折成 ORIG 的 `cmpl $0xa,0xc(%ebp)` 只在
//   **形参自身为 unsigned**（`_Z1bj` 探针）或 e1..e5 五种等价写法全不改 —— 而本件 mangled
//   是 `…13CheckMoveTownEi`（int）⇒ 不可造。判 N，勿再试。
int CUser::CheckMoveTown(int vill)
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    // ORIG 0x8678548 `cmpl $0xa,0xc(%ebp); ja`：4.1 对 arg 槽直折无符号比较；
    // 4.4 恒先载 %eax（具名 uv 槽双 spill 更差，probe），保留 cast 形 ndiff=1。
    if (static_cast<unsigned int>(vill) <= 10)   // ORIG: ja 无符号比较
    {
        // N6 消解：直写表达式 `(1 << vill) & 0x580` 被 4.4 前端折成 `sar+and$1`
        // （probe p17.f2）；改具名 bit 局部保 shl 链。裸 `register` 提示（非
        // asm("reg") 绑定，P0/N96 合规）令 4.4 免槽回写，复现 ORIG
        // `mov $1; shl %cl; mov %ebx,%eax; and $0x580` 全寄存器形（同 is_fighting 先例）。
        register int bit;
        bit = 1 << vill;
        if (bit & 0x580)
            return 0;
    }
    	Village* village = G_GameWorld()->GetVillage(vill);
	if (!village)
		return 8;
	if (get_charac_level() < village->m_field2c)
		return 8;
	if (village->m_questIdx != 0)
	{
		if (!getCurCharacQuestR()->isClearQuest(village->m_questIdx))
			return 7;
	}
	return 0;
}