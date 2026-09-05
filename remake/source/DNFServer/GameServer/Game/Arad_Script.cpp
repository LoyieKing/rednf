#include "Arad_Script.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

// [moved] include 区统一收敛到文件头（中部 include 在 lsd44 链下会污染后续标准头）

Arad_Script::Arad_Script()
{
}

Arad_Script::Arad_Script(const char* path)
{
    const char* full = make_file_path(path);
    pathC_ = full;
}

int Arad_Script::GetRowNum()
{
    return usedRows_;
}

Arad_Script::~Arad_Script()
{
    if (cells_ != 0)
    {
        if (cells_ != 0)
            delete[] cells_;
    }
}

bool Arad_Script::Create()
{
    if (cells_ == 0 && cells_ != 0)
        delete[] cells_;
    cols_ = 0x20;
    rows_ = 0x100;
    bufBytes_ = cols_ * rows_ * 0x80;
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

bool Arad_Script::ExpandCol()
{
    cols_ = cols_ + 0x20;
    bufBytes_ = cols_ * rows_ * 0x80;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    std::memset(n, 0, (size_t)bufBytes_);
    if (cells_ != 0)
        delete[] cells_;
    cells_ = n;
    return 1;
}

bool Arad_Script::ExpandRow()
{
    rows_ = rows_ + rows_;
    bufBytes_ = cols_ * rows_ * 0x80;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    int nsz = bufBytes_;
    std::memset(n + nsz, 0, (size_t)(bufBytes_ - nsz));
    std::memcpy(n, cells_, (size_t)nsz);
    if (cells_ != 0)
        delete[] cells_;
    cells_ = n;
    return 1;
}

const char* Arad_Script::make_file_path(const char* file)
{
    static char full_path[0x200];
    std::strcpy(full_path, file);
    return full_path;
}

int Arad_Script::GetIdx4Col(char* name)
{
    std::map<std::string, int>::iterator it = colIndex_.find(std::string(name));
    if (it != colIndex_.end())
        return it->second;
    return -1;
}

int Arad_Script::GetIdx4Row(char* name)
{
    std::map<std::string, int>::iterator it = rowIndex_.find(std::string(name));
    if (it != rowIndex_.end())
        return it->second;
    return -1;
}

int Arad_Script::GetInt(int row, int col)
{
    char* local_c;
    local_c = cells_ + ((cols_ * row + col) << 7);
    return std::atoi(local_c);
}

int Arad_Script::GetInt(char* row, char* col)
{
    return GetInt(GetIdx4Row(row), GetIdx4Col(col));
}

int Arad_Script::GetInt(char* row, int col)
{
    return GetInt(GetIdx4Row(row), col);
}

int Arad_Script::GetInt(int row, char* col)
{
    return GetInt(row, GetIdx4Col(col));
}

float Arad_Script::GetFloat(int row, int col)
{
    char* local_c;
    local_c = cells_ + ((cols_ * row + col) << 7);
    return (float)std::atof(local_c);
}

float Arad_Script::GetFloat(char* row, char* col)
{
    return GetFloat(GetIdx4Row(row), GetIdx4Col(col));
}

float Arad_Script::GetFloat(char* row, int col)
{
    return GetFloat(GetIdx4Row(row), col);
}

float Arad_Script::GetFloat(int row, char* col)
{
    return GetFloat(row, GetIdx4Col(col));
}

double Arad_Script::GetDouble(int row, int col)
{
    char* local_c;
    local_c = cells_ + ((cols_ * row + col) << 7);
    return std::atof(local_c);
}

double Arad_Script::GetDouble(char* row, char* col)
{
    return GetDouble(GetIdx4Row(row), GetIdx4Col(col));
}

double Arad_Script::GetDouble(char* row, int col)
{
    return GetDouble(GetIdx4Row(row), col);
}

double Arad_Script::GetDouble(int row, char* col)
{
    return GetDouble(row, GetIdx4Col(col));
}

char* Arad_Script::GetString(int row, int col)
{
    char* local_c;
    local_c = cells_ + ((cols_ * row + col) << 7);
    return local_c;
}

char* Arad_Script::GetString(char* row, char* col)
{
    return GetString(GetIdx4Row(row), GetIdx4Col(col));
}

char* Arad_Script::GetString(char* row, int col)
{
    return GetString(GetIdx4Row(row), col);
}

char* Arad_Script::GetString(int row, char* col)
{
    return GetString(row, GetIdx4Col(col));
}

void Arad_Script::SetInt(int row, int col, int value)
{
    std::sprintf(cells_ + ((cols_ * row + col) << 7), "%d", value);
}

void Arad_Script::SetInt(char* row, char* col, int value)
{
    SetInt(GetIdx4Row(row), GetIdx4Col(col), value);
}

void Arad_Script::SetInt(char* row, int col, int value)
{
    SetInt(GetIdx4Row(row), col, value);
}

void Arad_Script::SetInt(int row, char* col, int value)
{
    SetInt(row, GetIdx4Col(col), value);
}

void Arad_Script::SetString(int row, int col, char* value)
{
    std::strcpy(cells_ + ((cols_ * row + col) << 7), value);
}

void Arad_Script::SetString(char* row, char* col, char* value)
{
    SetString(GetIdx4Row(row), GetIdx4Col(col), value);
}

void Arad_Script::SetString(char* row, int col, char* value)
{
    SetString(GetIdx4Row(row), col, value);
}

void Arad_Script::SetString(int row, char* col, char* value)
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
bool Arad_Script::Load(const char* dir, const char* file)
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

bool Arad_Script::Load(const char* path)
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
SEventPeriodData::SEventPeriodData(int eventCode, unsigned int beginTime,
                                   unsigned int endTime)
{
    m_eventCode = eventCode;
    m_beginTime = beginTime;
    m_endTime   = endTime;
}

// 【objdump】D1：空体。
SEventPeriodData::~SEventPeriodData()
{
}


bool SEventPeriodData::operator==(SEventPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

// ---- readTime ---------------------------------------------------------------
// 报告 ARAD__SCRIPT/readTime.md 全文转写：连读 4 个 short；任一 ScanInt(bool*)
// 失败抛出该字段名常量，catch 后 logFormat(line=0x27) 并返回 false。
bool readTime(short* out)
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

// 【objdump】D1：空体。
SRewardPeriodData::~SRewardPeriodData()
{
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

_AradJumpingRewardItem::~_AradJumpingRewardItem()
{
}   // 【objdump】W 0x088b597e：5 字节空体。

AradJumpingScriptData::AradJumpingScriptData()
{
    // 【objdump】W 0x0837170a：仅调 std::vector<_AradJumpingRewardItem>::C1Ev。
}

AradJumpingScriptData::~AradJumpingScriptData()
{
    // 【objdump】W 0x0837171e：仅调 std::vector<_AradJumpingRewardItem>::D1Ev。
}

AradJumping_Script::AradJumping_Script()
{
    // 【objdump】W 0x08371732：直接转发 AradJumpingScriptData C1。
}

AradJumping_Script::~AradJumping_Script()
{
    // 【objdump】W 0x08371746：直接转发 AradJumpingScriptData D1。
}

std::vector<_AradJumpingRewardItem>* AradJumping_Script::getRDARScriptData()
{
    return &m_rewardItems;   // 【objdump】W 0x088b43ee：mov %eax,%eax … 返回 this+0。
}

bool AradJumping_Script::importScript(char* path, AradJumping_Script& target)
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

    while (ScanType(token, true))
    {
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
    }
    return false;
}
bool importAradEventScript(std::vector<SCRIPT::SEventPeriodData>& eventList,
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
        while (ScanType(token, true))
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
