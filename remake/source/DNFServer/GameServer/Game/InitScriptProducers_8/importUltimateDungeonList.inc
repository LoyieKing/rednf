// ============================================================================
// importUltimateDungeonList —— ORIG 0x08913c7e
// set 清空后扫描；[apply ultimate] 段内逐 int insert；遇到 [/apply ultimate]
// 或任意其他 token 连续吞至该结束标记；EOF 与正常结束均返回 1，打开失败返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 08913c7e：clean 89/89（ORIG 92 insns）；
// p167 CLEAR。已对齐：set::clear、ok 每轮重置、insert(int)、EOF 与 [/apply ultimate]
// 共享尾块 return 1、打开失败日志返回 0。
bool importUltimateDungeonList(const char* path, std::set<int>& out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
            0x3209,
            "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 "
            "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
            path);
        return 0;
    }

    std::string token;
    bool ok;
    int value;

    out.clear();                                   // ORIG set::clear
    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return 1
        ok = false;                                // ORIG local_19=false 每轮重置
        if (token == "[apply ultimate]")
        {
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out.insert(value);
            }
            continue;
        }
        if (token == "[/apply ultimate]")
            break;                                 // 结束标记 → 直接收尾
        // 其他 token：ORIG do-while(!=)[/apply ultimate]) 继续吞并
    }
    return 1;
}