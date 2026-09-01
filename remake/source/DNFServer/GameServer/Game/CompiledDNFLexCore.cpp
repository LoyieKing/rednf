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
const char* toMbcs(const std::string& src);
const char* toTString(const std::string& src);

namespace
{

const unsigned int STRING_BUFFER_SIZE = 0x400;
const unsigned int STREAM_NODE_ELEMS = 0x1f4 / sizeof(CompiledDNFLex::stream_t);

}  // namespace

// ORIG `_ZZ17readIntFromBufferRPcE9intBuffer` / `_ZZ18readCharFromBufferRPcE10charBuffer`
// / `_ZZ19readFloatFromBufferRPcE11floatBuffer`：外部链接、函数内 static，-O2 内联进
// getInt/getStr/getToken/getFloat。
static int readIntFromBuffer(char*& offset)
{
    static int intBuffer;
    intBuffer = *reinterpret_cast<int*>(offset);
    offset += 4;
    return intBuffer;
}

int readCharFromBuffer(char*& offset)
{
    static char charBuffer;
    signed char c = *reinterpret_cast<signed char*>(offset);
    ++offset;
    charBuffer = c;
    return c;
}

float readFloatFromBuffer(char*& offset)
{
    static float floatBuffer;
    floatBuffer = *reinterpret_cast<float*>(offset);
    offset += 4;
    return floatBuffer;
}

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

const CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream() const
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
    if (!pop_all)
        goto check_size;
do_pop:
    {
        stream_t& top = streamStack_.back();
        if (top.buffer != 0)
            delete[] top.buffer;
        streamStack_.pop_back();
        if (!streamStack_.empty())
            offset_ = streamStack_.back().offset;
        return true;
    }
check_size:
    if (streamStack_.size() > 1)
        goto do_pop;
    return false;
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
    // ORIG 栈上 stream_t：只默认构造两棵 string，size/buffer/offset 不先清零。
    stream_t stream;
    if (dir == 0)
        stream.dir.assign("", static_cast<size_t>(0));
    else
        stream.dir.assign(dir, std::strlen(dir));
    if (filename == 0)
        stream.filename.assign("", static_cast<size_t>(0));
    else
        stream.filename.assign(filename, std::strlen(filename));

    bool empty_file = false;
    if (buffer == 0 || (empty_file = true, bufferSize != 0))
    {
        if (bufferSize < 2)
            return false;
        if (*reinterpret_cast<short*>(buffer) != static_cast<short>(0xd0b0))
        {
            delete[] buffer;
            return false;
        }
        empty_file = false;
    }

    stream.size = bufferSize;
    stream.buffer = buffer;

    if (!streamStack_.empty())
        streamStack_.back().offset = offset_;

    streamStack_.push_back(stream);

    offset_ = buffer;
    if (!empty_file)
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
bool CompiledDNFLex::IsStream() const
{
    return !streamStack_.empty();
}

// ---- GetOffset（ORIG 0x8acff20）：--finish 后取 buffer；空/指针 0/buffer 0 → -1。----
int CompiledDNFLex::GetOffset() const
{
    stream_t* p = streamStack_.end()._M_cur;
    if (p == streamStack_.begin()._M_cur)
        goto fail;
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (p != 0)
            goto have;
    }
fail:
    return -1;
have:
    if (p->buffer == 0)
        goto fail;
    return (int)(offset_ - p->buffer);
}

// ---- getStreamName（ORIG 0x8ad0290）：NRVO 槽上 copy dir 再 append(filename)；空/空指针 "NONE"。----
std::string CompiledDNFLex::getStreamName() const
{
    stream_t* p = streamStack_.end()._M_cur;
    if (p == streamStack_.begin()._M_cur)
        return std::string("NONE");
    stream_t** node = streamStack_.end()._M_node;
    if (p == streamStack_.end()._M_first)
        p = *(node - 1) + STREAM_NODE_ELEMS;
    p = p - 1;
    if (p == 0)
        return std::string("NONE");
    std::string r(p->dir);
    r.append(p->filename);
    return r;
}

// ---- isEnd（ORIG 0x8ad0000）：--finish 后 buffer+size <= offset_；空/空指针返回 0。----
bool CompiledDNFLex::isEnd()
{
    stream_t* p = streamStack_.end()._M_cur;
    if (p == streamStack_.begin()._M_cur)
        return false;
    stream_t** node = streamStack_.end()._M_node;
    if (p == streamStack_.end()._M_first)
        p = *(node - 1) + STREAM_NODE_ELEMS;
    p = p - 1;
    if (p == 0)
        return false;
    return p->buffer + p->size <= offset_;
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
    unsigned d = (unsigned)tok - 2;
    if (d <= 1)
        goto fail;
    if (tok != DNF_TOK_PAIR_INT)
        goto more;
fail:
    return false;
more:
    if (tok == DNF_TOK_FLOAT)
        goto fail;
    if (tok == DNF_TOK_EOF)
        goto fail;
    if (tok == DNF_TOK_ERROR)
        goto fail;
    int index = readIntFromBuffer(offset_);
    return stringManager_->getString(index, &stringBuffer);
}

