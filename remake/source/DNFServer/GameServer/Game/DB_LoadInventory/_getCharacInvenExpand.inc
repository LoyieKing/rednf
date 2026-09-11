// ============================================================================
// DB_LoadInventory::_getCharacInvenExpand @0x0840ad2a（const）
// charac_inven_expand 4 列：cargo_capacity(0)→+0xa8e0，cargo(1)→+0xa8e4，
// jewel(2)→+0x949e，expand_equipslot(3)→+0x14aa4。容量 0x2b82/0x120f/0xa5c；
// 解压后 cargo/jewel 按 0x3d 对齐校验，expand_equipslot 按 (size-4)%0x3d。
// 返回 LineFunc 风格错误行号 0x243b..0x2463，成功 0。
// b28d-solo-DB_LoadInventory：len/binLen 副本（P171）+ 清零三形（memset call /
// 对齐剥离慢路 / 简单 rep stos）已归位（277 vs 278，残差 9 行）
// ＝N107（`&sig->m_cargoCapacity` 4.4 恒 add $0xa8e0 两步 vs ORIG lea；字段须
// unsigned 不可改）+ P75（3 处 uncompress_zip arg0/arg4 装载序跨版本）；
// p167 差集 MISSING=0。
// [L3CUserPremDBInven 本波实测（ORIG 0x0840ad2a）] 复算 277/278：残差逐条 =
//   ① `reinterpret_cast<int&>(sig->m_cargoCapacity)` 4.4 恒 `add $0xa8e0,%eax;
//   mov %eax,%edx`（0840ae79 ORIG 为单条 `lea 0xa8e0(%eax),%edx`）——本波改
//   `*reinterpret_cast<int*>(&...)` 形**实测不变**（仍 add+mov，278 条）⇒ 回滚；
//   ② 3 处 uncompress_zip arg0/arg4 装载序（ORIG 先 arg0 后 arg4）。
//   ⇒ N107（4.4 对成员常量偏移恒折 add，非取址形可择）+ P75 双封顶。
// SweepCInventoryTail（2026-09-09）：补回 ORIG 实有静态符号
// `_ZZNK16DB_LoadInventory21_getCharacInvenExpand...E9handleIdx`（.rodata size 4，
// nm 实证；旧注释误判「本函数无 handleIdx」系漏读 K 前缀）⇒ 恢复
// `static const int handleIdx = 3;`，4.4 使用点仍折叠为立即数 3（指令数不变）。
// ============================================================================
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
}