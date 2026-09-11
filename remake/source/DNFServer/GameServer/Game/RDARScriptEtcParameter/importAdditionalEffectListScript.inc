// ===========================================================================
// importAdditionalEffectListScript —— ORIG T 0x08907e3a
// （docs/class_func_reports/_lt_global_gt_/importAdditionalEffectListScript.md；
// 偏移经 objdump 复核：+0x00 index/+0x04 m_strings(animation)/+0x10 type/
// +0x14 fixX/+0x18/+0x1c/+0x20/+0x24/+0x28 各延迟项/+0x2c drawOver byte）
// 清空 g_additionalEffectMap_ 后按 "[effect]" 块解析各字段，
// "[/effect]" 时 g_additionalEffectMap_[index] = tmp。
// loadRDARScriptFile 失败记日志返 0，成功返 1。
// 验收：差集 40/40 归零；clean 225=225，残余为 clear/dtor 块序镜像
// （4.4 jump-threading 跨版本伪影，N 类）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] AE：ORIG 0x08907e3a；外层 ScanType 失败改
// break+函数尾 return 1，复现 ORIG 共享出口（P63）；fn_diff_tu 归一 0 差
// （225=225，raw 230=230），p167 CLEAR（40/40）。
int importAdditionalEffectListScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              // Q7S4: ORIG 0x08907e63 movl $0x8e103d8 = 完整前缀串（我方此前仅 "%s"）
                              __FUNCTION__, 0x172c,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    std::string token;
    g_additionalEffectMap_.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;                              // ORIG 0x332 共享出口（P63）
        if (token == "[effect]")
        {
            AdditionalEffectData data;             // ORIG -0x40（每轮外层 body 构造）
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                          // EOF：仅析构 data，**不**写表
            if (token == "[index]")
                data.m_id = ScanInt((bool*)0);
            else if (token == "[type]")
                data.m_mode = ScanInt((bool*)0);
            else if (token == "[draw over]")
                data.m_flag = (unsigned char)(ScanInt((bool*)0) != 0);
            else if (token == "[fix position x]")
                data.m_count = ScanInt((bool*)0);
            else if (token == "[custom ani delay]")
                data.m_min = ScanInt((bool*)0);
            else if (token == "[custom ani delay in town]")
                data.m_max = ScanInt((bool*)0);
            else if (token == "[default term in town]")
                data.m_valueA = ScanInt((bool*)0);
            else if (token == "[default term]")
                data.m_valueB = ScanInt((bool*)0);
            else if (token == "[dash term]")
                data.m_valueC = ScanInt((bool*)0);
            else if (token == "[animation]")
            {
                std::string str;                   // ORIG -0x10 块级一次构造/尾析构
                for (;;)
                {
                    if (!ScanStr(&str))              // ORIG xor$1/test/jne 形
                        break;
                    data.m_strings.push_back(str);
                }
            }
            else if (token == "[/effect]")
            {
                g_additionalEffectMap_[data.m_id] = data;
                break;
            }
            }
        }
    }
    return 1;
}