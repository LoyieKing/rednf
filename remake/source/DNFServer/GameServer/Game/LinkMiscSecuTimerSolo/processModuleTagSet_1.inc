// processModuleTagSet(std::string*, int&)  ORIG T 0x0896a87b（RDARScriptSkill.cpp）
// 局部 static 开/闭标签对表（函数局部 _ZZ19processModuleTagSetPSsRiE13moduleNameTag，
// guard 惰性构造），5 组 {开,闭} 对：[expert job none] 对（idx 0）不参与主循环。
// i=1..4 命中开标签（arr[i][0]）写 out=i 返回 0；命中闭标签（arr[i][1]）返回 0；
// 否则返回 1。
// 2D 形证据（batch-12）：ORIG guard 段构造/清理链按 pair 记账（outer 计数 -0x54
// 初值 4、pair 基址步进 +8、清理外循环 `sub -0x54;shl $0x3` 步长 8 = 2×string），
// 主体 arr[i][0] 寻址为 `shl $0x3; add base`（stride 8 折叠）；[10] 平表 + `2*i`
// 表达式在 4.4.7 下不产生该形态。[5][2] 形整函数 438 条与 ORIG 逐条对位
// （/tmp/pt2.md diff 仅剩 guard 数据地址与字面量常量，AE 口径归一）。
// ===========================================================================
bool processModuleTagSet(std::string* tag, int& out)
{
    static const std::string moduleNameTag[5][2] = {
        {"[expert job none]", "[/expert job none]"},
        {"[magic enchant]",   "[/magic enchant]"},
        {"[alchemist]",       "[/alchemist]"},
        {"[disjointer]",      "[/disjointer]"},
        {"[doll controller]", "[/doll controller]"}
    };
    for (int i = 1; i <= 4; ++i)
    {
        if (*tag == moduleNameTag[i][0])
        {
            out = i;
            return 0;
        }
        if (*tag == moduleNameTag[i][1])
            return 0;
    }
    return 1;
}