// ORIG 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 用 DNFLex 扫描列表文件，逐行 ScanInt+ScanStr 填充 m_fileMap。
// 第 4 参 map<int,int>* 在 ORIG 中未被使用（可由调用方传 0）。
// b28d-31: N 封顶（N82）——ORIG 0x088bbd77；134 vs 136（帧/槽）。
// [b28d-Main 代落·S248 P292] int→bool：ORIG 调用点 0x08903edb 为 `test %al,%al; je`
// （无 setne）；返回域 0/1。另两处声明（OnlyOrigBatch12.cpp:53 / CtorProducers_1.cpp:133）
// 本已为 bool ⇒ 本次消除跨 TU 返回类型 ODR 分歧。返回类型不入 mangled。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* /*unused*/)
{
    if (!loadRDARScriptFile(dir, listPath))
        return 0;
    list->Clear();                 // ORIG 0x088bbda8 call STScriptFileList::Clear（非内联 m_fileMap.clear）

    // [round4-L19 2026-09-09] 判据⑫ widthscan 实测 `clr_b orig=0 our=1` ⇒
    // ORIG 无该 bool 预清（ScanInt 为出参，总会写）⇒ 按 P171「ORIG 不写即删」
    // 去掉 `= false`。
    bool ok;
    std::string line;
    for (;;)
    {
        int idx = ScanInt(&ok);
        if (!ok)
            return 1;
        if (!ScanStr(&line))
            return 1;

        bool isNew = (list->m_fileMap.find(idx) == list->m_fileMap.end());
        if (!isNew && !g_isAllowScriptDuplication)
        {
            // ORIG 0x088bf09：logFormat(0,"../RDARScript.cpp",
            // __FUNCTION__,0x323,"SCRIPT FILE LIST LOAD FAIL!
            // Index Duplicated [%s%s][%d]", dir, listPath, idx)
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 0x323,
                "SCRIPT FILE LIST LOAD FAIL! Index Duplicated [%s%s][%d]",
                dir, listPath, idx);
            return 0;
        }
        int sign = isNew ? 1 : -1;
        list->m_fileMap.insert(std::pair<const int, std::string>(idx * sign, line));
    }
}