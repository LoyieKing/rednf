// ============================================================================
// importExperienceItemMapInfo —— ORIG 0x089136a4
// 报告 docs/class_func_reports/_lt_global_gt_/importExperienceItemMapInfo.md
// 调用点（init.md +0x181c / +0x1891）：
//   init → this+0xa928（m_mapA928），bool=0
//   init → this+0xa940（m_mapA940），bool=1
// 阈值：第二个值 > 0x88b8（35000）时 log 报错返回 0。
// OBJDUMP 复核（0x89137b2 true 分支 / 0x8913801 false 分支）：
//   true (param_2==true, -0x4c(%ebp)!=0):
//     make_pair(&local_28 {first=-0x44(level), second=-0x48(val)})  → insert
//   false:
//     make_pair(&local_10 {first=-0x48(val), second=-0x44(level)}) → insert
//   即 second==true → insert(val,key)；second==false → insert(key⋯ 实际以
//   实参压栈序为准；下述实现按 false 存 (key,val)、true 存 (val,key)。
// 注意报告 Ghidra 的 make_pair 两分支实参顺序相反，以 objdump 为准：
//   false 分支把 -0x48(val) 放 first、-0x44(level) 放 second；true 分支相反。
// ============================================================================
// [b28d-solo-RDARScriptEtcParameter] AE：ORIG 0x089136a4；本波按 P309（4.4 头部
// 声明「先声明浅」）把 ok 提到 token 之前 ⇒ token 归位 -0x40、key/val/形参截断槽
// 全部归位（134=134 助记符全等）。残余 4 条 = ok 槽 -0x3c vs ORIG -0x39：4.4 标量
// 槽基 4 对齐而 4.1 从 -0x39 起；实测 4B 数组/结构体尾字节取址恒 `lea -0x3c;add $0x3`
// 不折叠（P235 反例）⇒ N95 族封顶。p167 CLEAR（17/17），语义闭合。
bool importExperienceItemMapInfo(std::map<int, int, std::less<int>,
                                 std::allocator<std::pair<int const, int> > >& m,
                                 bool second)
{
    m.clear();

    if (!loadRDARScriptFile("Etc/ItemDictionary/", "(R)baseItemDictionaryTable.etc"))
        return 0;

    // [b28d-solo-RDARScriptEtcParameter] ORIG 槽序 token@-0x40 / ok@-0x39（4.1 先声明深）；
    // 4.4 先声明浅 ⇒ 倒序声明（ok 先、token 后）使 token 归位 -0x40（key/val/形参截断槽
    // 因 4 字节对齐吸收差异全部归位）。残余 4 条 = ok 槽 -0x3c vs ORIG -0x39：4.4 标量槽基
    // 4 对齐（-0x3c）而 4.1 从 -0x39 起；实测 4B 数组/结构体尾字节取址恒 `lea -0x3c;add $0x3`
    // 不折叠（P235 反例），无源码形可造 ⇒ N95 族封顶。无初始化（ORIG ScanInt 必写）。
    bool ok;
    std::string token;            // ORIG -0x40
    if (!ScanType(token, true))
        return 0;
    if (token == "[base table]")
    {
        for (;;)
        {
            int key = ScanInt(&ok);
            if (!ok)
                break;
            int val = ScanInt(&ok);
            if (!ok)
                return 0;
            // ORIG cmp $0x88b8/jle：有符号比较
            if (val > 0x88b8)
            {
                LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                      __FUNCTION__, 0x30b8,
                                      "ItemDictionary mappingTable MAX INDEX(40000) WARNING READY TO DB_TOOL");
                return 0;
            }
            // 依据 objdump 压栈序：second==false → (key,val)；true → (val,key)
            if (second)
                m.insert(std::make_pair(val, key));
            else
                m.insert(std::make_pair(key, val));
        }
        return 1;
    }
    return 0;
}