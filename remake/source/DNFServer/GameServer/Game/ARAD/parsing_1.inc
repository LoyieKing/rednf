int PurchaseLimitItem::parsing(char* data)
{
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
