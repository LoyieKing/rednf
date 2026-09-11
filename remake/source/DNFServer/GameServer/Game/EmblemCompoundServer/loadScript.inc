// ============================================================================
// loadScript —— ORIG T 0x088b154a。
// "Etc/EmblemRenewal/" + "emblem_compound_jpn.etc"；三节：
//   [emblem compound cost]   grade value         → m_cost（重复 grade → printf+-2）
//   [emblem compound result] grade 4×short flag  → m_result（new Result(0xa)；
//                              重复 grade → delete+printf+-2）
//   [emblem list]            listIdx itemIndex   → m_list[listIdx].push_back
// 循环形态与 ORIG 相同：节内由 ScanTypeOrInt 取数值项（isType==false&&isInt==true
// 才处理）；遇 TYPE/EOF 退出后回到外层 ScanType(token,true)。查重均为
// end()→find()→operator==、命中则插入否则报错的分支形态（与 ORIG 一致）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
// [b28d-solo-EmblemCompoundServer] loadScript 判词（ORIG 0x088b154a，273 insn）。
// 已修：外层 while→for(;;)+头部 break 复现 4.1 条件块前置布局（P283/P164，消 ~30 行
// 布局差）；new Result()（P90 value-init）；list 节 int 槽 + prvalue 转换临时
// （p167 实证 ORIG 调 push_back(T&&) `EOm`，具名左值会错选 `RKm`；修正后 p167 CLEAR
// 35/35）。
// 残余 N：ORIG +0x1c2 的 value-init 前置清零为 4.1 循环展开
// （`mov;mov;mov;movw;mov;movw;…;je;add;jmp` 清 8B + movb 尾，9 条），4.4 恒
// 展开 4×movw+movb（探针 /tmp/nr_probe.cpp `new R()` 实证，5 条）⇒ N78/跨版本
// 展开形态不可复现；gap 6 全在此段。其余助记符序列逐条同构（mnem-diff 0 块）。
// [G21A 2026-09-11] 槽平移修形（保留）：ORIG 局部组(token/isType/isInt)整体比我方
//   深 8（-0x4c/-0x4d/-0x4e vs -0x44/-0x45/-0x46）⇒ 函数头加 `char framePad8[8]`（P31）
//   后该组逐值对齐，diff 330→144 行（-56%），gap 不变（+4 全在 value-init 展开段）。
//   已试并回滚：framePad12（帧 >0x7f，`sub $0x70` 变 `add $<A>,%esp`，反劣）。
int EmblemCompound::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "emblem_compound_jpn.etc"))
        return -1;

    char framePad8[8];
    (void)framePad8;
    std::string token;
    bool isType = false;
    bool isInt = false;
    // ORIG +0x40：`jmp cond` + 条件块在循环体之前（4.1 while 布局）⇒
    // for(;;)+头部 break 复现（P283/P164）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[emblem compound cost]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                int value = ScanInt((bool*)0);   // ORIG +0xb6：value 先扫再查重
                std::map<int, int>::iterator endIt = m_cost.end();
                if (m_cost.find(grade) == endIt)
                    m_cost.insert(std::map<int, int>::value_type(grade, value));
                else
                {
                    printf("[emblem compound cost] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem compound result]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG +0x1c2：new Result()（P90 value-init 前置清零循环 2×movw+movb，
                // 覆盖 +0..+8，跳过 +9 对齐 pad）后逐字段写入。
                Result* result = new Result();
                result->m_listIdx = static_cast<short>(ScanInt((bool*)0));
                result->m_itemIndex = static_cast<short>(ScanInt((bool*)0));
                result->m_itemCount = static_cast<short>(ScanInt((bool*)0));
                result->m_field6 = static_cast<short>(ScanInt((bool*)0));
                // [b23S3C] ORIG 0x088b17cx 段 `call ScanInt; mov %eax,-0x14(%ebp);
                // mov -0x60(%ebp),%eax; cmpl $0x1,-0x14(%ebp); sete %dl; mov %dl,
                // 0x8(%eax)` ⇒ 扫描结果有**具名 int 槽**（P0 反向：ORIG 有槽我方
                // 无）。单表达式 `(ScanInt()==1)` 恒 `cmp $0x1,%eax; sete %al`
                // 直比（少 2 条）⇒ 拆出具名局部复现槽 + 内存比较。
                int selectableValue = ScanInt((bool*)0);
                result->m_selectable = (selectableValue == 1);
                std::map<int, Result*>::iterator endIt = m_result.end();
                if (m_result.find(grade) == endIt)
                    m_result.insert(std::map<int, Result*>::value_type(grade, result));
                else
                {
                    ::operator delete(result);
                    printf("[emblem compound result] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem list]")
        {
            for (;;)
            {
                int listIdx = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG +0x36c：ScanInt 结果先落 int 槽(-0xc)，再转 unsigned long
                // 转换临时槽(-0x18)；p167 实证 ORIG 调 push_back(T&&)（EOm）⇒
                // 实参必须是 prvalue（禁具名左值，否则选 RKm 左值重载）。
                int itemIdx = ScanInt((bool*)0);
                // ORIG 直接 this + listIdx*12 + 0x30 索引（无边界检查，忠实保留）。
                m_list[listIdx].push_back(static_cast<unsigned long>(itemIdx));
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
}