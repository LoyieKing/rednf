// ============================================================================
// CompiledDNFLex 成员函数（词法引擎编译态层；ORIG 地址与汇编证据见
// docs/class_func_reports/CompiledDNFLex/ 目录）。
// ----------------------------------------------------------------------------
// 本 TU 提供（_ZN14CompiledDNFLex*，全部真实 C++ 成员函数；vtable/typeinfo
// 由编译器生成 —— ORIG _ZTV14CompiledDNFLex 仅 D1/D0 两虚析构槽）：
//   getStringManager()   0x8ad0070   setStringManager(...) 0x8ad0050
//   GetInputStream()     0x8acfea0   CloseInputStream()    0x8ad0e90
//   PopInputStream(bool) 0x8ad0c80   PushInputStream(...)  0x8ad0a00
//   SwitchInputStream()  0x8ad0f40   SetHandler(...)       0x8acfdf0
//   EnableInclusion()    0x8ad0090   DisableInclusion()    0x8ad0080
//   IsStream()           0x8acfe80   GetOffset()           0x8acff20
//   getStreamName()      0x8ad0290   isEnd()               0x8ad0000
//   LexerError(...)      0x8acfe10   LoadStream(...)       0x8acfe40
//   getToken()           0x8ad0fa0   getInt/getFloat/getStr 0x8acff70/0x8acffc0/
//                                                     0x8ad00d0/0x8ad0190/0x8ad0250
//   create(bool)         0x8ad03d0   destroy()             0x8ad0370
//   CompiledDNFLex()     0x8ad07c0 / (ScriptStringManager*) 0x8ad0580
//   ~CompiledDNFLex()    0x8ad0ed0 D1 / 0x8ad0f20 D0
//
// 布局（0x204c）：+0x00 vptr、+0x04 offset_、+0x08 lastToken_、+0x0c
// stringManagerDelete_、+0x10 stringManager_、+0x14 stringBuffer_、
// +0x18 tokenBuffer_[0x2000]、+0x2018 std::deque<stream_t> streamStack_
// （libstdc++ _Deque_base 同为 0x28 字节：map/map_size/start/finish）、
// +0x2040 allow_inclusion_、+0x2044 lexerError_、+0x2048 loadStream_。
//
// stream_t（0x14）：dir/filename 为真实 std::string（COW），size/buffer/offset。
// deque 节点容量由 libstdc++ 决定（sizeof(stream_t)==0x14 → 每节点
// 0x1f4/0x14=25 元素，与 ORIG _M_initialize_map 一致）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <new>
#include <string>

#include "CompiledDNFLex.h"
#include "ScriptStringManager.h"

// toMbcs/toTString（UnicodeConvert.cpp 提供，恒等转换）。
const char* toMbcs(const char* src);
const char* toTString(const std::string& src);

namespace
{

const unsigned int STRING_BUFFER_SIZE = 0x400;

}  // namespace

// ---- getStringManager（ORIG 0x8ad0070）：返回 stringManager_。----
ScriptStringManager* CompiledDNFLex::getStringManager()
{
    return stringManager_;
}

// ---- setStringManager（ORIG 0x8ad0050）----
void CompiledDNFLex::setStringManager(ScriptStringManager* pStringManager,
                                      bool allowOwner)
{
    stringManagerDelete_ = allowOwner;
    stringManager_ = pStringManager;
}

// ---- GetInputStream（ORIG 0x8acfea0）：栈空返回 0，否则栈顶流。----
CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream()
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}

// ---- PopInputStream（ORIG 0x8ad0c80）----
// 栈空返回 false；!pop_all 且元素数 <2 返回 false（保留最底层流）。
// 否则弹出一个：释放 buffer、析构 dir/filename、pop_back；弹出后若栈仍非空，
// offset_ = 新栈顶 offset。返回 true。
bool CompiledDNFLex::PopInputStream(bool pop_all)
{
    if (streamStack_.empty())
        return false;
    if (!pop_all && streamStack_.size() < 2)
        return false;

    stream_t& top = streamStack_.back();
    if (top.buffer != 0)
    {
        ::operator delete(top.buffer);
        top.buffer = 0;
    }
    // stream_t 析构（两个 std::string 引用计数递减）由 pop_back 完成。
    streamStack_.pop_back();

    if (!streamStack_.empty())
    {
        offset_ = streamStack_.back().offset;
    }
    return true;
}

