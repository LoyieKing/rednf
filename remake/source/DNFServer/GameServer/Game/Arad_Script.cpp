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
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    local_c = cells_ + a;
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
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    local_c = cells_ + a;
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
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    local_c = cells_ + a;
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
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    return cells_ + a;
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
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    std::sprintf(cells_ + a, "%d", value);
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
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    std::strcpy(cells_ + a, value);
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

static bool parse_arad_from_getc(Arad_Script* self, int (*getc_fn)(void*), void* src,
                                 const char* section_name)
{
    char section[0x400];
    char token[0x200];
    int i;
    int sect_len;
    int match_pos;
    int ch;
    int tok_len;
    int col;

    if (!self->Create())
        return 0;

    std::memset(section, 0, sizeof(section));
    std::sprintf(section, "[%s]", section_name);
    match_pos = 0;
    sect_len = (int)std::strlen(section);

read_sect:
    ch = getc_fn(src);
    if (ch == -1)
        return 0;
    if (ch <= 0x20)
        goto read_sect;
    if (ch > 0x7e)
        goto read_sect;
    if ((int)(signed char)section[match_pos] == ch)
    {
        match_pos = match_pos + 1;
        if (match_pos == sect_len)
            goto found_sect;
        goto read_sect;
    }
    match_pos = 0;
    goto read_sect;

found_sect:
    std::memset(token, 0, 0x200);
    tok_len = 0;

read_hdr:
    ch = getc_fn(src);
    if (ch == -1)
        return 0;
    if (ch == 0x20 || ch == 9 || ch == 0xa || ch == 0xd)
    {
        if (tok_len <= 0)
            goto after_hdr_tok;
        token[tok_len] = 0;
        tok_len = 0;
        {
            char* cur = token;
            for (i = 0; token[i] != 0; ++i)
            {
                if (token[i] == '/')
                {
                    token[i] = 0;
                    self->colIndex_[std::string(cur)] = self->usedCols_;
                    cur = token + i + 1;
                    self->usedCols_ = self->usedCols_ + 1;
                    if (self->usedCols_ + 1 >= self->cols_)
                    {
                        if (!self->ExpandCol())
                            return 0;
                    }
                }
            }
            self->colIndex_[std::string(cur)] = self->usedCols_;
            self->usedCols_ = self->usedCols_ + 1;
            if (self->usedCols_ + 1 >= self->cols_)
            {
                if (!self->ExpandCol())
                    return 0;
            }
        }
    after_hdr_tok:
        if (ch == 0xa)
            goto rows;
        goto read_hdr;
    }
    token[tok_len] = (char)ch;
    tok_len = tok_len + 1;
    goto read_hdr;

rows:
    col = -1;
    tok_len = 0;
read_row:
    ch = getc_fn(src);
    if (ch == 0x20 || ch == 9 || ch == 0xa || ch == 0xd)
        goto end_tok;
    if (ch == -1)
        goto end_tok;
    token[tok_len] = (char)ch;
    tok_len = tok_len + 1;
    goto read_row;

end_tok:
    if (tok_len <= 0)
    {
        ch = -1;
        goto row_done;
    }
    token[tok_len] = 0;
    tok_len = 0;
    if (col == -1)
    {
        self->rowIndex_[std::string(token)] = self->usedRows_;
        goto next_col;
    }
    std::strcpy(self->cells_ + (self->cols_ * self->usedRows_ + col) * 0x80, token);
next_col:
    col = col + 1;
    if (col == self->usedCols_)
    {
        while (ch != -1 && ch != 0xa)
            ch = getc_fn(src);
    }
    if (ch == 0xa || ch == -1)
    {
        if (col != self->usedCols_)
            return 0;
        self->usedRows_ = self->usedRows_ + 1;
        if (self->usedRows_ + 1 >= self->rows_)
        {
            if (!self->ExpandRow())
                return 0;
        }
    }
    else
        goto read_row;

row_done:
    if (ch == 0xa || ch == -1)
        ;
    else
        goto read_row;
    if (ch == -1)
        return 1;
    goto rows;
}

static int getc_from_ptr(void* p)
{
    const char** s = (const char**)p;
    unsigned char c = (unsigned char)**s;
    *s = *s + 1;
    return (int)(signed char)c;
}

static int getc_from_file(void* p)
{
    return std::fgetc((std::FILE*)p);
}

bool Arad_Script::Load(const char* dir, const char* file)
{
    if (!Create())
        return 0;
    // ORIG 0x817b950：先在数据流中定位 sprintf("[%s]", file) 节标记，跳过后
    // 才开始解析（滑动匹配：空白/控制字符忽略，不匹配则 pos 归零继续读）。
    char tag[0x100] = {0};
    sprintf(tag, "[%s]", file);
    int tagLen = (int)strlen(tag);

    const char* p = dir;
    int pos = 0;
    while (true)
    {
        unsigned char c = (unsigned char)*p;
        int ch = (int)(signed char)c;
        if (ch == -1)
            return 0;
        if (ch <= 0x20 || ch > 0x7e)
        {
            ++p;
            continue;
        }
        if (tag[pos] == ch)
        {
            ++pos;
            if (pos == tagLen)
            {
                ++p;
                break;
            }
        }
        else
        {
            pos = 0;
        }
        ++p;
    }
    return parse_arad_from_getc(this, getc_from_ptr, &p, file);
}

bool Arad_Script::Load(const char* path)
{
    std::FILE* fp = std::fopen(pathC_.c_str(), "rt");
    if (fp == 0)
        return 0;
    if (!Create())
        return 0;
    // ORIG 0x817bea4：在文件流中定位 sprintf("[%s]", path) 节标记后解析。
    char tag[0x100] = {0};
    sprintf(tag, "[%s]", path);
    int tagLen = (int)strlen(tag);
    int pos = 0;
    while (true)
    {
        int ch = std::fgetc(fp);
        if (ch == -1)
        {
            std::fclose(fp);
            return 0;
        }
        if (ch <= 0x20 || ch > 0x7e)
            continue;
        if (tag[pos] == ch)
        {
            ++pos;
            if (pos == tagLen)
                break;
        }
        else
        {
            pos = 0;
        }
    }
    bool ok = parse_arad_from_getc(this, getc_from_file, fp, path);
    std::fclose(fp);
    return ok;
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
