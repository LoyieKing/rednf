int AradBonusItem::parsing(char* data)
{
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
