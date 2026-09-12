void CFieldLengthChecker::AddField(char const* query)
{
    // [b28d-56 判词·N] N82/N91：ORIG 0x0817? 623 条 vs 我们 611 条；find 重载已按
    // p167 修正（MISSING/EXTRA 各 6→0）；残余为 string 局部槽基差 8（ORIG -0x41 起
    // 非 4 对齐槽基）+ 4.4 对 string 临时析构多布 EH landing（P216 边界：callee 为
    // libstdc++ 不可加 throw()）。语义完成。
    // [F2d-C05② 甄别] 无命中形：gap=+12（show_func_diff orig=623 our=611，我们**少**）
    //   ⇒ 减指令型 F2 方向相反；残余 ORIG-only={mov×8,add×3,test,je,jmp,sub,jne,jbe}
    //   = string 局部槽基（ORIG -0x41 非 4 对齐）+ EH landing 族 ⇒ 维持 N82/N91。
    // [N6S3 2026-09-11 复核·封顶] 件级读数（N1 口径 /tmp/n1_v.py）：verdict=DIFF
    //   orig_insns=623 our=611 gap=+12；残差形 = ORIG `lea -0x41(%ebp)` /
    //   `lea -0x48(%ebp)` / `lea -0x4c(%ebp)` / `lea -0x50(%ebp)` 四个 string
    //   槽（首槽 -0x41 **非 4 对齐**）vs 我方 `-0x49/-0x50/-0x54/-0x58/-0x5c`
    //   （低 8 + 多一槽），且构造/析构序成对平移。定性证据（两把独立尺子
    //   均为空）：p167all.py CFieldLengthChecker ⇒ AddField **无 MISSING/EXTRA
    //   call**（差集只出现在 _Z41__static_initialization_and_destruction_0ii 与
    //   uninitialized_copy 模型件）；strall.py ⇒ AddField 无真串差异。
    //   ⇒ 无缺失语义调用、无缺失/多余字面量；余项为 -O0 槽分配（string 4 字节
    //   槽 + 非对齐基）与 libstdc++ string 临时析构的 EH landing 排布，属编译器
    //   内部分配序，函数体内无可落地旋钮（P216 边界：callee 为 libstdc++ 不可
    //   加 throw()）⇒ 维持封顶，不做噪声改动。
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
        // [R5a16 ⑤] ORIG 0x086275a3/0x08627a65 `movl $0x8ce2dec,0x4(%esp)`
        //   ⇒ 该 VA 的 raw 字节 = `2c 00`（","），非 `;`（0x8ce2df7 = `3b 00` 未被引用）。
        //   两侧字符串多重集：ORIG{","×2} vs 我方{";"×2} ⇒ 分隔符真错，改为 ","。
        while ((pos = c.find(",")) != std::string::npos) {
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
        for (int i = 0; i < fieldLength_.size() && (pos = c.find(",")) != std::string::npos; ++i) {
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
}