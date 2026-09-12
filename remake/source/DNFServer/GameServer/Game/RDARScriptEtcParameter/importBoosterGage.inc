// ============================================================================
// importBoosterGage —— ORIG 0x08913a7e
// 报告 docs/class_func_reports/_lt_global_gt_/importBoosterGage.md
// 调用点（init.md +0x2412）：CDataManager::init → this+0x8770
//   （CDataManager::m_boosterGage，BoosterGageData）。
// 布局：+0x00 vector<BoosterPoint> m_BoosterPoint
//       +0x0c unsigned char m_cMaxBoosterGage
//       +0x0d unsigned char m_cFieldD
//       +0x0e unsigned char m_cMultiboxCount
// BoosterPoint：+0x00 unsigned long m_dwItemIdx / +0x04 unsigned char m_cPoint
// ============================================================================
// [b28 BM2 判词·AE] 清单 NEAR(2) 为全二进制 call 目标标签伪差；fresh
// /tmp/tu_RDARScriptEtcParameter.o fn_diff_tu 归一 0 差（152=152 逐条）。
// [b28d-27] AE：ORIG 0x08913a7e；fn_diff_tu 归一 0 差（157=157），p167 CLEAR。
// [b28d-solo-RDARScriptEtcParameter] AE（IDENTICAL 级）：fresh .o fn_diff_tu
// 归一 0 差（152=152，raw 157=157），p167 CLEAR（19/19）。
// [SplitC batch10 · 判据] 权威返回类型 = **bool**：ORIG 唯一外部调用点 `_ZN12CDataManager4initEv`
//   的 `call` 后为 `xor $0x1,%eax` + `test %al,%al`（运算符 ! 的 8 位语义）
//   ⇒ 与 DataManagerInitDepends.h 的 bool 声明一致；原 int 为历史错修。
bool importBoosterGage(char* path, BoosterGageData* out)
{
    if (!out)
        return 0;

    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    std::string unused;   // ORIG -0x10：构造/析构但从未使用的第二 string（照抄）
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[max gage]")
        {
            // ORIG：store 先于 ok 检查（ScanInt 返回值 %eax→%edx 截 byte 直写 +0xc）
            bool okMax = false;                    // ORIG -0x11 独立槽
            out->m_cMaxBoosterGage = (unsigned char)ScanInt(&okMax);
            if (!okMax)
                break;
        }
        else if (token == "[sub gage]")
        {
            bool okSub = false;                    // ORIG -0x12 独立槽
            out->m_cFieldD = (unsigned char)ScanInt(&okSub);
            if (!okSub)
                break;
        }
        else if (token == "[reward multiple]")
        {
            bool okRw = false;                     // ORIG -0x13 独立槽
            out->m_cMultiboxCount = (unsigned char)ScanInt(&okRw);
            if (!okRw)
                break;
        }
        else if (token == "[booster point]")
        {
            bool okBp;                             // ORIG -0x14 独立槽（无初值）
            BoosterPoint bp;                       // ORIG -0x19..-0x15 固定槽
            for (;;)
            {
                // [SemB10 s2_10 2026-09-12 · 结构性偏差修复] ORIG 内层回边
                // `jmp 0x8913bd4` 落点 = `movb $0x0,-0x14(%ebp)`（okBp 预清）
                // ⇒ 原码的 `okBp = false;` 是**循环体内赋值语句**（P198① 两句式：
                // 声明在循环外无初值、预清在循环内），旧形只有声明处一次预清，
                // 回边落点前移到 `lea -0x14(%ebp),%eax`（跳过预清，偏移
                // +0x156→+0x15a，链接口径差一行）。语义等价（预清幂等）。
                okBp = false;
                bp.m_dwItemIdx = (unsigned long)ScanInt(&okBp);   // →-0x19
                if (!okBp)
                    break;
                bp.m_cPoint = (unsigned char)ScanInt(&okBp);      // al 直写 -0x15
                if (!okBp)
                    break;
                out->m_BoosterPoint.push_back(bp);
            }
        }
    }
    return 1;
}