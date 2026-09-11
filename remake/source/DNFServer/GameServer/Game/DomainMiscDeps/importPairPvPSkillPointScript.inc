// ORIG 0x08a5e226（0x42b 字节）逐分支转写：
//   [skill point]：ScanFloat 四舍五入为 int（fstcw/fldcw 就近偶数舍入），
//   按 (jobIdx, 0..0x36) 分行插入 m_map18；残行（非 0 非 0x37）→ 失败。
//   [version]：单字节版本写 +0x50。[init skill tree]：(key,val) 入 m_map54，
//   重复键 → 失败。
// b28d-solo-DomainMiscDeps: DIFF 残余（p167 CLEAR 33/33；N82/N93 槽向）
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
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
    // ORIG 0x08a5e3a0 / 0x08a5e2f2 两处比较段均为 `mov -0xc(%ebp),%edx; cmp %edx,%eax`
    // （槽→寄存器两载）。L3Free25to36 实测：**变量保持 int + 比较式去掉
    // `static_cast<int>(row.size())`** ⇒ 混合符号（unsigned size() vs int 槽）触发
    // 4.4 先把槽转 unsigned 载入 %edx ⇒ 逐条 AE（275/275）。
    // 反例：`static_cast<int>(row.size()) == slotsPerJob` 两侧同型 ⇒ 折
    // `cmp -0xc(%ebp),%eax` 单条（gap +2）；`unsigned` 槽同样折 mem cmp，均无效。
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
                if (row.size() == slotsPerJob)
                {
                    m_map18.insert(
                        std::make_pair(jobIdx++, row));
                    row.clear();
                    slotIdx = 0;
                }
            }
            // ORIG 0x08a5e470/0x08a5e485 两次独立 `call size`（P48：O0 无 CSE，
            // 两次调用=源码两次；具名 left 多一槽使全帧 +8）。
            if (row.size() != slotsPerJob &&
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
}