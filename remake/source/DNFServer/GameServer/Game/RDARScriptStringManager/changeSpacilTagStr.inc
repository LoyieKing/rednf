bool RDARScriptStringManager::changeSpacilTagStr(std::string& s)
{
    // ORIG 0x8aa60b0：把 "<URL LINK>内文</URL LINK>" 转为 0x1c 内文 0x1d，
    // 尾部残留追加，最后补 "\n"。任何初始条件不满足返回 false 且不修改 s。
    // 逐条对齐要点：① 两处死拷贝 std::string(s)（ORIG 0x60d2/0x60e4，只析构
    // 不读，P184 照抄）；② OPEN/CLOSE 为**局部** const char*（ORIG -0x24/-0x20
    // 槽写入，非 static 立即数折叠）；③ find/rfind 结果截断存 short（ORIG
    // `mov %ax,-0x1c/-0x1a` + `cmpw $0xffff`，npos→-1）；④ 三处 substr 的
    // strlen 各写两次（ORIG 重复 call）；⑤ 正极性包裹 `!empty() && !empty()`
    // （ORIG `xor $1;test;je` 链，P214①）。
    if (s.empty())
        return false;

    std::string deadCopy1 = s;
    std::string deadCopy2 = s;
    const char* OPEN = "<URL LINK>";
    const char* CLOSE = "</URL LINK>";

    short posOpen = s.find(OPEN, 0);
    if (posOpen == -1)
        return false;
    short posClose = s.rfind(CLOSE, std::string::npos);
    if (posClose == -1)
        return false;
    if (posOpen == posClose)
        return false;

    std::string part1 = s.substr(0, posOpen);
    std::string partInner = s.substr(
        posOpen + std::strlen(OPEN), posClose - posOpen - std::strlen(OPEN));
    std::string partTail = s.substr(
        posClose + std::strlen(CLOSE), s.length() - posClose - std::strlen(CLOSE));

    if (!part1.empty() && !partInner.empty())
    {
        std::string out = part1;
        out += static_cast<char>(0x1c);
        out += partInner;
        out += static_cast<char>(0x1d);
        if (!partTail.empty())
            out += partTail;
        out += "\n";

        if (!out.empty())
        {
            s.clear();
            s = out;
            return true;
        }
    }
    return false;
