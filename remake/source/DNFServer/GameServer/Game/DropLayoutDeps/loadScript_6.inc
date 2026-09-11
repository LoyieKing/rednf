// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
int CGeneratorCommonData::loadScript(const char* path)
{
    // [b28d-solo-DropLayoutDeps] AE（ORIG 0x08534456；verdict 以 check_tu_routed
    // clean 口径为准，尾部对齐 nop 由 §6.1 strip_nop_padding 剥离；game_func_report
    // --no-compile 报 101/102 属 N124 伪差）。p167 CLEAR（10=10）。已修：for(;;)+
    // goto 尾标签（P164/P223）、idx 循环前预清（P198①）、条件 operator==（p167 判据）、
    // return 0x1f8 块移至循环出口后。形实验记录：if/else 正形为最优；break+循环后
    // return、标签置循环内、#pragma optimize(no-align-labels/loops/jumps/align-labels=1)
    // 均致块序错或条数 -1，已回退。
    if (!loadRDARScriptFile("", path))
        return 0x1d8;
    std::string token;
    int idx = 0;                             // ORIG 循环前预清（-0x10，P198①）
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            goto ok_done;
        if (token == "[gold drop ref table]")   // ORIG 用 operator==（p167 判据）
        {
            m_goldGenTable[0].m_base = 0;
            m_goldGenTable[0].m_randAmp = 0;
            for (int i = 1; i <= 200; ++i)
            {
                idx = ScanInt(&ok);          // 写入下标来自脚本首列（ORIG 同形）
                if (!ok)
                    return 0x1ec;
                m_goldGenTable[idx].m_base = ScanInt(&ok);
                if (!ok)
                    return 0x1f0;
                m_goldGenTable[idx].m_randAmp = ScanInt(&ok);
                if (!ok)
                    return 500;              // 0x1f4
            }
        }
        else
        {
            goto fail_ref_table;
        }
    }
fail_ref_table:
    return 0x1f8;
ok_done:
    return 0;
}