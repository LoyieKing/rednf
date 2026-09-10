void CFieldLengthChecker::AddField(char const* query)
{
    // [b28d-56 判词·N] N82/N91：ORIG 0x0817? 623 条 vs 我们 611 条；find 重载已按
    // p167 修正（MISSING/EXTRA 各 6→0）；残余为 string 局部槽基差 8（ORIG -0x41 起
    // 非 4 对齐槽基）+ 4.4 对 string 临时析构多布 EH landing（P216 边界：callee 为
    // libstdc++ 不可加 throw()）。语义完成。
    // [F2d-C05② 甄别] 无命中形：gap=+12（show_func_diff orig=623 our=611，我们**少**）
    //   ⇒ 减指令型 F2 方向相反；残余 ORIG-only={mov×8,add×3,test,je,jmp,sub,jne,jbe}
    //   = string 局部槽基（ORIG -0x41 非 4 对齐）+ EH landing 族 ⇒ 维持 N82/N91。
    if (fieldLength_.empty()) {
        std::string s(query);
        std::string a;
        std::string b;
        std::string c;
        // [b28d-56 判词·P167] ORIG 0x0817? 六处调用 `find(char const*, size_type)`
        // （`_ZNKSs4findEPKcj`），旧码 `find(char)` 选 `find(char, size_type)`
        // 重载 ⇒ p167 MISSING/EXTRA 各 6。改为字符串字面量实参。
        size_t pos = s.find("(");
        a = s.substr(0, pos);
        b = s.substr(pos + 1, std::string::npos);
        pos = b.find(")");
        c = b.substr(0, pos);
        while ((pos = c.find(";")) != std::string::npos) {
            fieldNames_.push_back(c.substr(0, pos));
            c = c.substr(pos + 1, std::string::npos);
            fieldLength_.push_back(0);
        }
        fieldNames_.push_back(c);
        fieldLength_.push_back(0);
    }
    if (m_fieldNames_.empty()) {
        std::string s(query);
        std::string a;
        std::string b;
        std::string c;
        size_t pos = s.find("values(");
        a = s.substr(0, pos);
        b = s.substr(pos + 7, std::string::npos);
        pos = b.find(")");
        c = b.substr(0, pos);
        for (int i = 0; i < fieldLength_.size() && (pos = c.find(";")) != std::string::npos; ++i) {
            std::string val;
            if (c[0] == '\'') {
                pos = 0;
                do {
                    pos = c.find("'", pos + 1);
                    if (pos == std::string::npos)
                        return;
                } while (c[pos - 1] == '\\');
                ++pos;
                val = c.substr(1, pos - 2);
            } else {
                val = "";
            }
            trim(val);
            if (fieldLength_[i] < val.length()) {
                fieldLength_[i] = val.length();
            }
            c = c.substr(pos + 1, std::string::npos);
        }
    }
