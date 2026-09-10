int importSpPerLevelReferenceTable(const char* path, stSpPerLevelTable& table)
{
    bool ok = false;
    std::string token;
    int level;
    int value;

    if (!loadRDARScriptFile("", path))
        return 0x2948;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[sp table]")
        {
            level = 0;
            value = 0;
            table.reserve(200);
            table.push_back(value);                    // 等级 0 占位（int→unsigned 右值）
            for (;;)
            {
                level = ScanInt(&ok);                  // 等级位（丢弃，索引即位）
                if (!ok)
                    break;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                table.push_back(value);                // 仅数值位入表
            }
        }
    }
    return 0;