// ---- CloseInputStream（ORIG 0x8ad0e90）：反复弹出至空。----
void CompiledDNFLex::CloseInputStream()
{
    while (!streamStack_.empty())
    {
        PopInputStream(true);
    }
}

// ---- PushInputStream（ORIG 0x8ad0a00）----
// buffer 魔数校验（首 short == 0xd0b0；不匹配释放 buffer 并失败）；
// 非空栈先把旧栈顶 offset 保存；push 新流并置 offset_（魔数匹配时跳过
// 2 字节头）。
bool CompiledDNFLex::PushInputStream(const char* dir, const char* filename,
                                     char* buffer, unsigned int bufferSize)
{
    bool emptyFile = false;   // buffer==NULL || bufferSize==0 → 无魔数校验
    if (buffer != 0 && bufferSize != 0)
    {
        if (bufferSize < 2)
            return false;
        if (*reinterpret_cast<short*>(buffer) !=
            static_cast<short>(0xd0b0))
        {
            ::operator delete(buffer);
            return false;
        }
        emptyFile = false;
    }
    else
    {
        emptyFile = true;
    }

    std::string d(dir ? dir : "");
    std::string f(filename ? filename : "");

    if (!streamStack_.empty())
        streamStack_.back().offset = offset_;

    streamStack_.push_back(stream_t(d, f));
    stream_t& s = streamStack_.back();
    s.size = bufferSize;
    s.buffer = buffer;
    s.offset = buffer;

    offset_ = buffer;
    if (!emptyFile)
        offset_ = buffer + 2;
    return true;
}

// ---- SwitchInputStream（ORIG 0x8ad0f40）：Close 后尾调 Push。----
bool CompiledDNFLex::SwitchInputStream(const char* dir, const char* filename,
                                       char* buffer, unsigned int bufferSize)
{
    CloseInputStream();
    return PushInputStream(dir, filename, buffer, bufferSize);
}

// ---- SetHandler（ORIG 0x8acfdf0）----
void CompiledDNFLex::SetHandler(TLexerError lexerErrorFunc,
                                TLoadStream loadStreamFunc)
{
    lexerError_ = lexerErrorFunc;
    loadStream_ = loadStreamFunc;
}

// ---- EnableInclusion / DisableInclusion（ORIG 0x8ad0090/0x8ad0080）----
void CompiledDNFLex::EnableInclusion()
{
    flag2040_ = 1;
}

void CompiledDNFLex::DisableInclusion()
{
    flag2040_ = 0;
}

// ---- IsStream（ORIG 0x8acfe80）：栈非空。----
bool CompiledDNFLex::IsStream()
{
    return !streamStack_.empty();
}

// ---- GetOffset（ORIG 0x8acff20）：当前流内偏移；无流/无缓冲返回 -1。----
int CompiledDNFLex::GetOffset()
{
    if (streamStack_.empty())
        return -1;
    const stream_t& s = streamStack_.back();
    if (s.buffer == 0)
        return -1;
    return static_cast<int>(offset_ - s.buffer);
}

// ---- getStreamName（ORIG 0x8ad0290）：dir+filename；空栈 "NONE"。----
std::string CompiledDNFLex::getStreamName()
{
    if (streamStack_.empty())
        return std::string("NONE");
    const stream_t& s = streamStack_.back();
    if (&s.filename == 0)  // 防御（ORIG 空指针分支）
        return std::string("NONE");
    std::string result = s.filename;
    result.append(s.dir);
    return result;
}

// ---- isEnd（ORIG 0x8ad0000）：读位置到达当前流末尾。----
bool CompiledDNFLex::isEnd()
{
    if (streamStack_.empty())
        return false;
    const stream_t& s = streamStack_.back();
    if (s.buffer == 0)
        return false;
    return s.buffer + s.size <= offset_;
}

// ---- LexerError（ORIG 0x8acfe10）----
void CompiledDNFLex::LexerError(const char* msg)
{
    if (lexerError_ != 0)
        lexerError_(msg);
}

// ---- LoadStream（ORIG 0x8acfe40）：转发注册回调。----
bool CompiledDNFLex::LoadStream(const char* dir, const char* filename,
                                DNFLex::stream_data_t* s)
{
    if (loadStream_ == 0)
        return false;
    return loadStream_(dir, filename, s);
}