// ---- getStr(char*, int)（ORIG 0x8ad0190）----
bool CompiledDNFLex::getStr(char* buffer, int maxSize)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d <= 1)
        goto fail;
    if (tok != DNF_TOK_PAIR_INT)
        goto more;
fail:
    return false;
more:
    if (tok == DNF_TOK_FLOAT)
        goto fail;
    if (tok == DNF_TOK_EOF)
        goto fail;
    if (tok == DNF_TOK_ERROR)
        goto fail;
    int index = readIntFromBuffer(offset_);
    const std::string* r_string = stringManager_->getString(index);
    const char* src = r_string->c_str();
    int string_length = (int)r_string->size();
    if (__builtin_expect(string_length > maxSize, 0))
        goto overflow;
    std::memcpy(buffer, src, (unsigned)string_length);
    buffer[string_length] = 0;
    return true;
overflow:
    std::memcpy(buffer, src, (unsigned)(maxSize - 1));
    buffer[string_length - 1] = 0;
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
    unsigned d = (unsigned)tok - 2;
    if (d > 1 && tok != DNF_TOK_PAIR_INT)
    {
        if (result != 0)
            *result = false;
        return 0;
    }
    if (result != 0)
        *result = true;
    static int intBuffer;
    intBuffer = *reinterpret_cast<int*>(offset_);
    offset_ += 4;
    return intBuffer;
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
    static float floatBuffer;
    floatBuffer = *reinterpret_cast<float*>(offset_);
    offset_ += 4;
    return floatBuffer;
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
    if (stringManagerDelete_)
    {
        ScriptStringManager* p = stringManager_;
        if (p != 0)
        {
            void (**vt)(void*) = *(void (***)(void*))p;
            vt[1](p);
            stringManager_ = 0;
        }
        stringManagerDelete_ = false;
    }
    else
        stringManager_ = 0;
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
    // ORIG uses the std::deque internals directly; at -O0 end()._M_xxx
    // re-materializes an iterator each time (4 end + 1 begin = 5 calls).
    // Reuse two iterators.
    std::deque<stream_t>::iterator endIt = streamStack_.end();
    std::deque<stream_t>::iterator beginIt = streamStack_.begin();
    stream_t* p = endIt._M_cur;
    if (p != beginIt._M_cur)
    {
        if (p == endIt._M_first)
            p = *(endIt._M_node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (p != 0)
        {
            if (offset_ >= p->buffer + p->size)
            {
                lastToken_ = DNF_TOK_EOF;
                if (!PopInputStream(false))
                    return lastToken_;
                goto recurse;
            }
        }
    }

    {
    int c = readCharFromBuffer(offset_);
    lastToken_ = c;
    if (flag2040_ == 0)
        return lastToken_;
    if (c != DNF_TOK_INCLUDE)
        return lastToken_;

    // ORIG 内联计算当前流指针（栈顶 = finish._M_cur - 1），不调 GetInputStream()；
    // 边界节点回退 node[-1]+0x1f4-1，栈空时为 0。
    stream_t* incl = endIt._M_cur;
    if (incl != beginIt._M_cur)
    {
        if (incl == endIt._M_first)
            incl = *(endIt._M_node - 1) + STREAM_NODE_ELEMS;
        incl = incl - 1;
    }
    else
    {
        incl = 0;
    }
    int next = readCharFromBuffer(offset_);
    lastToken_ = next;
    if (next != DNF_TOK_STRING)
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s/%s, invalid inclusion.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    std::string include_path;
    if (!getStr(include_path))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, invalid inclusion.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    DNFLex::stream_data_t sd;
    if (!LoadStream("", toTString(include_path), &sd))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, LoadStream failed.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
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
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, PushInputStream failed.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    if (sd.stream != 0)
    {
        void (**vtable)(void*) =
            *reinterpret_cast<void (***)(void*)>(sd.stream);
        vtable[1](sd.stream);
    }
    }
recurse:
    return getToken();
}

// ---- CompiledDNFLex()（ORIG 0x8ad07c0，默认构造：create(true) 自建串表）----
CompiledDNFLex::CompiledDNFLex()
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(0),
      stringBuffer_(0)
{
    std::deque<stream_t> tmp;
    streamStack_.swap(tmp);
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(true);
}

// ---- CompiledDNFLex(ScriptStringManager*)（ORIG 0x8ad0580：create(false)）----
CompiledDNFLex::CompiledDNFLex(ScriptStringManager* pStringManager)
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(pStringManager),
      stringBuffer_(0)
{
    std::deque<stream_t> tmp;
    streamStack_.swap(tmp);
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(false);
}

// ---- ~CompiledDNFLex（ORIG 0x8ad0ed0 D1：Close → destroy → ~deque）----
CompiledDNFLex::~CompiledDNFLex()
{
    CloseInputStream();
    destroy();
}
