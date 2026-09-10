// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_Script/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Arad_Script.h"

// ============================================================================
// [b28d-64 判词汇总 · 泳道64] 本 TU 4 件（ORIG 地址见各报告 /tmp/b28d_lane_64_result.tsv）：
//   AradJumping_Script::importScript 0x089... 92/102 N封顶：P167 CLEAR 39/39；
//     残余 = cMyTrace 字符串重定位 + nop 岛落点 + %esi/%ebx 实参保活序（N80/N7）。
//   Arad_Script::Load(PKc)           418/421 N封顶：P167 CLEAR 39/39；槽序/寄存器
//     （本 TU 路由 -O2 + gnu++0x，ORIG 为 -O0）。
//   Arad_Script::Load(PKc, S1)       397/400 N封顶：P167 CLEAR 31/31；同上。
//   ARAD::SCRIPT::importAradEventScript 340/360 N封顶：P167 EXTRA
//     SEventPeriodData/SRewardPeriodData::~D1 ×2（4.4 对 vector 元素/临时多发析构 + EH）；
//     MISSING 空 ⇒ 语义完成、N91 封顶。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>

// [moved] include 区统一收敛到文件头（中部 include 在 lsd44 链下会污染后续标准头）

#include "Arad_Script/Arad_Script.inc"
{
}

#include "Arad_Script/Arad_Script_1.inc"
{
    // ORIG 0x0817b580：mov 0xc(%ebp),%eax; mov %eax,(%esp); call make_file_path
    // → mov 0x8(%ebp),%edx; add $0x20,%edx; mov %eax,0x4(%esp); mov %edx,(%esp);
    // call std::string::append(char const*)@0x08708000（nm 实证）。原实现写
    // pathC_ = full 生成 operator=(char const*)@0x08708060 且多一个 full 槽（P0：
    // ORIG 无槽往返 ⇒ 右值内联）。
    pathC_.append(make_file_path(path));
}

#include "Arad_Script/GetRowNum.inc"
{
    return usedRows_;
}

#include "Arad_Script/dtor_Arad_Script.inc"
{
    // 修正：ORIG D1（0x0817be03 range 内）delete[] 前带显式 null 守卫
    // （mov 0x14 reload + test + je 4 条），恢复 if 形与 ORIG 对位。
    if (cells_ != 0)
        delete[] cells_;
}

// [b28d-solo-Arad_Script] Create verdict=AE（fresh .o 66/66 ndiff=0）：三处形全等——
// ①memset size 实参两段 `mov 0x10(%eax),%eax; mov %eax,%edx` 由 `int bufBytes_`
// 的 int→size_t 转换复现（探针 fb/fk）；②store 段 `shl;mov` 由 RHS `* 0x80u`
// 与 int 字段异型复现（探针 gb）；③new[]/map clear 段原形。连带 ExpandCol/ExpandRow
// 同时 AE（RHS 异型 `unsigned<<7` / 同型 `*128` 两向开关，探针 ga/gd）。
#include "Arad_Script/Create.inc"
{
    // ORIG 0x0817b72a 头部 test;jne+test;je 双检 = 源形 `if (cells_ == 0)
    // delete[] cells_;`（外层 ==0 守卫 + delete[] 编译器自带 null 守卫）。
    // [b28d-solo] 载入中转由 `(int)(cols_ * (unsigned int)rows_) * 0x80u` 复现
    // （探针 gb）：ORIG 0x0817b768 `mov (%eax),%eax; mov %eax,%edx` +
    // 0x0817b775 `shl $7,%eax; mov %eax,%edx` 逐条全等。
    // [b28d-solo] memset size 实参两段（ORIG 0x0817b7aa `mov 0x10(%eax),%eax;
    // mov %eax,%edx`）由字段 `int bufBytes_` 的 int→size_t 转换复现（探针 fb）；
    // 旧 unsigned 字段恒单段。字段类型为 int 后本函数三处与 ORIG 全等。
    if (cells_ == 0)
        delete[] cells_;
    cols_ = 0x20;
    rows_ = 0x100;
    bufBytes_ = (int)(cols_ * (unsigned int)rows_) * 0x80u;
    cells_ = new char[bufBytes_];
    if (cells_ == 0)
        return 0;
    std::memset(cells_, 0, (size_t)bufBytes_);
    usedCols_ = 0;
    usedRows_ = 0;
    colIndex_.clear();
    rowIndex_.clear();
    return 1;
}

