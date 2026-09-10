bool importExperienceItemMapInfo(std::map<int, int, std::less<int>,
                                 std::allocator<std::pair<int const, int> > >& m,
                                 bool second)
{
    m.clear();

    if (!loadRDARScriptFile("Etc/ItemDictionary/", "(R)baseItemDictionaryTable.etc"))
        return 0;

    // [b28d-solo-RDARScriptEtcParameter] ORIG 槽序 token@-0x40 / ok@-0x39（4.1 先声明深）；
    // 4.4 先声明浅 ⇒ 倒序声明（ok 先、token 后）使 token 归位 -0x40（key/val/形参截断槽
    // 因 4 字节对齐吸收差异全部归位）。残余 4 条 = ok 槽 -0x3c vs ORIG -0x39：4.4 标量槽基
    // 4 对齐（-0x3c）而 4.1 从 -0x39 起；实测 4B 数组/结构体尾字节取址恒 `lea -0x3c;add $0x3`
    // 不折叠（P235 反例），无源码形可造 ⇒ N95 族封顶。无初始化（ORIG ScanInt 必写）。
    bool ok;
    std::string token;            // ORIG -0x40
    if (!ScanType(token, true))
        return 0;
    if (token == "[base table]")
    {
        for (;;)
        {
            int key = ScanInt(&ok);
            if (!ok)
                break;
            int val = ScanInt(&ok);
            if (!ok)
                return 0;
            // ORIG cmp $0x88b8/jle：有符号比较
            if (val > 0x88b8)
            {
                LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                      __FUNCTION__, 0x30b8,
                                      "ItemDictionary mappingTable MAX INDEX(40000) WARNING READY TO DB_TOOL");
                return 0;
            }
            // 依据 objdump 压栈序：second==false → (key,val)；true → (val,key)
            if (second)
                m.insert(std::make_pair(val, key));
            else
                m.insert(std::make_pair(key, val));
        }
        return 1;
    }
    return 0;