// ---- getStr(std::string&)（ORIG 0x8ad00d0）：按字符串表索引取串。----
bool CompiledDNFLex::getStr(std::string& stringBuffer)
{
    int tok = lastToken_;
    if (!(tok > DNF_TOK_HEXADECIMAL && tok != DNF_TOK_PAIR_INT &&
          tok != DNF_TOK_FLOAT && tok != DNF_TOK_EOF && tok != DNF_TOK_ERROR))
        return false;

    int index = *reinterpret_cast<int*>(offset_);
    offset_ += 4;
    return stringManager_->getString(index, &stringBuffer);
}

// ---- getStr(char*, int)（ORIG 0x8ad0190）----
bool CompiledDNFLex::getStr(char* buffer, int maxSize)
{
    int tok = lastToken_;
    if (!(tok > DNF_TOK_HEXADECIMAL && tok != DNF_TOK_PAIR_INT &&
          tok != DNF_TOK_FLOAT && tok != DNF_TOK_EOF && tok != DNF_TOK_ERROR))
        return false;

    int index = *reinterpret_cast<int*>(offset_);
    offset_ += 4;
    const std::string* r_string = stringManager_->getString(index);
    const char* src = r_string->c_str();
    std::size_t string_length = r_string->size();
    if (static_cast<int>(string_length) <= maxSize)
    {
        std::memcpy(buffer, src, string_length);
        buffer[string_length] = '\0';
        return true;
    }
    std::memcpy(buffer, src, maxSize - 1);
    buffer[maxSize - 1] = '\0';
    return false;
}

// ---- getStr(bool*)（ORIG 0x8ad0250）：经 stringBuffer_ 缓冲。----
char* CompiledDNFLex::getStr(bool* result)
{
    bool rslt = getStr(stringBuffer_, STRING_BUFFER_SIZE);
    if (result != 0)
        *result = rslt;
    return stringBuffer_;
}

// ---- getInt（ORIG 0x8acff70）：DECIMAL/HEXADECIMAL/PAIR_INT 有效。----
int CompiledDNFLex::getInt(bool* result)
{
    int tok = lastToken_;
    if (tok > DNF_TOK_HEXADECIMAL && tok != DNF_TOK_PAIR_INT)
    {
        if (result != 0)
            *result = false;
        return 0;
    }
    if (result != 0)
        *result = true;
    int value = *reinterpret_cast<int*>(offset_);
    offset_ += 4;
    return value;
}

// ---- getFloat（ORIG 0x8acffc0）：仅 FLOAT 有效。----
float CompiledDNFLex::getFloat(bool* result)
{
    if (lastToken_ != DNF_TOK_FLOAT)
    {
        if (result != 0)
            *result = false;
        return 0.0f;
    }
    if (result != 0)
        *result = true;
    float value = *reinterpret_cast<float*>(offset_);
    offset_ += 4;
    return value;
}

// ---- create（ORIG 0x8ad03d0）----
void CompiledDNFLex::create(bool createStringTable)
{
    if (createStringTable)
    {
        if (stringManager_ == 0)
            stringManager_ = new ScriptStringManager();
        stringManagerDelete_ = true;
    }
    if (stringBuffer_ == 0)
        stringBuffer_ = new char[STRING_BUFFER_SIZE];
}

// ---- destroy（ORIG 0x8ad0370）----
void CompiledDNFLex::destroy()
{
    if (!stringManagerDelete_)
    {
        stringManager_ = 0;
    }
    else
    {
        if (stringManager_ != 0)
        {
            delete stringManager_;   // 经 D0 deleting 析构（vtable 槽 1）
            stringManager_ = 0;
        }
        stringManagerDelete_ = false;
    }
    if (stringBuffer_ != 0)
    {
        ::operator delete[](stringBuffer_);
        stringBuffer_ = 0;
    }
}

