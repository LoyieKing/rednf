// b28d-solo-ARAD verdict=IDENTICAL/AE：Info 提到循环外构造一次（ORIG 0x81a21dc 在
// r=0 之前）+ Item_Index 写入 Info+0x04（make_pair first = `lea -0x2c; add $0x4`，
// 即 info.reserved 槽）+ 具名 insert 结果 res ⇒ 槽位全对齐（script -0x88 / Info -0x2c /
// insert sret -0x34 / .second -0x30）。残余仅 4.4 循环对齐 nop（§6.1 豁免）。
int PurchaseLimitItem::parsing(char* data)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [L4e 2026-09-11 · 负结论：112/113，残余＝goto 后 1 条 `nop`] ORIG 0x81a21d6 `jmp`
    // 直落 +0x147（无填充），我方同位置多 1 条 nop。根因同上（4.4.x 循环尾 nop，4.1.2 无，
    // 8 旗标无效）⇒ 源码层不可修。OBJ 未链接的表象（call 目标/串偏移）为 §6.3 假阳性。
    Arad_Script script;
    if (!script.Load(data, "purchase_limit_item"))
        return 0x4c;                  // ORIG 0x81a21ab：Load 失败返 0x4c
    if (script.GetRowNum() == 0)
        return 0x4f;                  // ORIG 0x81a21cc：无行返 0x4f
    // b28d-solo-ARAD：ORIG 0x81a21dc 在 r=0 之前**循环外**构造一次 Info（-0x2c），
    // 循环内复用 ⇒ Info 声明必须在函数级（原循环体内声明使 script 槽 -0x8c、Info -0x34）。
    Info info;
    int r = 0;
    goto check;
loop_body:
    {
        // b28d-solo-ARAD：ORIG 0x81a220a 把 Item_Index 写入 Info+0x04（reserved 槽，
        // make_pair 的 first 即 `lea -0x2c; add $0x4`），非独立 idx 局部。
        info.reserved = (unsigned int)script.GetInt(r, "Item_Index");
        info.purchaseType = (unsigned short)script.GetInt(r, "purchase_Type");
        info.limitCount = (unsigned short)script.GetInt(r, "puchase_limit_count");
        // ORIG 0x81a2297：insert 而非 operator[]（重复 key 保留首条），
        // 插入失败（已存在）返 0x5c。
        // b28d-solo-ARAD：具名 sret（同 ExceptionItem）令 4.4 把 insert 返回槽排到
        // ORIG 的 -0x34（.second -0x30），Info 上移到 -0x2c。
        std::pair<std::map<unsigned int, Info>::iterator, bool> res =
            m_purchaseLimitMap.insert(std::make_pair(info.reserved, info));
        if (!res.second)
            return 0x5c;
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}