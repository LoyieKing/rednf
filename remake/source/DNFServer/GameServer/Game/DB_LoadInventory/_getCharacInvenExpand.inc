int DB_LoadInventory::_getCharacInvenExpand(SIG_LOAD_INVENTORY* sig) const
{
    // ORIG 有 `_ZZNK16DB_LoadInventory21_getCharacInvenExpand...E9handleIdx`
    // （.rodata size 4，nm 实证；const 版 K 前缀易漏读）⇒ 保留 static const
    // 声明，4.1 使用点折叠为立即数 3（0840adf9 `movl $0x3,0x4(%esp)`）。
    static const int handleIdx = 3;
    // ORIG 清零三形（memset call / 对齐剥离慢路 / 简单 rep stos）由数组尺寸与
    // 槽对齐自然产生；长度变量与其 binLen 副本成对（ORIG `mov -0x20,%eax;
    // mov %eax,-0x1c`），照抄 P171。
    char cargoBuf[0x2b82] = {0};
    unsigned int cargoLen = 0x2b82;     // ORIG -0x20
    int cargoBinLen = cargoLen;         // ORIG -0x1c（初始化副本，后被覆盖）
    char jewelBuf[0x120f] = {0};
    unsigned int jewelLen = 0x120f;     // ORIG -0x24
    int jewelBinLen = jewelLen;         // ORIG -0x18
    char equipBuf[0xa5c] = {0};
    unsigned int equipLen = 0xa5c;      // ORIG -0x28
    int equipBinLen = equipLen;         // ORIG -0x14

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect cargo_capacity,cargo,jewel,expand_equipslot from"
                  " charac_inven_expand where charac_no=%u", sig->m_characNo);
    if (!db->exec(true) || !db->fetch())
    {
        return 0x243b;
    }
    // 声明序=槽深序（4.4 先声明深）：db 声明在前占 -0x10，col 声明在后占
    // -0xc，与 ORIG 一致；col 的初始化位置也落在 exec/fetch 检查之后。
    int col = 0;                        // ORIG -0xc（get_* 的 col++ 实参形）
    if (!db->get_int(col++, reinterpret_cast<int&>(sig->m_cargoCapacity)))
    {
        return 0x243f;
    }
    cargoBinLen = db->get_binary_length(col);
    if (!db->get_binary(col++, cargoBuf, cargoLen))
    {
        return 0x2443;
    }
    jewelBinLen = db->get_binary_length(col);
    if (!db->get_binary(col++, jewelBuf, jewelLen))
    {
        return 0x2448;
    }
    equipBinLen = db->get_binary_length(col);
    if (!db->get_binary(col++, equipBuf, equipLen))
    {
        return 0x2453;
    }
    if (!uncompress_zip(reinterpret_cast<char*>(sig->m_cargo), cargoLen, cargoBuf,
                        static_cast<unsigned int>(cargoBinLen)))
    {
        return 0x2457;
    }
    if ((cargoLen % 0x3d) != 0 && cargoLen != 0)
    {
        return 0x2458;
    }
    // jewel 区 ORIG `add $0x949e` = 0x5949+249×0x3d（与 invenType1 重叠序列化区）。
    if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_invenType1[249]),
                        jewelLen, jewelBuf,
                        static_cast<unsigned int>(jewelBinLen)))
    {
        return 0x245c;
    }
    if ((jewelLen % 0x3d) != 0 && jewelLen != 0)
    {
        return 0x245d;
    }
    if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_expandEquip), equipLen,
                        equipBuf,
                        static_cast<unsigned int>(equipBinLen)))
    {
        return 0x2462;
    }
    if (((equipLen - 4) % 0x3d) != 0 && equipLen != 0)
    {
        return 0x2463;
    }
    return 0;
