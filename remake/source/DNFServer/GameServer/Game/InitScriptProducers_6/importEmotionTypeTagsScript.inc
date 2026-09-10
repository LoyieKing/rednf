bool importEmotionTypeTagsScript(const char* path, stEmotionTypeTagsInfo* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x10
    std::string unused;                 // ORIG -0x14（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x15

    if (out == 0)
    {
        return false;                               // ORIG 0x890decb 判空
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[EMOTION INFO]")
        {
            stOneEmotionData data;                  // ctor 0x891b560 + clear
            std::string str;                        // ORIG -0x1c（块内构造）
            int tag = ScanInt(&ok);
            if (!ok) { return false; }
            out->m_tags.push_back(tag);
            data.m_0 = tag;

            // [C03 实测] ORIG 0x0eb/0x103 的 `mov %eax,-0xc; mov -0xc,%eax; mov %eax,-0x40`
            // 是 4.1 的具名中转；我方补 `int v4 = ScanInt(&ok); data.m_4 = v4;` 后
            // ourN 239→241 且全槽整体深 4（新槽未复用 ORIG 的 -0xc）⇒ 反劣，已回退。
            data.m_4 = ScanInt(&ok);
            if (!ok) { return false; }
            if (!ScanStr(&data.m_8))
            {
                return false;
            }
            if (!ScanStr(&data.m_c))
            {
                return false;
            }
            int v = ScanInt(&ok);
            if (!ok) { return false; }
            // ORIG `mov -0x20(%ebp),%eax; cmp $0x1,%eax; sete %al; mov %al,-0x34(=data.m_10)`
            // ⇒ 直接 bool→字节字段赋值即复现 sete 形（勿写 `(v==1)?1:0`：那会产分支形）。
            data.m_10 = (v == 1);
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_11 = (v == 1);
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_12 = (v == 1);

            if (!ScanStr(&data.m_14))
            {
                return false;
            }
            for (;;)
            {
                if (!ScanStr(&str))
                {
                    break;                          // 列表终止（含 $END 缺失情形）
                }
                if (str == "$END")
                {
                    break;
                }
                data.m_18.push_back(str);
            }
            out->m_data.push_back(data);
        }
    }
