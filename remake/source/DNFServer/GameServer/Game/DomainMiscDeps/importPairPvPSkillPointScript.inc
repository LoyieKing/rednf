bool PvPSkillTreeParameterScript::importPairPvPSkillPointScript(
    const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    // ORIG 0x08a5e26a：row 构造在 slotIdx/jobIdx/slotsPerJob 预清之前 ⇒ 声明序在前。
    std::map<int, int> row;
    int slotIdx = 0;
    int jobIdx = 0;
    // ORIG 0x08a5e3a0 `mov -0xc(%ebp),%edx; cmp %edx,%eax` 从槽载比较 ⇒ 非 const
    // 具名局部（const 被 4.4 折成 `cmp $0x37,%eax` 立即数形）。
    int slotsPerJob = 0x37;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                 // ORIG 08a5e5eb：跳循环外 return true
        }
        if (token == "[skill point]")
        {
            for (;;)
            {
                // ORIG：先 (int) 转换（x87 fistpl 截断）后检查 ok；lrintf 是
                // round-to-nearest，对 .5 值语义不同（真差异）。
                int rounded = static_cast<int>(ScanFloat(&ok));
                if (!ok)
                {
                    break;
                }
                row.insert(std::make_pair(slotIdx++, rounded));
                if (static_cast<int>(row.size()) == slotsPerJob)
                {
                    m_map18.insert(
                        std::make_pair(jobIdx++, row));
                    row.clear();
                    slotIdx = 0;
                }
            }
            // ORIG 0x08a5e470/0x08a5e485 两次独立 `call size`（P48：O0 无 CSE，
            // 两次调用=源码两次；具名 left 多一槽使全帧 +8）。
            if (static_cast<int>(row.size()) != slotsPerJob &&
                static_cast<int>(row.size()) != 0)
            {
                return false;                      // 残行 → 失败（ORIG 08a5e49a）
            }
        }
        else if (token == "[version]")
        {
            // ORIG 0x08a5e4d4：`call ScanInt; mov %eax,%edx; mov %dl,0x50(%eax)`
            // 无具名 ver 局部（先存后查 ok，P204 顺序）。
            m_field50 = (char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[init skill tree]")
        {
            // ORIG 0x08a5e516/0x08a5e520 两槽预清（key=-0xb4 / val=-0xb8）⇒
            // 头部声明 + 循环内赋值；循环形 `for(;;){key=Scan; if(!ok)break; ...}`。
            int key = 0;
            int val = 0;
            for (;;)
            {
                key = ScanInt(&ok);
                if (!ok)
                {
                    break;                         // 段结束回主扫描
                }
                val = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                if (!m_map54.insert(std::make_pair(key, val)).second)
                {
                    return false;                  // ORIG local_2c!=1 路径
                }
            }
        }
    }
    return true;
