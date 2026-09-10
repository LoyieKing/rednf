// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFieldLengthChecker/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CFieldLengthChecker.cpp — ORIG 转写（df_game_r elf32-i386）
// AddField 双分支：静态 fieldLength_ 空 → 解析字段定义（'(' ')' ';' 分隔，填充
// 静态 fieldNames_/fieldLength_）；实例成员空 → 解析 "values(" 数据行，按列
// 更新静态 fieldLength_ 为实测最大长度（'...' 引号值去转义、trim）。
#include <iostream>
#include "CFieldLengthChecker.h"
#include "CEnvironment.h"

// [b28d-Lane16] TU 归属迁移（否则 GI 键名不匹配）：
// ORIG 本 TU（= SyncScript.cpp，`orig_tu_names.tsv:107`
// `086269a2 _GLOBAL__I__Z22GetSocketTypeCharactert`）中 `GetSocketTypeCharacter`
// @0x0861ca1b 是首个全局函数声明 ⇒ GI 键名取它；定义必须置于本文件最前。
// ORIG T 0x0861ca1b：socket 类型 → 字符。1→'1' 2→'2' 4→'3' 8→'4'
// 0x10→'5' 0xffef→'6' 0xffff→'7'，其余 0（决策树见 ORIG 反汇编）。
#include "CFieldLengthChecker/GetSocketTypeCharacter.inc"
{
    switch (type)
    {
    case 1:
        return '1';
    case 2:
        return '2';
    case 4:
        return '3';
    case 8:
        return '4';
    case 0x10:
        return '5';
    case 0xffef:
        return '6';
    case 0xffff:
        return '7';
    default:
        return 0;
    }
}

// fieldNames_/fieldLength_ 的权威定义在本 TU（判据 P186/P108 键名 + 地址邻接）：
// ORIG static_init @0x0862690c 依次构造 ioinit@0x94734d6、fieldNames_@0x94734a0、
// fieldLength_@0x94734ac，且与 `CFieldLengthChecker::AddField`@0x086272e8、
// `sync_script::DisplayMaxLength`@0x0861ca94、`GetSocketTypeCharacter`@0x0861ca1b
// 同属一个 TU（GI 键名 `_GLOBAL__I__Z22GetSocketTypeCharactert`）。
// 反向证据：CItemList TU 的 ORIG static_init @0x0851398b 只含 ioinit（26 条），
// 故两个静态定义不得留在 CItemList.cpp（否则本 TU GI 缺 2 对象、CItemList GI 多 2 对象，
// 两侧同时 DIFF）。
std::vector<std::string> CFieldLengthChecker::fieldNames_;
std::vector<unsigned int> CFieldLengthChecker::fieldLength_;

#include "CFieldLengthChecker/CFieldLengthChecker.inc"
{
}

#include "CFieldLengthChecker/dtor_CFieldLengthChecker.inc"
{
}

#include "CFieldLengthChecker/AddField.inc"
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
}