#include "Arad_Script/ExpandCol.inc"
{
    // [b28d-solo] ORIG 0x0817b816 `mov 0x4(%eax),%eax; mov %eax,%edx` 载入中转 +
    // `shl $7,%eax; mov %eax,%edx`：RHS 须与 int 字段异型（unsigned 移位，探针 ga）。
    cols_ = cols_ + 0x20;
    bufBytes_ = (unsigned int)(rows_ * (unsigned int)cols_) << 7;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    std::memset(n, 0, (size_t)bufBytes_);
    delete[] cells_;
    cells_ = n;
    return 1;
}

#include "Arad_Script/ExpandRow.inc"
{
    // [b28d-solo] ORIG 0x0817b892 shl 段为 `mov %eax,%edx; shl $0x7,%edx`（先搬后移，
    // 与 Create 的 `shl;mov` 相反）⇒ RHS 与 int 字段**同型**（int 常量 128，探针 gd）；
    // `* 128u` 异型反产 `shl;mov`（探针 ge）。memset size 段 ORIG `mov m,%eax;
    // sub nsz,%eax; mov %eax,%edx` 为 int-int 减法（字段改 int 后 cast 可去）。
    rows_ = rows_ * 2;
    bufBytes_ = rows_ * cols_ * 128;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    int nsz = bufBytes_;
    std::memset(n + nsz, 0, (size_t)(bufBytes_ - nsz));
    std::memcpy(n, cells_, (size_t)nsz);
    delete[] cells_;
    cells_ = n;
    return 1;
}

#include "Arad_Script/make_file_path.inc"
{
    static char full_path[0x200];
    std::strcpy(full_path, file);
    return full_path;
}

#include "Arad_Script/GetIdx4Col.inc"
{
    std::map<std::string, int>::iterator it = colIndex_.find(std::string(name));
    if (it != colIndex_.end())
        return it->second;
    return -1;
}

#include "Arad_Script/GetIdx4Row.inc"
{
    std::map<std::string, int>::iterator it = rowIndex_.find(std::string(name));
    if (it != rowIndex_.end())
        return it->second;
    return -1;
}

#include "Arad_Script/GetInt.inc"
{
    // ORIG 0x0817c6e7 lea 形（imul→mov%ecx→mov col→lea (%ecx,%eax,1)→shl→lea）
    // 复现写法：索引拆两乘积 `(cols*row)*0x80 + col*0x80`——4.4 强度归约后逐条
    // 同形（/tmp/l0_probe.cpp GetInt6 实证，batch-25c Lane0 撤销 N18a 封顶归因）。
    char* local_c;
    local_c = cells_ + (cols_ * row) * 0x80 + col * 0x80;
    return std::atoi(local_c);
}

#include "Arad_Script/GetInt_1.inc"
{
    return GetInt(GetIdx4Row(row), GetIdx4Col(col));
}

#include "Arad_Script/GetInt_2.inc"
{
    return GetInt(GetIdx4Row(row), col);
}

#include "Arad_Script/GetInt_3.inc"
{
    return GetInt(row, GetIdx4Col(col));
}

#include "Arad_Script/GetFloat.inc"
{
    char* local_c;
    local_c = cells_ + (cols_ * row) * 0x80 + col * 0x80;
    return (float)std::atof(local_c);
}

#include "Arad_Script/GetFloat_1.inc"
{
    return GetFloat(GetIdx4Row(row), GetIdx4Col(col));
}

#include "Arad_Script/GetFloat_2.inc"
{
    return GetFloat(GetIdx4Row(row), col);
}

#include "Arad_Script/GetFloat_3.inc"
{
    return GetFloat(row, GetIdx4Col(col));
}

#include "Arad_Script/GetDouble.inc"
{
    char* local_c;
    local_c = cells_ + (cols_ * row) * 0x80 + col * 0x80;
    return std::atof(local_c);
}

#include "Arad_Script/GetDouble_1.inc"
{
    return GetDouble(GetIdx4Row(row), GetIdx4Col(col));
}

#include "Arad_Script/GetDouble_2.inc"
{
    return GetDouble(GetIdx4Row(row), col);
}

#include "Arad_Script/GetDouble_3.inc"
{
    return GetDouble(row, GetIdx4Col(col));
}

#include "Arad_Script/GetString.inc"
{
    return cells_ + (cols_ * row) * 0x80 + col * 0x80;
}

#include "Arad_Script/GetString_1.inc"
{
    return GetString(GetIdx4Row(row), GetIdx4Col(col));
}

#include "Arad_Script/GetString_2.inc"
{
    return GetString(GetIdx4Row(row), col);
}

#include "Arad_Script/GetString_3.inc"
{
    return GetString(row, GetIdx4Col(col));
}

