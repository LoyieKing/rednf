// ============================================================================
// importGoldLimitByLevel —— ORIG 0x088fdb69
// 报告 docs/class_func_reports/_lt_global_gt_/importGoldLimitByLevel.md
// 调用点（init.md 0x832…+0x2e4b）：CDataManager::init → this+0x6410
//   （CDataManager::m_moneyLimit[201]，int 数组）。
// 目标为 unsigned int*（报告 uint*），访问 out[level]。
// 验收：gap 5→1；P167 差集 11/11 归零；残余 1 行 = ORIG `mov $0,%eax;
// mov %eax,-0x10` 死初始化二段物化，4.4 恒折单条 movl（N18a 族）。
// ===========================================================================
bool importGoldLimitByLevel(unsigned int* out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    // 清前 0x190（400）字节（与汇编 movl $0x190 一致；m_moneyLimit 数组更大，
    // 但 ORIG 仅清零 400 字节）。ORIG 0x088fdb69 无空指针检查（0x8fdb71 直接 memset）。
    std::memset(out, 0, 0x190);

    if (!loadRDARScriptFile("", "Etc/GoldLimitByLevel.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xe3,
                              // Q7S4: ORIG 0x08e0f3dc = CP949「레벨별 금액 소지제한 정보 로드 실패!」
                              // （我方此前为 UTF-8 编码且文本不同「전적분…」，AE 归一化掩盖）
                              "CRITICAL ERROR : \xb7\xb9\xba\xa7\xba\xb0 \xb1\xdd\xbe\xd7 \xbc\xd2\xc1\xf6\xc1\xa6\xc7\xd1 \xc1\xa4\xba\xb8 \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return 0;
    }

    std::string token;
    int level = 0;            // ORIG -0x14（movl $0 直存）
    // [b28c-lane24] ORIG -0x10 死初始化为 `mov $0x0,%eax; mov %eax,-0x10(%ebp)`
    // ——探针 /tmp/b28c_l24_probe3.cpp f_float0 实证这是 float 0.0f 的 4.4 形
    //（int 0 生 `movl $0,slot` 单段）⇒ 该死局部原码为 float（P171 照抄死写入）。
    float value = 0.0f;
    bool ok = false;          // ORIG -0x19
    (void)value;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[gold limit from level]")
        {
            for (int j = 0; j <= 0x63; ++j)   // ORIG cmpl $0x63/setle（P2）
            {
                level = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG：LHS 地址先物化（ebx）→ ScanInt(0) → (%ebx) 直存
                out[level] = (unsigned int)ScanInt((bool*)0);
            }
        }
    }
    return 1;
}