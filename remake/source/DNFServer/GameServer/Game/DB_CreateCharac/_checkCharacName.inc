// ===========================================================================
// _checkCharacName — 报告 @0x084011d4
// 重名/delete_flag 判定；gc_no==1010(0x3f2) 且恰与基准名相同时追查 query 日志。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084011d4 120=120。
// 修法：P220/P222 正极性包裹（!=0 → setne/test/je return-true 岛）+ P227 两处全表达式
// 临时链（rowCnt→%ebx、sig+4→%esi、GetQueryString→%ebx）。
bool DB_CreateCharac::_checkCharacName(MySQL* db, char const* dbTable,
                                       SIG_CREATE_CHARAC* sig)
{
    db->set_query("seLect delete_flag from %s.charac_info where charac_name='%s'",
                  dbTable, sig->m_name);
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }

    int deleted = 0;
    // P220/P222 正极性包裹：ORIG `test; setne; test; je <return-true 岛>`。
    if (db->get_n_rows() != 0)
    {
        sig->m_resultCode = 2;
        // [b23S3C] P48 去缓存：无 nRows/badFetch 具名槽（ORIG rows 直判 + log 点
        // 二次 call；短路双 xor 直判无 al 落槽）；deleted!=0 反形（ORIG `test;je
        // gc块`、rc3 作 body，P191）；rowCnt/qs 在 ctor 前求值（ORIG +0x10c/
        // +0x17d call 位于 ctor 之前），值跨 ctor 4.1 落 %ebx/%esi 寄存器缓存
        // （P227 全表达式临时链复现，4.4 槽差已消）。
        if (!db->fetch() || !db->get_int(0, deleted))
            return false;
        if (deleted != 0)
        {
            sig->m_resultCode = 3;
        }
        else
        {
            if (G_CEnvironment()->get_gc_no() == 0x3f2)
            {
                // P227 全表达式临时链：ORIG rowCnt 于 ctor 前求值入 %ebx、
                // sig+4 入 %esi 跨 ctor 保活（084012eb/084012f0 段）。
                cMyTrace(__PRETTY_FUNCTION__, 0xd6a, 5)(
                    "already charac_name(%s) row(%d)", sig->m_name,
                    db->get_n_rows());
                // [OurOnly33 符号名轴 2026-09-11] ORIG 全 ELF 无 BASELINE_NAME（我方发射
                // `_ZZN15DB_CreateCharac16_checkCharacName…E13BASELINE_NAME`）——ORIG 直接以
                // .rodata 匿名串 DAT_08c42c04 作 strcmp 立即数实参 ⇒ 删 local static、内联字面量。
                // 编码（§6.5.1 J2）：ORIG 实字节 = `bb e7 c0 cc c5 e4 00`（EUC-KR "사이토"，
                // objdump -s .rodata @0x08c42c04 实证）⇒ 用字节转义而非 UTF-8 源字面量。
                if (strcmp(sig->m_name, "\xbb\xe7\xc0\xcc\xc5\xe4") == 0)
                {
                    // P227：GetQueryString 结果于 ctor 前入 %ebx（08401356 段）。
                    cMyTrace(__PRETTY_FUNCTION__, 0xd6d, 5)(
                        "already charac_name query(%s)", db->GetQueryString());
                }
            }
            sig->m_resultCode = 1;
        }
        return false;
    }
    return true;
}