#include "Arad_Script/SetInt.inc"
{
    std::sprintf(cells_ + (cols_ * row) * 0x80 + col * 0x80, "%d", value);
}

#include "Arad_Script/SetInt_1.inc"
{
    SetInt(GetIdx4Row(row), GetIdx4Col(col), value);
}

#include "Arad_Script/SetInt_2.inc"
{
    SetInt(GetIdx4Row(row), col, value);
}

#include "Arad_Script/SetInt_3.inc"
{
    SetInt(row, GetIdx4Col(col), value);
}

#include "Arad_Script/SetString.inc"
{
    std::strcpy(cells_ + (cols_ * row) * 0x80 + col * 0x80, value);
}

#include "Arad_Script/SetString_1.inc"
{
    SetString(GetIdx4Row(row), GetIdx4Col(col), value);
}

#include "Arad_Script/SetString_2.inc"
{
    SetString(GetIdx4Row(row), col, value);
}

#include "Arad_Script/SetString_3.inc"
{
    SetString(row, GetIdx4Col(col), value);
}

// ---------------------------------------------------------------------------
// Arad_Script::Load(dir, file) —— ORIG _ZN11Arad_Script4LoadEPKcS1_ @ 0x0817b950
// 报告 docs/class_func_reports/Arad_Script/Load.md（汇编 + Ghidra C 全文转写）。
// 语义：内存流定位 "[%s]"(file) 节标记 → 列名行解析（'/' 分段写入 colIndex_，
// 末尾统一 ++usedCols_ 并按需 ExpandCol）→ 数据块行解析（首 token 写 rowIndex_，
// 其余 strcpy 进 cells_，行满 usedCols_ 后跳行尾，按需 ExpandRow）。
// EOF(-1) 在节/列行段直接失败；数据段空 token 视作 EOF。失败路径不 fclose
// （本重载无文件句柄）。
// 【残差豁免·跨版本 N 类】ORIG 397 / ours 400（gap 24→3）。语义与调用序逐段对齐；
// 剩余差异均为 gcc4.1↔4.4 结构性产物：①do-while/while 回边 `je exit;jmp top` vs
// `jmp top;nop;jne top`（负例表"空 else nop;jmp 岛 vs jump-threading"，ORIG
// 0x0817bc7d/0x0817ba70）；②key2 写回 `mov %eax,%edx` 缓存 ref（4.1 regmove 方向，
// N18a，ORIG 0x0817bbf4）；③cells_ 寻址 `lea (%ecx,%eax,1)` vs `add mem,%eax`
// （N4 折叠，ORIG 0x0817bdbc）；④string/allocator 临时栈槽偏移 -0x54/-0x4d… vs
// -0x44/-0x3d…（NEAR 级槽序）；⑤EH landing-pad 块序（"EH 双份 dtor epilogue vs
// merge"）。源码层无合法写法翻转以上各向。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_Script] Load(PKc,PKc) verdict=N 封顶（fresh .o 387/387 gap=0,
// p167 CLEAR 31/31，ndiff 196 中 139 条为块内临时对象槽号整体深 0x10：4.1 把循环内
// allocator/string 对放在 -0x4d/-0x54 贴近 colBuf，4.4 放在 -0x3d/-0x44 贴近 ebp
// —— 4.1 声明序 vs 4.4 首次使用序的槽方向差，N82/N93 族（P212 全头部声明法不适用
// 于依赖运行期实参构造的块内对象）；其余为 4.1 空块 nop;jmp 岛/块序（N104/N112/N121）
// 与 `mov %eax,%ecx; lea` vs `add`（N18a/P30）。语义完成、byte 层 N 封顶。
// [L3Free25to36.L3AUserHistoryAradScript 2026-09-11 · 再补一形] 列名行循环 `for(;;)`
//   改 `while (true)`（语义等价）复编后：ndiff 196 不变、ORIG-only/OURS-only 助记符差
//   不变（仍 {mov,je,lea} vs {add,jne,jmp}）⇒ 回滚。4.4 对本处两种书写发同一 CFG。
#include "Arad_Script/Load.inc"
{
    if (!Create())
        return 0;
    char* pStart = 0;
    char tag[0x400] = { 0 };
    std::sprintf(tag, "[%s]", file);
    unsigned int i = 0;
    unsigned int tokenLen = 0;
    int pos = 0;
    int tagLen = (int)std::strlen(tag);
    int c = 0;
    int rowIdx = 0;
    int colLen = 0;
    int unused = 0;
    (void)unused;

    // 节标记滑动匹配（ORIG +0xad..+0x101：空白/可打印外字符忽略）
    while (true)
    {
        c = (int)(signed char)*dir++;
        if (c == -1)
            return 0;
        if (c <= 0x20)
        {
        }
        else if (c > 0x7e)
        {
        }
        else if ((int)(signed char)tag[pos] == c)
        {
            ++pos;
            if (pos == tagLen)
                break;
        }
        else
            pos = 0;
    }

    // 列名行（ORIG +0x103..+0x34d）：'/' 分段键各写 colIndex_[seg]=usedCols_，
    // 末段写完后 usedCols_ 自增一次并按需 ExpandCol。
    char colBuf[0x200] = { 0 };
    colLen = 0;
    for (;;)
    {
        c = (int)(signed char)*dir++;
        if (c == -1)
            return 0;
        if (c == 0x20 || c == 9 || c == 10 || c == 13)
        {
            if (colLen > 0)
            {
                colBuf[colLen] = 0;
                colLen = 0;
                pStart = colBuf;
                tokenLen = (unsigned int)std::strlen(colBuf);
                for (i = 0; i < tokenLen; ++i)
                {
                    if (colBuf[i] == '/')
                    {
                        colBuf[i] = 0;
                        {
                            std::allocator<char> alloc;
                            std::string key(pStart, alloc);
                            colIndex_[key] = usedCols_;
                        }
                        pStart = colBuf + i + 1;
                    }
                }
                {
                    std::allocator<char> alloc;
                    std::string key(pStart, alloc);
                    colIndex_[key] = usedCols_;
                    usedCols_ = usedCols_ + 1;
                }
                if (usedCols_ + 1 >= cols_)
                {
                    if (!ExpandCol())
                        return 0;
                }
            }
        }
        else
        {
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (c == 10)
            break;
    }

    // 数据块行（ORIG +0x34f..+0x544）：rowIdx=-1 起；首 token 建行名，
    // 之后逐格 strcpy；行满后跳至行尾/EOF；行结束但 rowIdx!=usedCols_ → 失败。
    while (true)
    {
        rowIdx = -1;
        colLen = 0;
        while (true)
        {
            c = (int)(signed char)*dir++;
            if (c == 0x20 || c == 9 || c == 10 || c == 13 || c == -1)
            {
                if (colLen > 0)
                {
                    colBuf[colLen] = 0;
                    colLen = 0;
                    if (rowIdx == -1)
                    {
                        {
                            std::allocator<char> alloc;
                            std::string key(colBuf, alloc);
                            rowIndex_[key] = usedRows_;
                        }
                    }
                    else
                    {
                        std::strcpy(cells_ + ((rowIdx + (usedRows_ * cols_)) << 7), colBuf);
                    }
                    ++rowIdx;
                    if (rowIdx == usedCols_)
                    {
                        while (c != -1 && c != 10)
                            c = (int)(signed char)*dir++;
                    }
                }
                else
                {
                    c = -1;
                }
                if (c == 10 || c == -1)
                    break;
                continue;
            }
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (usedCols_ != rowIdx)
            return 0;
        ++usedRows_;
        if (usedRows_ + 1 >= rows_)
        {
            if (!ExpandRow())
                return 0;
        }
        if (c == -1)
            break;
    }
    return 1;
}

// [b28d-solo-Arad_Script] Load(PKc) verdict=N 封顶（fresh .o 408/408 gap=0,
// p167 CLEAR 39/39）。残余同双参版：块内 allocator/string 临时槽整体差 0x10
// （N82/N93 槽方向）、4.1 空块 nop;jmp 岛与块序（N104/N112/N121）、
// `mov %eax,%ecx; lea (%ecx,%eax,1)` vs `add`（N18a/P30；rowIdx 为 int 且需与 -1
// 比较，无法用 P211 同型化翻 add-vs-lea）。语义完成、byte 层 N 封顶。
#include "Arad_Script/Load_1.inc"
{
    // ORIG _ZN11Arad_Script4LoadEPKc @ 0x0817bea4（报告缺失：docs 同名 Load.md
    // 仅含双参重载；本函数按 objdump [0x0817bea4,0x0817c44d] 全量转写）。
    // 与双参版同构，差异：字符来源为 fgetc(fp)；节匹配循环每轮先读
    // pathC_[scanIdx++] 入死槽（ORIG +0x82..+0x9d 真实调用，原码遗留行为，
    // 按指令恢复）；失败 return 0 均不 fclose（ORIG 泄漏语义），仅成功路径
    // 末尾 fclose(fp) 后返 1。
    // 【残差豁免·同双参版】语义/调用序逐段对齐；残余为 gcc4.1↔4.4 CFG 块排布、
    // 回边极性与临时栈槽偏移差异（负例表「跨版本」族：空 else nop;jmp 岛 vs
    // jump-threading、outgoing 槽序、EH cleanup epilogue 合并），证据同
    // 双参重载注释（ORIG 0x0817bea4..0x0817c44d 全区间 objdump 转写）。
    std::FILE* fp = std::fopen(pathC_.c_str(), "rt");
    if (fp == 0)
        return 0;
    if (!Create())
        return 0;
    char* pStart = 0;
    char tag[0x400] = { 0 };
    std::sprintf(tag, "[%s]", path);
    unsigned int i = 0;
    unsigned int tokenLen = 0;
    int pos = 0;
    int tagLen = (int)std::strlen(tag);
    int c = 0;
    int rowIdx = 0;
    int colLen = 0;
    int scanIdx = 0;
    int probe;

    while (true)
    {
        probe = pathC_[scanIdx++];
        c = std::fgetc(fp);
        if (c == -1)
            return 0;
        if (c <= 0x20)
        {
        }
        else if (c > 0x7e)
        {
        }
        else if ((int)(signed char)tag[pos] == c)
        {
            ++pos;
            if (pos == tagLen)
                break;
        }
        else
            pos = 0;
    }

    char colBuf[0x200] = { 0 };
    colLen = 0;
    while (true)
    {
        c = std::fgetc(fp);
        if (c == -1)
            return 0;
        if (c == 0x20 || c == 9 || c == 10 || c == 13)
        {
            if (colLen > 0)
            {
                colBuf[colLen] = 0;
                colLen = 0;
                pStart = colBuf;
                tokenLen = (unsigned int)std::strlen(colBuf);
                for (i = 0; i < tokenLen; ++i)
                {
                    if (colBuf[i] == '/')
                    {
                        colBuf[i] = 0;
                        {
                            std::allocator<char> alloc;
                            std::string key(pStart, alloc);
                            colIndex_[key] = usedCols_;
                        }
                        pStart = colBuf + i + 1;
                    }
                }
                {
                    std::allocator<char> alloc;
                    std::string key(pStart, alloc);
                    colIndex_[key] = usedCols_;
                    usedCols_ = usedCols_ + 1;
                }
                if (usedCols_ + 1 >= cols_)
                {
                    if (!ExpandCol())
                        return 0;
                }
            }
        }
        else
        {
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (c == 10)
            break;
    }

    while (true)
    {
        rowIdx = -1;
        colLen = 0;
        while (true)
        {
            c = std::fgetc(fp);
            if (c == 0x20 || c == 9 || c == 10 || c == 13 || c == -1)
            {
                if (colLen > 0)
                {
                    colBuf[colLen] = 0;
                    colLen = 0;
                    if (rowIdx == -1)
                    {
                        {
                            std::allocator<char> alloc;
                            std::string key(colBuf, alloc);
                            rowIndex_[key] = usedRows_;
                        }
                    }
                    else
                    {
                        std::strcpy(cells_ + ((rowIdx + (usedRows_ * cols_)) << 7), colBuf);
                    }
                    ++rowIdx;
                    if (rowIdx == usedCols_)
                    {
                        while (c != -1 && c != 10)
                            c = std::fgetc(fp);
                    }
                }
                else
                {
                    c = -1;
                }
                if (c == 10 || c == -1)
                    break;
                continue;
            }
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (usedCols_ != rowIdx)
            return 0;
        ++usedRows_;
        if (usedRows_ + 1 >= rows_)
        {
            if (!ExpandRow())
                return 0;
        }
        if (c == -1)
            break;
    }
    std::fclose(fp);
    return 1;
}

// ============================================================================
// W6_ARAD.ScriptPods —— ORID df_game_r 的 ARAD::SCRIPT 域残余真实现清零批。
//
// 覆盖 ORIG 符号（nm -S 实证，全部弱/T）：
//   _ZN4ARAD6SCRIPT16SEventPeriodDataC1Eijj    W 0x088b4368（【objdump】三字段直存）
//   _ZN4ARAD6SCRIPT16SEventPeriodDataD1Ev      W 0x088b558a（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT16SEventPeriodDataeqERKS1_  W 0x088b4388（仅比 +0x00【objdump】）
//   _ZN4ARAD6SCRIPT17SRewardPeriodDataD1Ev     W 0x088b5764（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT17SRewardPeriodDataeqERKS1_ W 0x088b43da（仅比 +0x00【objdump】）
//   _ZN4ARAD6SCRIPT22_AradJumpingRewardItemD1Ev W 0x088b597e（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT21AradJumpingScriptDataC1Ev W 0x0837170a / D1Ev W 0x0837171e
//   _ZN4ARAD6SCRIPT18AradJumping_ScriptC1Ev    W 0x08371732 / D1Ev W 0x08371746
//   _ZN4ARAD6SCRIPT18AradJumping_Script17getRDARScriptDataEv W 0x088b43ee
//   _ZN4ARAD6SCRIPT18AradJumping_Script12importScriptEPcRS1_ T 0x088b4128
//   _ZN4ARAD6SCRIPT8readTimeEPs                T 0x088b3970（报告 ARAD__SCRIPT/readTime.md）
//   _ZN4ARAD6SCRIPT10uint32TimeEPs             T 0x088b3b4b（报告 ARAD__SCRIPT/uint32Time.md）
//   _ZN4ARAD6SCRIPT21importAradEventScriptE... T 0x088b3bd8（报告 importAradEventScript.md）
// getCurRewardItem(T 0x088b424a) 已由 AradPeriodB.cpp 批次提供，此处不重复定义。
//
// Ctor/Dtor 族说明：ORIG AradJumpingScriptData C1 仅调用 vector<_AradJumpingRewardItem>
// 默认构造；AradJumping_Script C1/D1 直接转发基类 —— 空体定义 + vector 成员即忠实对应。
// ============================================================================
#include <algorithm>
#include "CDataManager.h"               // ARAD::SCRIPT::AradJumping*（m_aradJumping 权威段）
#include <ctime>

#include "DNFLexWrapper.h"               // loadRDARScriptFile / ScanType / ScanInt(bool*)
#include "LogManager.h"                  // LogManager::logFormat(int, file, func, line, fmt, ...)
#include "Arad_EventPeriodDataManager.h" // SEventPeriodData / SRewardPeriodData / readTime 声明

namespace ARAD {
namespace SCRIPT {

namespace {
char const* const kRDAREtcParamFile = "../localjapan/Arad_RDARScriptEtcParameter.cpp";
}

// ---- SEventPeriodData -------------------------------------------------------
// 【objdump】C1：m_eventCode/m_beginTime/m_endTime 三字段直存（无其它副作用）。
#include "Arad_Script/SEventPeriodData.inc"
                                   unsigned int endTime)
{
    // 【二次核验 CCMC8】ORIG 0x088b4368 首存基址先载；4.4 对 int→unsigned 首成员
    // 恒值先物化——this->+(unsigned) 赋值形与成员初始化列表形两种 probe 实测均不改，
    // 维持直存（同 GetInt 族 N18a/cprop 归因）。
    m_eventCode = eventCode;
    m_beginTime = beginTime;
    m_endTime   = endTime;
}

bool SEventPeriodData::operator==(SEventPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

// ---- readTime ---------------------------------------------------------------
// 报告 ARAD__SCRIPT/readTime.md 全文转写：连读 4 个 short；任一 ScanInt(bool*)
// 失败抛出该字段名常量，catch 后 logFormat(line=0x27) 并返回 false。
#include "Arad_Script/readTime.inc"
{
    int idx = 0;
    bool ok = false;
    try
    {
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start year";
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start month";
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start day";
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start time";
        return true;
    }
    catch (char const* tag)
    {
        LogManager::logFormat(
            1, kRDAREtcParamFile, __FUNCTION__, 0x27,
            "[ARAD SCRIPT] Load fail. AradEventPeroid.etc %s", tag);
        return false;
    }
}

bool SRewardPeriodData::operator==(SRewardPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

// ---- uint32Time -------------------------------------------------------------
// 报告 ARAD__SCRIPT/uint32Time.md 全文转写：
//   time(NULL) → localtime → 改 y(-1900)/m(-1)/d/h（min/sec=0）→ mktime，
//   返回值沿用 mktime 返回（ORIG eax 直通）。年/月加载后 cwtl/movswl 符号扩展。
unsigned int uint32Time(short* t)
{
    time_t now = ::time(0);
    tm* lt = ::localtime(&now);
    lt->tm_year = (int)*t + -1900;       // lea -0x76c(%eax)
    lt->tm_mon  = (int)t[1] + -1;        // lea -0x1(%eax)
    lt->tm_mday = (int)t[2];             // movswl
    lt->tm_hour = (int)t[3];             // movswl
    lt->tm_min  = 0;
    lt->tm_sec  = 0;
    return (unsigned int)::mktime(lt);
}

}  // namespace SCRIPT

// ============================================================================
// AradJumping 族生命周期（见 CDataManager.h 中声明段注释的 ORIG 地址清单）。
// ============================================================================
namespace SCRIPT {

// [b28d-solo] _AradJumpingRewardItem 无用户 dtor（P317/P12）：ORIG D1/D2 W 0x088b597e
// 由 new_allocator<T>::destroy/construct 使用点发射为 W；显式空体定义会使类型
// non-trivial 并让 importScript 多发 D1 call + EH landing（ORIG 无）。

#include "Arad_Script/AradJumpingScriptData.inc"
{
    // 【objdump】W 0x0837170a：仅调 std::vector<_AradJumpingRewardItem>::C1Ev。
}

#include "Arad_Script/dtor_AradJumpingScriptData.inc"
{
    // 【objdump】W 0x0837171e：仅调 std::vector<_AradJumpingRewardItem>::D1Ev。
}

#include "Arad_Script/AradJumping_Script.inc"
{
    // 【objdump】W 0x08371732：直接转发 AradJumpingScriptData C1。
}

#include "Arad_Script/dtor_AradJumping_Script.inc"
{
    // 【objdump】W 0x08371746：直接转发 AradJumpingScriptData D1。
}

#include "Arad_Script/getRDARScriptData.inc"
{
    return &m_rewardItems;   // 【objdump】W 0x088b43ee：mov %eax,%eax … 返回 this+0。
}

// [b28d-solo-Arad_Script] importScript verdict=N 封顶（fresh .o 90/90 gap=0,
// p167 CLEAR 12/12, ndiff 4）。修复：①删 `_AradJumpingRewardItem` 用户空 dtor
// （P317：ORIG 局部 item 无 D1 call，D1/D2 仍由 allocator::destroy 使用点发射 W）；
// ②外层 while 改 goto 顶测消 loop rotation（P283 反向，ndiff 39→21）；③内层
// `while(a&&b&&c)` 物化岛改 for+`if(!ok)break;`（ndiff 21→4）。残余 4 条 =
// ORIG `mov items,%eax; lea &item,%edx`（this 先求值保活）vs 4.4 实参先装载
// （N7/P75；`(*items).`/语句表达式/static_cast receiver 三形实测均 4 条不变）。
#include "Arad_Script/importScript.inc"
{
    // 报告 ARAD__SCRIPT__AradJumping_Script/importScript.md 全文转写（T 0x088b4128）。
    // ORIG 返回约定：loadRDARScriptFile 失败 → 1；"[Jumping Gift]" 三元组
    // 解析完毕 → 0。条目循环内每个 ScanInt 失败直接回到 ScanType 主循环。
    std::string token;
    bool ok = false;
    if (!loadRDARScriptFile("", path))
        return true;

    // getRDARScriptData(target) 在解析循环前取一次（ORIG +0x41 存 -0xc(%ebp)）。
    std::vector<_AradJumpingRewardItem>* items = target.getRDARScriptData();

    // [b28d-solo-importScript] ORIG 外层循环为「条件置顶 + 体尾回跳」布局
    // （0x088b4174 条件块 → 0x088b418c `jne 0x088b4213` 退出 → 体 →
    // 0x088b420e `jmp 0x088b4174`），4.4 对 while 恒做 loop rotation
    // （条件置底 + 入口 jmp）⇒ goto 顶测形复现（P283 反向用法）。
    // 内层：ORIG 每条件 `movzbl ok; xor $1; test; je 继续; jmp 出口`（4.1 短路
    // 平铺）；4.4 对 `while(a&&b&&c)` 恒物化 0/1（+6 条）⇒ 改 for(;;)+`if(!ok)break;`
    // 消物化（ndiff 39→4）。残余 4 条 = ORIG `mov items,%eax; lea &item,%edx`（this
    // 先求值保活）vs 4.4 `lea &item,%eax; mov items`（实参先）——this-first 装载序
    // 不可翻（N7/P75 跨版本实参装载时刻；`(*items).`/语句表达式/static_cast receiver
    // 三形实测均 4 条不变）。gap=0、调用点集全等。
    // [L3Free25to36.L3AUserHistoryAradScript 2026-09-11 · 再补一形] receiver 改下标形
    // `items[0].push_back(item)` 复编后 ndiff 仍 =4、两侧助记符差均空 ⇒ 4.4 求值序
    // 不变（实参仍先于 receiver 装载）⇒ 回滚。
outer_test:
    if (!ScanType(token, true))
        goto outer_done;
    if (token == "[Jumping Gift]")
    {
        _AradJumpingRewardItem item;
        for (;;)
        {
            item.m_job = ScanInt(&ok);
            if (!ok)
                break;
            item.m_growValue = ScanInt(&ok);
            if (!ok)
                break;
            item.m_money = (unsigned long)(unsigned int)ScanInt(&ok);
            if (!ok)
                break;
            items->push_back(item);
        }
    }
    goto outer_test;
outer_done:
    return false;
}
// [b28d-solo-Arad_Script] importAradEventScript verdict=N 封顶（fresh .o 336/336
// gap=0，p167 CLEAR 57/57）。修复：①删 SEventPeriodData/SRewardPeriodData 用户空
// dtor（P317，消 4 处 EXTRA D1 call，identical +4/missing -4）；②外层 while 改
// goto 顶测消 loop rotation（P283 反向，gap -22→0）。残余（归一化槽号后 35 条）=
// 槽号差 8（N82/N93）、SRewardPeriodData 5 实参装载序/中转寄存器差（N7/P75）、
// 尾部 ebx 返回值物化位置（N104）。语义完成、byte 层 N 封顶。
#include "Arad_Script/importAradEventScript.inc"
                                   std::vector<SCRIPT::SRewardPeriodData>& rewardList)
{
    eventList.clear();    // ORIG +0x0e：vector<SEventPeriodData>::clear()
    rewardList.clear();   // ORIG +0x1b：vector<SRewardPeriodData>::clear()

    if (!loadRDARScriptFile("", "Etc/AradEventPeroid.etc"))
        return false;      // ORIG：loadRDARScriptFile 在 try 外，失败直接返回

    try
    {
        std::string token;
        bool ok = false;
        // [b28d-solo] ORIG 0x088b3bd8 外层循环同为「条件置顶 + 体尾回跳」
        // （条件块 `movl $1,0x4(%esp); lea token; call ScanType; xor $1; test;
        // jne 出口`），4.4 while 恒 loop rotation ⇒ goto 顶测形（P283 反向）。
event_scan:
        if (!ScanType(token, true))
            goto event_done;
        {
            if (token == "[event period]")
            {
                int eventCode = 0;
                short startShorts[4] = {0};   // ORIG 五个 movl $0 清零（-0x1c/-0x42/-0x3e/-0x4a/-0x46）
                short endShorts[4] = {0};
                for (;;)
                {
                    eventCode = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (!SCRIPT::readTime(startShorts))
                        throw "start time";
                    if (!SCRIPT::readTime(endShorts))
                        throw "end time";

                    SEventPeriodData data(eventCode,
                                          SCRIPT::uint32Time(startShorts),
                                          SCRIPT::uint32Time(endShorts));
                    if (std::find(eventList.begin(), eventList.end(), data)
                            != eventList.end())
                        throw "already exist";          // ORIG find != end 分支
                    eventList.push_back(data);
                }
            }
            else if (token == "[reward period]")
            {
                int eventCode = 0;
                short startShorts[4] = {0};   // ORIG 七个清零（-0x18/-0x64/-0x60/-0x6c/-0x68/-0x14/-0xe word）
                short endShorts[4] = {0};
                int itemIndex = 0;
                short itemCount = 0;
                for (;;)
                {
                    eventCode = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (!SCRIPT::readTime(startShorts))
                        throw "start time";
                    if (!SCRIPT::readTime(endShorts))
                        throw "end time";

                    itemIndex = ScanInt(&ok);
                    if (!ok)
                        throw "item index";
                    itemCount = (short)ScanInt(&ok);
                    if (!ok)
                        throw "item count";

                    SRewardPeriodData data(eventCode,
                                           (unsigned long)(unsigned int)itemIndex,
                                           itemCount,
                                           SCRIPT::uint32Time(startShorts),
                                           SCRIPT::uint32Time(endShorts));
                    if (std::find(rewardList.begin(), rewardList.end(), data)
                            != rewardList.end())
                        throw "already exist";
                    rewardList.push_back(data);
                }
            }
        }
        goto event_scan;
event_done:
        return true;    // ORIG 扫描完成 ebx=1。
    }
    catch (char const* msg)
    {
        LogManager::logFormat(
            1, "../localjapan/Arad_RDARScriptEtcParameter.cpp",
            __FUNCTION__, 0x99,
            "[ARAD SCRIPT] Load fail. AradEventPeroid.etc %s", msg);
        return false;
    }
}

}  // namespace SCRIPT
}  // namespace ARAD
