bool importGoldLimitByLevel(unsigned int* out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    // 清前 0x190（400）字节（与汇编 movl $0x190 一致；m_moneyLimit 数组更大，
    // 但 ORIG 仅清零 400 字节）。ORIG 0x088fdb69 无空指针检查（0x8fdb71 直接 memset）。
    std::memset(out, 0, 0x190);

    if (!loadRDARScriptFile("", "Etc/GoldLimitByLevel.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xe3,
                              "CRITICAL ERROR : \xec\xa0\x84\xec\xa0\x90\xeb\xb6\x84 \xea\xb8\x88\xec\x95\xa1 \xec\x86\x8c\xec\xa7\x80\xec\xa0\x9c\xed\x95\x9c \xec\xa0\x95\xeb\xb3\xb4 \xeb\xa1\x9c\xeb\x93\x9c \xec\x8b\xa4\xed\x8c\xa8!");
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
