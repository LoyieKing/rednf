int importItmeGroupTable(const char* dir, const char* file,
                         std::map<std::string, int>& table)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1eb2;

    std::string name;
    bool ok = false;

    while (true)
    {
        int id = ScanInt(&ok);
        if (!ok)
            break;
        // b28c-l23：ORIG @0x0890bb5x 循环体内**无** `movb $0x0,-0x25(%ebp)` 预清
        // （ok 仅由 ScanInt 出参写、判后不复位）⇒ 该复位是原码没有的死写入，删。
        if (!ScanStr(&name))
            return 0x1ebd;
        table.insert(std::make_pair(name, id));
    }
    return 0;
