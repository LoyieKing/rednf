// ORIG 0x08134132：见头注。type==1→+0x10 vec；type==2→+0x1c vec；type==0→+0x04 vec；
// 其它 type 直接返回 false。逐项 CodePage::script2Database 转换 desc 后 sprintf 元组，
// 项间追加 ", "；任一项转换失败走 cMyTrace 错误日志并返回 false。
// [b28d-L8→b28d-SymRemnant P326 重试] 旧判 N82「帧差 0x85c vs 0x55c」已破：
// 帧差唯一来源 = **conv 是 char[0x400]（0x300 差额）**，非 0x100（ORIG 清零
// rep stosl 0x100 dwords @0x081341c6 实证）；tuple 声明须在循环体 conv 之后
// （-0x844 更深）；i=0 在 beginEv 后 ⇒ 迭代器出环具名 + postfix it++；trace
// 单语句临时。**残留 9 行** = switch lowering 末节点方向：ORIG `test; jne fail`
// +T0 fallthrough，4.4.7 对同语义 switch **必发** `test; je T0`+fail fallthrough
// （/tmp/swtest 最小探针实测），if-链形又退化为 mem 直比 `cmpl $N,0x10(%ebp)`
// （ORIG 为单 `mov 0x10,%eax` + 三连寄存器比较 = switch lowering 特征）⇒
// 两形各有不可替代半，封底 9 行。
// [P2S2STQuest 2026-09-11 prett 负结论] ORIG 在此函数引用
// `_ZZN12advancealtar10SyncScript18getItemValueStirng...E19__PRETTY_FUNCTION__`
// （size 0x96），我方用手写串（内容与 ORIG **逐字节相同**，pstr.py 核实）⇒ 仅缺
// 符号载体。**不可换 `__PRETTY_FUNCTION__`**：ORIG 串末参为 `std::tstring&`
// （std 内 typedef），本仓库只有**全局** `typedef std::string tstring`（PackSet.h:32）
// ⇒ 4.4 会打印 `std::string&`（现拼写）或 `tstring&`（若改拼写），两者都不等于 ORIG
// 串（探针 /tmp/p2/probe_ts.cpp 实测：全局 typedef 打印**不带** `std::` 限定）
// ⇒ 依 P8 prett 边界 1「宁可不换」，保留手写串。
bool SyncScript::getItemValueStirng(int ridableId, const BuyShopData& data,
                                    AdvanceAltarShopType::T type, std::string& out)
{
    const std::vector<BuyUpgradeData>* vec = 0;
    // ORIG 0x08134145..0x08134177 形：单次 `mov 0x10(%ebp),%eax` + cmp1/cmp2/
    // test 三连（4.4 **switch** lowering 特征；if-链会退化成 mem 操作数直比）。
    // [P2S2STQuest 2026-09-11] 叶子落点由 case **书写顺序**决定（/tmp/p2 探针实测）：
    // 首 case=T_0 ⇒ 末环 `test %eax,%eax; jne <default>` + case0 代码 fallthrough
    // （ORIG 0x08134172/0x08134174 形）；首 case=T_1（旧）⇒ `je L0; jmp <default>` 多 1 条。
    // 实测 verdict DIFF(gap -1) → **AE(gap +0)**，TU 四数 diff 1→0（无劣化）。
    switch (type)
    {
    case AdvanceAltarShopType::T_0: vec = &data.m_vecT0; break;
    case AdvanceAltarShopType::T_1: vec = &data.m_vecT1; break;
    case AdvanceAltarShopType::T_2: vec = &data.m_vecT2; break;
    default: return false;
    }
    const int total = (int)vec->size();
    // 配方⑥（同 InsertDescTable）：ORIG i=0 在 beginEv(0x0813419c) **之后**
    // 0x081341a4 ⇒ 迭代器出环具名；配方⑦：ORIG 尾 `movl $0x0,0x8(%esp)`+ppEv
    // ⇒ postfix `it++`。
    std::vector<BuyUpgradeData>::const_iterator it = vec->begin();
    int i = 0;
    for (; it != vec->end(); it++, ++i)
    {
        std::string s;                 // ORIG 循环体内多余局部（汇编构造/析构实证）
        // ORIG 清零 rep stosl 0x100 dwords = **1024B**（0x081341bb..0x081341cf，
        // 缓冲 -0x444）⇒ conv 是 char[0x400]，非 0x100（帧差 0x85c-0x55c=0x300
        // 的唯一来源）。
        char conv[0x400] = {0};
        // 槽位证据：ORIG tuple 基址 -0x844 比 conv(-0x444) **更深** ⇒ 4.4
        // 后声明更深的分配律下 tuple 声明须在 conv 之后（循环体内）。
        char tuple[0x400];
        if (!CodePage::script2Database(
                const_cast<char*>(it->m_strDesc.c_str()), conv))
        {
            // 配方⑩：ORIG 先求值 c_str→%edi / database→%esi / script→%ebx，
            // **其后**才 C1 ⇒ 单语句临时。
            cMyTrace(
                "static bool advancealtar::SyncScript::getItemValueStirng(int, const advancealtar::BuyShopData&, advancealtar::AdvanceAltarShopType::T, std::tstring&)",
                0xd36, 5)(
                "Error!!! %s->%s [%s][%s][%d]",
                CodePage::script(), CodePage::database(),
                it->m_strDesc.c_str(),
                "static bool advancealtar::SyncScript::getItemValueStirng(int, const advancealtar::BuyShopData&, advancealtar::AdvanceAltarShopType::T, std::tstring&)",
                0xd36);
            return false;
        }
        sprintf(tuple, "(%d,%d,%d,'%s')", ridableId, (int)type,
                it->m_nItemId, conv);
        out += tuple;
        if (i < total - 1)
        {
            out += ", ";
        }
    }
    return true;
}