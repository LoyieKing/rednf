// b28d-solo-N 封顶（EH 尾段）：前 185 条逐条全等（含两处死拷贝 std::string(s)、
// 局部 OPEN/CLOSE、find/rfind 截断 short、三处 substr 的重复 strlen）；残差全在
// 函数尾部 EH cleanup 决策链——ORIG 每析构点前发
// `test %esi,%esi; jne L; mov $0,%esi; jmp; L: mov $1,%esi`（4.1 landing-pad 决策），
// 4.4 直接顺序析构，属 N91/N104 族（跨版本 EH 结构，无源形解）。返回类型已按
// P159 由 void 改 bool（ORIG 尾 `mov %ebx,%eax`，ebx=1/0）。ORIG 0x8aa60b0。
// [M2S5StringRaw 2026-09-11 复核] gap +14 逐条归因完成：ORIG 独有 3 组 4.1
// landing-pad 决策链（`test %esi,%esi; jne L; mov $0,%esi; jmp L2; L: mov $1,%esi`，
// 6 条/组 ×3 = 18）对 4.4 的 3 条单 `jmp` ⇒ 18−3 = 15 ≈ 全部 gap（余 1 条对齐
// nop 口径差）。P216 方向为「我方多 _Unwind_Resume」，与本件反向不适用；无源形
// 可控项 ⇒ N91/N104 封顶维持。
// [M2S5E 2026-09-11 复验·新旋钮亦封顶] 本轮按「Ghidra bVar1 被复用两次」假说
//   在探针（/tmp/probeRDAR，4.4.6 后端 + 真 TU 旗标）复现 ORIG 结构：
//   `bool ret=false; bool ok = !part1.empty() && !partInner.empty(); if (ok) {
//    std::string out=…; ok = !out.empty(); if (ok) { s.clear(); s=out; ret=true; } }
//    return ret;` ⇒ 255 条（我方现形 259、ORIG 274），**零归一化组** ⇒ 具名 bool
//   在 4.4 -O0 恒落栈（`movzbl -0x9(%ebp),%ebx`），永不复现 ORIG 的 esi sink。
//   结论：ORIG 0x8aa631e..0x8aa63e7 的三组 6 条归一化
//   （`test %esi,%esi; jne L; mov $0x0,%esi; jmp L2; L: mov $0x1,%esi`）是 4.1 -O0
//   把跨 EH-cleanup 的临时保活到寄存器（esi）并在每个析构点重建的行为；4.4 -O0
//   只用栈槽/单次消费（我方 259 条中仅 ~out 后 1 处 `test %esi,%esi; je; mov $0,%ebx`）
//   ⇒ 源形不可翻，gap +14 封顶维持。
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
}