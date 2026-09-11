// ORIG 0x088bff9c：
//   1) 用 g_scriptStringManager_ 的文件列表装载 dir 指定的列表文件；
//   2) 逐项按 s_bUseScriptPack 选择 Pack/Plain 装流，调 loadStringFile；
//   3) 任一失败向 err 追加 ErrorLog（code=0x80000003，message=文件名）并置失败。
// [b28d-lane16 DIFF] ORIG 088bff9c：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
bool InitRDARScriptStringManager(const char* dir, const char* file,
                                 std::vector<ErrorLog>* err)
{
    if (file == 0)
        file = "";

    // ORIG 在本函数发**三处** getStringFileList() 调用点（0x088bff9c 内
    // +0x1c / +0x6f / +0x92）：列表装载、循环外 size()、循环内 GetFileName(i)
    // 各自重新取一次（无缓存局部变量）。照抄调用点结构（4.4 -O0 逐点对应）。
    if (!loadRDARScriptListFile(
            static_cast<STScriptFileList*>(g_scriptStringManager_.getStringFileList()),
            "", dir, 0))
        return false;

    // s_bUseScriptPack 由 InitRDARScript 置位（ORIG 同一 RDARScript.cpp 域静态）。
    DNFLex::TLoadStream loadStream =
        s_bUseScriptPack ? LoadStreamFromPack : LoadStreamFromPlain;

    bool allOk = true;
    // ORIG 0x088c001a：循环外一次 map<int,string>::size()（4.1 out-of-line
    // 0x086d2cd8）存槽 -0x14；循环 i<size、name 经 STScriptFileList::
    // GetFileName(i)（0x088bad4e，0x088c0044 call）取，索引 i 同时是
    // loadStringFile 的 table 形参（-0x10 槽）。
    // [L10-2] P183/P0：ORIG 0x088c0012 `call getStringFileList; mov %eax,(%esp);
    // call size()`（无具名 fileList 槽往返）⇒ 内联，省 -0x18 槽。
    int count = static_cast<int>(static_cast<STScriptFileList*>(
        g_scriptStringManager_.getStringFileList())->m_fileMap.size());
    for (int table = 0; table < count; ++table)
    {
        const char* name = static_cast<STScriptFileList*>(
            g_scriptStringManager_.getStringFileList())->GetFileName(table);
        DNFLex::stream_data_t stream;
        // [L10-2] 判据①/档①：ORIG 0x088c0065 `call *%eax; xor $0x1,%eax; test
        // %al,%al; je <成功路径>` ⇒ 负向判定（bool callee），失败路径直落；
        // 且成功路径 allOk 物化为 `movzbl %al,%eax; and %ebx,%eax; test; setne
        // %al; mov %al,-0x15`（0x088c0109..0x088c0113）= 单表达式 `f() && allOk`，
        // 无具名 ok 局部、无具名 istream* in（ORIG 直接读 stream 首字段）。
        if (!loadStream(file, name, &stream))
        {
            allOk = false;
            if (err != 0)
            {
                ErrorLog entry;
                entry.m_code = static_cast<int>(0x80000003);
                entry.m_message = name;
                err->push_back(entry);
            }
        }
        else
        {
            // [L10-2] ORIG 0x088c0109 段无短路分支，直接
            // `movzbl %al,%eax; and %ebx,%eax; test; setne %al; mov %al,-0x15`
            //（allOk 先入 %ebx 跨 call 保活）⇒ 原码为按位与物化（非短路 &&）。
            allOk = allOk & g_scriptStringManager_.loadStringFile(
                                 static_cast<std::istream*>(stream.stream), name, table, err);
            stream.destroy();
        }
    }
    return allOk;
}