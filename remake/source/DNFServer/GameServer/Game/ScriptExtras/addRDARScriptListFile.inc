bool addRDARScriptListFile(STScriptFileList* list, const char* dir, const char* file)
{
    if (!loadRDARScriptFile(dir, file))
        return false;

    // [L3] P309 头部声明律：ORIG name@-0x2c（深）/ idx@-0x24（浅）⇒ 4.4 头内声明
    // 「先声明浅」⇒ idx 必须先声明（原循环内声明使 name 占浅槽、idx 占深槽，反向）。
    int idx;
    // [L3] 头内声明序 = ORIG 浅→深（idx -0x24 / ok -0x25 / name -0x2c）：
    // 4.4 头内声明「先声明浅」，故 ok 必须在 name 之前。
    // [L10-2] 判据⑫：同 loadRDARNameFile（ORIG clr_b=0）⇒ 去初始化器。
    bool ok;
    std::string name;
    for (;;)
    {
        idx = ScanInt(&ok);
        if (!ok)
            break;
        if (!ScanStr(&name))
            break;
        // [L3] P198② 镜像：ORIG `test %al,%al` 直测（无槽无 cmpb）⇒ 谓词整体内联
        // 回 if 条件位（具名 bool isNew 使 4.4 落 `mov %al,-0x9; cmpb $0,-0x9`）。
        if (list->m_fileMap.find(idx) == list->m_fileMap.end())
        {
            // [b28d-lane16 P192] ORIG 0x088bc2?? 直接构造 value_type（mangled
            // `C1IRiRSsEEOT_OT0_` = pair<const int,string>(int&,string&)）；
            // make_pair 生成值 pair + 转换构造 + 额外 D1（p167 EXTRA D1Ev×2）。
            list->m_fileMap.insert(std::map<int, std::string>::value_type(idx, name));
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 0x418,
                "SCRIPT FILE LIST ADD FAIL! - DUPLICATED DATA [%s%s][%d]",
                dir, file, idx);
        }
    }
    return true;