// ---- getToken（ORIG 0x8ad0fa0）----
// 主循环：EOF 处理（PopInputStream(false)，失败保持 EOF）；INCLUDE(11) 时
// 读下一个 STRING 记号作文件名，LoadStream+PushInputStream 切入包含文件。
int CompiledDNFLex::getToken()
{
    for (;;)
    {
        if (!streamStack_.empty())
        {
            stream_t& cur = streamStack_.back();
            if (offset_ >= cur.buffer + cur.size)
            {
                // 当前流读完：EOF → 弹出后继续（弹不出则保持 EOF）。
                lastToken_ = DNF_TOK_EOF;
                if (!PopInputStream(false))
                    return lastToken_;
                continue;
            }
        }

        int c = static_cast<unsigned char>(*offset_);
        ++offset_;
        lastToken_ = c;

        if (flag2040_ == 0 || c != DNF_TOK_INCLUDE)
            return lastToken_;

        // INCLUDE：下一字节必须是 STRING 记号，随后是字符串表索引。
        stream_t* incl = streamStack_.empty() ? 0 : &streamStack_.back();
        int next = static_cast<unsigned char>(*offset_);
        ++offset_;
        lastToken_ = next;
        if (next != DNF_TOK_STRING || incl == 0)
        {
            std::snprintf(reinterpret_cast<char*>(tokenBuffer_),
                          sizeof(tokenBuffer_),
                          "%s - %s/%s, invalid inclusion.", "getToken",
                          toMbcs(incl->dir.c_str()),
                          toMbcs(incl->filename.c_str()));
            LexerError(reinterpret_cast<char*>(tokenBuffer_));
            return DNF_TOK_ERROR;
        }

        std::string include_path;
        if (!getStr(include_path))
        {
            std::snprintf(reinterpret_cast<char*>(tokenBuffer_),
                          sizeof(tokenBuffer_),
                          "%s - %s%s, invalid inclusion.", "getToken",
                          toMbcs(incl->dir.c_str()),
                          toMbcs(incl->filename.c_str()));
            LexerError(reinterpret_cast<char*>(tokenBuffer_));
            return DNF_TOK_ERROR;
        }

        DNFLex::stream_data_t sd;
        sd.clear();
        if (!LoadStream("", toTString(include_path), &sd))
        {
            std::snprintf(reinterpret_cast<char*>(tokenBuffer_), sizeof(tokenBuffer_),
                          "%s - %s%s, LoadStream failed.", "getToken",
                          toMbcs(incl->dir.c_str()),
                          toMbcs(incl->filename.c_str()));
            LexerError(reinterpret_cast<char*>(tokenBuffer_));
            return DNF_TOK_ERROR;
        }

        char* sd_buf = sd.buffer;
        if (sd_buf == 0)
        {
            sd_buf = new char[sd.size];
            reinterpret_cast<std::istream*>(sd.stream)->read(sd_buf, sd.size);
        }

        if (!PushInputStream("", toTString(include_path), sd_buf, sd.size))
        {
            std::snprintf(reinterpret_cast<char*>(tokenBuffer_), sizeof(tokenBuffer_),
                          "%s - %s%s, PushInputStream failed.", "getToken",
                          toMbcs(incl->dir.c_str()),
                          toMbcs(incl->filename.c_str()));
            LexerError(reinterpret_cast<char*>(tokenBuffer_));
            return DNF_TOK_ERROR;
        }

        if (sd.stream != 0)
        {
            // 关闭 istream（ORIG 经其 vtable 槽 +4 析构子对象）。
            void (**vtable)(void*) =
                *reinterpret_cast<void (***)(void*)>(sd.stream);
            vtable[1](sd.stream);
        }
    }
}

// ---- CompiledDNFLex()（ORIG 0x8ad07c0，默认构造：create(true) 自建串表）----
CompiledDNFLex::CompiledDNFLex()
    : offset_(0),
      lastToken_(DNF_TOK_EOF),
      stringManagerDelete_(false),
      stringManager_(0),
      stringBuffer_(0),
      tokenBuffer_(),
      streamStack_(),
      flag2040_(1),
      pad2041_(),
      lexerError_(0),
      loadStream_(0)
{
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    create(true);
}

// ---- CompiledDNFLex(ScriptStringManager*)（ORIG 0x8ad0580：create(false)）----
CompiledDNFLex::CompiledDNFLex(ScriptStringManager* pStringManager)
    : offset_(0),
      lastToken_(DNF_TOK_EOF),
      stringManagerDelete_(false),
      stringManager_(pStringManager),
      stringBuffer_(0),
      tokenBuffer_(),
      streamStack_(),
      flag2040_(1),
      pad2041_(),
      lexerError_(0),
      loadStream_(0)
{
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    create(false);
}

// ---- ~CompiledDNFLex（ORIG 0x8ad0ed0 D1：Close → destroy → ~deque）----
CompiledDNFLex::~CompiledDNFLex()
{
    CloseInputStream();
    destroy();
}
