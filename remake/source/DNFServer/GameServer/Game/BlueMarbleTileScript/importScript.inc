bool BlueMarbleTileScript::importScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    std::string token;
    std::string value;
    int enterDungeon = 0;
    // [b28d-47] ORIG 0x088d50f9 循环头 `call ScanType; xor $1,%eax; test %al,%al;
    // jne <退出>`（P168a bool callee 裸取反形）⇒ 写 `if (!ScanType) break;`；
    // `while (ScanType(...))` 4.4 直产 `test %al; je`（少 xor）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[type]")
        {
            ScanStr(&value);
            m_type = toType(value);
        }
        else if (token == "[enter dungeon]")
        {
            ScanInt(&enterDungeon);
            if (enterDungeon != 0)
            {
                m_char4 = 1;
            }
        }
    }
    return 1;
