// b28d-solo-ARAD verdict=IDENTICAL/AE：ORIG 0x81a23f8 在 r=0 之前**循环外**构造
// 一次 Data（-0x38），循环内复用；make_pair 实例 `_ZSt9make_pairIRjRN13AradBonusItem4DataEE`
// 的 first = &Data+0x04（即 unsigned int itemId 字段），无独立 itemIdx 局部
//（ORIG 0x81a2426 `call GetInt; mov %eax,-0x34` 直存字段）。
int AradBonusItem::parsing(char* data)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [L4e 2026-09-11 · 负结论：112/113，残余＝goto 后 1 条 `nop`] ORIG 0x81a23f2 `jmp`
    // 直落 +0x146（无填充），我方同位置多 1 条 nop。根因同上（4.4.x 循环尾 nop，4.1.2 无，
    // 8 旗标无效）⇒ 源码层不可修；本函数其余逐条全等（含 Data 循环外构造/字段直存）。
    Arad_Script script;
    if (!script.Load(data, "bonus_item"))
        return 0x79;
    if (script.GetRowNum() == 0)
        return 0x7c;
    // b28d-solo-ARAD：Data 循环外构造（同 PurchaseLimitItem::Info）。
    Data d;
    int r = 0;
    goto check;
loop_body:
    {
        // ORIG 0x81a2426/0x81a2446/0x81a2467：item_id→Data+4、bonus_type→Data+8、
        // bonus_value→Data+0（GetInt 结果直存，无中间局部）。
        d.itemId = (unsigned int)script.GetInt(r, "item_id");
        d.bonusType = (unsigned short)script.GetInt(r, "bonus_type");
        d.bonusValue = script.GetInt(r, "bonus_value");
        // b28d-solo-ARAD：具名 sret 令 insert 返回槽落 ORIG 的 -0x40（.second -0x3c），
        // Data 上移到 -0x38。
        std::pair<std::map<int, Data>::iterator, bool> res =
            m_bonusMap.insert(std::make_pair(d.itemId, d));
        if (!res.second)
            return 0x88;   // ORIG insert 失败（.second==0）→ 0x88
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}