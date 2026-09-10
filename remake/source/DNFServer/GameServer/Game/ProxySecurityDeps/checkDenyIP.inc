bool checkDenyIP(const char* innerIp, const char* peerIp)
{
    if (innerIp == 0 || peerIp == 0)
        return false;
    if (innerIp[0] == '\0' || peerIp[0] == '\0')
        return false;
    if (strncmp(innerIp, peerIp, 0x10) == 0)
        return false;
    std::string a = cutIPdclass(innerIp);
    std::string b = cutIPdclass(peerIp);
    if (a == b)                                   // ORIG 0x081f4556 eq(string,string)
        return false;
    // ORIG 0x081f457f..0x081f45e6：a/b 各再做 find('.',0)+erase(pos,npos)
    // （取到第一个 '.'，即仅保留首段）。**erase 返回值赋回自身**：ORIG 汇编
    // 0x081f459b `mov %eax,0x4(%esp)`（erase 返回值作 operator= 第 2 参）+
    // 0x081f45a5 call `_ZNSsaSERKSs`（erase 返 string& 左值 ⇒ 自我赋值走
    // operator=(const string&)）；p167 旧读数 MISSING×2 即此处漏赋值。
    a = a.erase(a.find('.', 0), std::string::npos);
    b = b.erase(b.find('.', 0), std::string::npos);
    // ORIG 0x081f45f9..0x081f4635：短路 `||` 双 call + bool 物化（081f4619 mov$1 /
    // 081f4620 mov$0 汇聚 → test %al）⇒ 原码是 `if (a=="0" || b=="0") return false;`
    // ——旧重建 `return b == "0";` 在 b=="0" 时返回 true，与 ORIG 相反（真语义 bug）。
    if (a == "0" || b == "0")
        return false;
    return true;
