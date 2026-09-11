// ============================================================================
// importVillageAttackedScript —— ORIG 0x089093ee
// [attacked map info] 段内每行 4 个 int 构成 stAttackedMapInfo_t push_back；
// 第 2/3/4 列 Scan 失败分别返回 0x1990/0x1994/0x1998；打开失败 0x197f；
// 成功返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 089093ee：clean 96/96（ORIG 99 insns）；
// p167 CLEAR。已对齐：4 列 ScanInt 独立错误码 0x1990/0x1994/0x1998、首列失败回标签、
// stAttackedMapInfo_t 逐字段 + push_back、打开失败 0x197f。
int importVillageAttackedScript(const char* path, stVillageAttacked_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
        return 0x197f;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[attacked map info]")
        {
            for (;;)
            {
                stAttackedMapInfo_t info;
                info.m_vill = ScanInt(&ok);
                if (!ok)
                    break;                         // 首列失败 → 回标签扫描
                info.m_area = ScanInt(&ok);
                if (!ok)
                    return 0x1990;
                info.m_field8 = ScanInt(&ok);
                if (!ok)
                    return 0x1994;
                info.m_fieldC = ScanInt(&ok);
                if (!ok)
                    return 0x1998;
                out.m_mapList.push_back(info);
            }
        }
    }
    return 0;
}