// ============================================================================
// CompiledDNFLex 底层方法（词法引擎基础层；ORIG 地址与汇编证据见
// docs/class_func_reports/CompiledDNFLex.md）
// ----------------------------------------------------------------------------
// 本 TU 提供（_ZN14CompiledDNFLex*）：
//   getStringManager()   _ZN14CompiledDNFLex16getStringManagerEv ORIG 0x8ad0070
//   GetInputStream()     _ZN14CompiledDNFLex14GetInputStreamEv   ORIG 0x8acfea0
//   CloseInputStream()   _ZN14CompiledDNFLex16CloseInputStreamEv ORIG 0x8ad0e90
//   PushInputStream(...) _ZN14CompiledDNFLex15PushInputStreamEPKcS1_Pcj
//                                                     ORIG 0x8ad0a00
//   SwitchInputStream()  _ZN14CompiledDNFLex17SwitchInputStreamEPKcS1_Pcj
//                                                     ORIG 0x8ad0f40
//   SetHandler(...)      _ZN14CompiledDNFLex10SetHandlerEPFvPKcEPFbS1_S1_PN6DNFLex13stream_data_tEE
//                                                     ORIG 0x8acfdf0
//
// 布局依据 ORIG CompiledDNFLex::CompiledDNFLex（0x8ad0580/0x8ad07c0）汇编：
//   +0x00 vptr（0x8e2ed20） +0x04 offset_（char*） +0x08 lastToken_
//   +0x0c stringManagerDelete_（bool） +0x10 stringManager_
//   +0x14 stringBuffer_（char*，create() 分配 0x400 字节）
//   +0x18..+0x2017 token 缓冲区（rep stos 0x800 dwords = 0x2000 字节）
//   +0x2018 stream_stack_：std::deque<stream_t>，即 _Deque_base：
//       +0x2018 _M_map  +0x201c _M_map_size
//       start{cur,first,last,node} @ +0x2020..0x202c
//       finish{cur,first,last,node} @ +0x2030..0x203c
//   +0x2040（byte 1） +0x2044（0） +0x2048（0），总大小 0x204c。
//
// stream_t（0x14 字节，见 PushInputStream 0x8ad0a00 / PopInputStream
// 0x8ad0c80 / GetInputStream 0x8acfea0 汇编）：
//   +0x00 dir（std::string） +0x04 filename（std::string）
//   +0x08 size（uint） +0x0c buffer（char*） +0x10 offset（char* 读取位置）
// deque 节点缓冲 0x1f4 字节（movl $0x1f4 分配），每节点 0x19（25）个元素。
//
// GetInputStream：空栈返回 0，否则返回栈顶 stream_t*（back()，跨节点回退
// 到 node[-1] + 0x19 - 1）。
// CloseInputStream：循环弹出直到栈空；弹出一个元素 = 释放 buffer
// （operator delete[]）+ 析构两个 std::string +（跨节点时）释放节点缓冲
// （operator delete），语义与 ORIG 循环 PopInputStream(this,true) 一致，
// 本 TU 内联实现，不额外引入 PopInputStream 符号。
// PushInputStream：dir/filename 空指针按空串；buffer 按 2 字节魔数 0xd0b0
// 校验（不匹配释放 buffer 并失败，匹配则 offset_ 从 buffer+2 开始）；
// 非空栈先把当前 offset_ 保存到旧栈顶 stream.offset；随后按真实
// std::deque<stream_t>::push_back 语义（快路径原位构造 / _M_push_back_aux
// 慢路径分配新节点，见下 streamStackPushBack）压入新流并置 offset_。
// SwitchInputStream：CloseInputStream 后尾调用 PushInputStream（ORIG jmp）。
//
// 构造/析构（按报告 0x8ad0580/0x8ad07c0/0x8ad0ed0 补全，2026-08-19）：
//   CompiledDNFLex()                _ZN14CompiledDNFLexC1Ev        ORIG 0x8ad07c0
//   CompiledDNFLex(ScriptStringManager*) _ZN14CompiledDNFLexC1EP19ScriptStringManager
//                                                                ORIG 0x8ad0580
//   ~CompiledDNFLex()               _ZN14CompiledDNFLexD1Ev        ORIG 0x8ad0ed0
//   ~CompiledDNFLex()（deleting）   _ZN14CompiledDNFLexD0Ev        ORIG 0x8ad0f20
// 两者经本 TU 内联的 create()/destroy()（ORIG 0x8ad03d0/0x8ad0370 语义）完成
// StringManager 的创建/销毁：默认构造 create(true) 在 stringManager_ 为空时
// 新建 ScriptStringManager（_ZN19ScriptStringManagerC1Ev，0x28 字节）并置
// stringManagerDelete_=true；析构在 delete 标志下经 deleting 析构
// （_ZN19ScriptStringManagerD0Ev，ORIG 经 vptr[1] 调用）释放。带参构造
// create(false) 只接管外部传入的 manager 指针，不取得所有权。
// 构造/析构均写 vptr（ORIG 0x8e2ed20），vtable 由本 TU 定义（_ZTV14CompiledDNFLex，
// 槽 0=D1、槽 1=D0，与 ScriptStringManager vtable 布局同）。deque 初始化用
// _M_initialize_map 等价语义（map 8 槽、1 节点置 map[3]、start=finish=节点首），
// 与 streamStackPushBack 的惰性初始化一致；析构在 CloseInputStream 之后释放
// 剩余空节点与 map（~deque 语义）。
// ============================================================================

#include <cstring>
#include <new>
#include <string>

namespace
{
// stream_t 布局代理（0x14 字节）。用真实 std::string 成员以便编译器生成
// 与 libstdc++ 一致的 string 析构/引用计数释放。offset 为读取位置指针
// （ORIG PushInputStream: mov this->offset_, stream+0x10；PopInputStream
// 恢复 offset_ = stream.offset，同 4 字节互换）。
struct CompiledStream
{
    std::string  dir;      // +0x00
    std::string  filename; // +0x04
    unsigned int size;     // +0x08
    char*        buffer;   // +0x0c
    char*        offset;   // +0x10

    CompiledStream() : dir(), filename(), size(0), buffer(0), offset(0) {}
    CompiledStream(const std::string& d, const std::string& f)
        : dir(d), filename(f), size(0), buffer(0), offset(0) {}
};

// CompiledDNFLex 布局代理（0x204c 字节）。
struct CompiledLexLayout
{
    void*         vptr;              // +0x00
    char*         offset_;           // +0x04
    int           lastToken_;        // +0x08
    bool          stringManagerDelete_; // +0x0c
    void*         stringManager_;    // +0x10  ScriptStringManager*
    char*         stringBuffer_;     // +0x14
    unsigned char tokenBuffer_[0x2000]; // +0x18
    // ---- std::deque<stream_t>（_Deque_base）@ +0x2018 ----
    CompiledStream** map_;          // +0x2018
    unsigned int  mapSize_;         // +0x201c
    CompiledStream* startCur_;       // +0x2020
    CompiledStream* startFirst_;     // +0x2024
    CompiledStream* startLast_;      // +0x2028
    CompiledStream** startNode_;     // +0x202c
    CompiledStream* finishCur_;      // +0x2030
    CompiledStream* finishFirst_;    // +0x2034
    CompiledStream* finishLast_;     // +0x2038
    CompiledStream** finishNode_;    // +0x203c
    unsigned char flag2040_;             // +0x2040
    char pad2041_[3];                    // +0x2041..0x2043
    void (*lexerError_)(const char*);    // +0x2044
    int (*loadStream_)(const char*, const char*, void*); // +0x2048
};

enum
{
    DEQUE_ELEMS_PER_NODE = 0x19  // 25 个元素 / 节点（0x1f4 字节）
};

// 在 pos 处原位构造 stream_t（dir/filename 拷贝构造，size/buffer/offset
// 赋值）。ORIG PushInputStream 0x8ad0b1f/0x8ad0b31 即 string 拷贝构造 +
// 三个字段赋值；拷贝构造失败时编译器自动析构已构造成员。
static void constructStream(CompiledStream* pos, const std::string& dir,
                            const std::string& filename, unsigned int size,
                            char* buffer, char* offset)
{
    new (pos) CompiledStream(dir, filename);
    pos->size = size;
    pos->buffer = buffer;
    pos->offset = offset;
}

// ---- ScriptStringManager 生命周期符号（ScriptStringManager.cpp 提供真实
// C++ 类实现：C1 为 0x28 字节构造，D0 为 deleting 析构）。CompiledDNFLex
// 的 create()/destroy()（ORIG 0x8ad03d0/0x8ad0370）按 ORIG 语义调用它们。----

// std::deque<stream_t>::_Deque_base::_M_initialize_map 等价语义（ORIG 构造
// 0x8ad05fc/0x8ad066e 调用 _M_initialize_map(0) 后 swap 到 this，最终状态：
// map 8 槽、1 个节点置 map[3]、start=finish=节点首元素）。与
// streamStackPushBack 的惰性初始化分支完全一致，故抽出复用；此后
// PushInputStream 快路径（finishCur_ != finishLast_-1）直接原位构造。
// 节点分配失败时释放已分配的 map 后重抛（_M_initialize_map 语义）。
static void compiledLexInitDeque(CompiledLexLayout* c)
{
    c->mapSize_ = 8;
    c->map_ = new CompiledStream*[c->mapSize_];
    for (unsigned int i = 0; i < c->mapSize_; ++i)
        c->map_[i] = 0;
    CompiledStream* node = 0;
    try
    {
        node = static_cast<CompiledStream*>(::operator new(
            DEQUE_ELEMS_PER_NODE * sizeof(CompiledStream)));
    }
    catch (...)
    {
        delete[] c->map_;
        c->map_ = 0;
        throw;
    }
    c->map_[3] = node;
    c->startNode_ = c->map_ + 3;
    c->startFirst_ = node;
    c->startLast_ = node + DEQUE_ELEMS_PER_NODE;
    c->startCur_ = node;
    c->finishNode_ = c->map_ + 3;
    c->finishFirst_ = node;
    c->finishLast_ = node + DEQUE_ELEMS_PER_NODE;
    c->finishCur_ = node;
}

// CompiledDNFLex::create(bool)（ORIG 0x8ad03d0）：
//   createStringTable=true：stringManager_ 为空时 operator new(0x28) 并
//   ScriptStringManager 构造（失败经 catch handler 释放内存后重抛），随后
//   stringManagerDelete_=true；false：跳过整个分支（带参构造只接管外部
//   manager 指针，不取得所有权）。
//   stringBuffer_ 为空时 operator new[](0x400)（_Znaj，ERR_BUF 读取缓冲）。
static void compiledLexCreate(CompiledLexLayout* c, int createStringTable)
{
    if (createStringTable)
    {
        if (c->stringManager_ == 0)
        {
            void* mem = operator new(0x28);
            try
            {
                sub_ScriptStringManager_C1(mem);
            }
            catch (...)
            {
                operator delete(mem);
                throw;
            }
            c->stringManager_ = mem;
        }
        c->stringManagerDelete_ = true;
    }
    if (c->stringBuffer_ == 0)
        c->stringBuffer_ = new char[0x400];
}

// CompiledDNFLex::destroy()（ORIG 0x8ad0370）：
//   stringManagerDelete_==false：仅 stringManager_=0（外部所有权，不释放）；
//   true：stringManager_ 非空时经 deleting 析构释放（ORIG 经 vptr[1] 调用
//   D0，与 ScriptStringManager vtable 槽 1=D0 一致），随后置 0 并清 delete
//   标志；最后 stringBuffer_ 非空时 operator delete[] 并置 0。
static void compiledLexDestroy(CompiledLexLayout* c)
{
    if (!c->stringManagerDelete_)
    {
        c->stringManager_ = 0;
    }
    else
    {
        if (c->stringManager_ != 0)
        {
            sub_ScriptStringManager_D0(c->stringManager_);
            c->stringManager_ = 0;
        }
        c->stringManagerDelete_ = false;
    }
    if (c->stringBuffer_ != 0)
    {
        operator delete[](c->stringBuffer_);
        c->stringBuffer_ = 0;
    }
}

// std::deque<stream_t>::~deque 的资源释放部分（在 CloseInputStream 弹空之后）：
// 释放 startNode_..finishNode_ 的节点缓冲（此时栈空，剩余恰为初始的空节点，
// 防御性遍历）并 delete[] map。ORIG 析构 0x8ad0ed0 尾调用
// _ZNSt5deque…D1Ev（0x8ad15c0）；CloseInputStream 循环只释放元素所在节点，
// 初始空节点与 map 由这里回收。
static void compiledLexReleaseDeque(CompiledLexLayout* c)
{
    if (c->map_ == 0)
        return;
    for (CompiledStream** p = c->startNode_; p <= c->finishNode_; ++p)
    {
        if (*p != 0)
            operator delete(*p);
    }
    delete[] c->map_;
    c->map_ = 0;
}

// std::deque<stream_t>::push_back 语义（ORIG 快路径 + _M_push_back_aux
// 慢路径，汇编 0x8ad0b00/0x8ad0ba8 分支 + _ZNSt5deque…16_M_push_back_aux）：
//   快路径：finish 未到节点最后槽（finish._M_last - 1）→ 原位构造 + ++cur；
//   慢路径：finish 已是最后槽 → _M_reserve_map_at_back(1)（map 尾部满则
//   扩容 2*(mapSize+2) 槽、节点指针移到中间）→ 分配新节点挂接 →
//   构造在旧节点最后槽 → finish 跳到新节点首元素（半开区间终点，未构造）。
// 若 deque 尚未初始化（map_==0；ORIG 由 CompiledDNFLex 构造完成
// _M_initialize_map，本 TU 构造亦已初始化，此处为绕过构造调用方的防御
// 兜底），先等价初始化：map 8 槽、1 个节点置于中间（map[3]），
// start=finish=节点首元素。
static void streamStackPushBack(CompiledLexLayout* c, const std::string& dir,
                                const std::string& filename, unsigned int size,
                                char* buffer, char* offset)
{
    if (c->map_ == 0)
        compiledLexInitDeque(c);

    if (c->finishCur_ != c->finishLast_ - 1)
    {
        // 快路径：原位构造，finishCur++。
        constructStream(c->finishCur_, dir, filename, size, buffer, offset);
        ++c->finishCur_;
        return;
    }

    // 慢路径：_M_push_back_aux（先 _M_reserve_map_at_back(1)）。
    if (c->finishNode_ == c->map_ + c->mapSize_ - 1)
    {
        unsigned int oldNum = static_cast<unsigned int>(c->finishNode_ -
                                                        c->startNode_) + 1;
        unsigned int oldBase =
            static_cast<unsigned int>(c->startNode_ - c->map_);
        unsigned int newSize = 2 * (c->mapSize_ + 2);
        CompiledStream** newMap = new CompiledStream*[newSize];
        for (unsigned int i = 0; i < newSize; ++i)
            newMap[i] = 0;
        unsigned int base = (newSize - oldNum) / 2;
        for (unsigned int i = 0; i < oldNum; ++i)
            newMap[base + i] = c->map_[oldBase + i];
        delete[] c->map_;
        c->map_ = newMap;
        c->mapSize_ = newSize;
        c->startNode_ = newMap + base;
        c->finishNode_ = newMap + base + (oldNum - 1);
    }
    CompiledStream* newNode =
        static_cast<CompiledStream*>(::operator new(
            DEQUE_ELEMS_PER_NODE * sizeof(CompiledStream)));
    try
    {
        // 构造在旧节点最后槽（_M_push_back_aux 先 construct 后 _M_set_node）。
        constructStream(c->finishCur_, dir, filename, size, buffer, offset);
    }
    catch (...)
    {
        ::operator delete(newNode);
        throw;
    }
    ++c->finishNode_;
    *c->finishNode_ = newNode;
    c->finishFirst_ = newNode;
    c->finishLast_ = newNode + DEQUE_ELEMS_PER_NODE;
    c->finishCur_ = newNode;  // 新节点首元素（空槽，区间终点）
}
}  // namespace

// getStringManager（ORIG 0x8ad0070）：返回 stringManager_（+0x10）。
extern "C" void* sub_CompiledDNFLex_getStringManager(void* self)
    asm("_ZN14CompiledDNFLex16getStringManagerEv");
extern "C" void* sub_CompiledDNFLex_getStringManager(void* self)
{
    CompiledLexLayout* c = reinterpret_cast<CompiledLexLayout*>(self);
    return c->stringManager_;
}

// GetInputStream（ORIG 0x8acfea0）：空栈返回 0；否则返回栈顶 stream_t*。
// finish._M_cur == finish._M_first 时栈顶在上一节点末尾（node[-1]+0x19-1）。
extern "C" void* sub_CompiledDNFLex_GetInputStream(void* self)
    asm("_ZN14CompiledDNFLex14GetInputStreamEv");
extern "C" void* sub_CompiledDNFLex_GetInputStream(void* self)
{
    CompiledLexLayout* c = reinterpret_cast<CompiledLexLayout*>(self);
    if (c->finishCur_ == c->startCur_)
        return 0;
    CompiledStream* back;
    if (c->finishCur_ == c->finishFirst_)
        back = c->finishNode_[-1] + (DEQUE_ELEMS_PER_NODE - 1);
    else
        back = c->finishCur_ - 1;
    return back;
}

// CloseInputStream（ORIG 0x8ad0e90）：循环弹出直到栈空（等价 ORIG
// while (finish != start) PopInputStream(this, true)）。每个元素：
//   1) 若 buffer 非空 operator delete[]（_ZdaPv）；
//   2) 析构两个 std::string（_ZNSsD1Ev 语义，refcount 递减）；
//   3) 若该元素是节点第一个（finish._M_cur == finish._M_first），先释放
//      节点缓冲（operator delete，_ZdlPv）并回退 finish 到上一节点末尾。
typedef void (*CompiledLexDtorFn)(void*);
CompiledLexDtorFn g_compiledDnfLexVtable[2] __asm__("_ZTV14CompiledDNFLex") =
{
    sub_CompiledDNFLex_D1,
    sub_CompiledDNFLex_D0
};

// ---- CompiledDNFLex::CompiledDNFLex()（ORIG 0x8ad07c0，默认构造）----
// 字段初始值（汇编 0x8ad07d8..0x8ad07f7）：vptr=0x8e2ed20、offset_=0、
// lastToken_=DNF_TOK_EOF(0)、stringManagerDelete_=false、stringManager_=0、
// stringBuffer_=0；随后 deque _M_initialize_map（等价初始化）、
// ERR_BUF(tokenBuffer_) rep stos 清零、+0x2040=1（allow_inclusion_）、
// +0x2044=0（LexerError_）、+0x2048=0（LoadStream_），最后尾调用
// create(this,true)（0x8ad09bb call 8ad03d0，arg1=1）——默认构造创建并
// 持有自己的 ScriptStringManager。
extern "C" void sub_CompiledDNFLex_C1EP19(void* self, void* pStringManager)
{
    CompiledLexLayout* c = reinterpret_cast<CompiledLexLayout*>(self);
    c->vptr = g_compiledDnfLexVtable;
    c->stringManager_ = pStringManager;
    c->offset_ = 0;
    c->lastToken_ = 0;                 // DNF_TOK_EOF == 0
    c->stringManagerDelete_ = false;
    c->stringBuffer_ = 0;
    compiledLexInitDeque(c);           // _M_initialize_map(0) 等价
    std::memset(c->tokenBuffer_, 0, sizeof(c->tokenBuffer_));
    c->flag2040_ = 1;
    c->lexerError_ = 0;
    c->loadStream_ = 0;
    try
    {
        compiledLexCreate(c, 0);       // create(this,false)
    }
    catch (...)
    {
        // ORIG handler（0x8ad078f/0x8ad07a4）析构 this->stream_stack_ 后重抛。
        compiledLexReleaseDeque(c);
        throw;
    }
}

// ---- CompiledDNFLex::~CompiledDNFLex()（ORIG 0x8ad0ed0，D1）----
// 汇编：vptr=0x8e2ed20 → CloseInputStream（0x8ad0ee3 call 0x8ad0e90）→
// destroy（0x8ad0eeb call 0x8ad0370）→ 尾调用 deque::~deque(+0x2018)
// （0x8ad0efe jmp 0x8ad15c0）。异常路径 handler（0x8ad0f03）先析构 deque
// 再 _Unwind_Resume。
extern "C" void sub_CompiledDNFLex_D1(void* self)
{
    CompiledLexLayout* c = reinterpret_cast<CompiledLexLayout*>(self);
    c->vptr = g_compiledDnfLexVtable;
    try
    {
        sub_CompiledDNFLex_CloseInputStream(self);
        compiledLexDestroy(c);
    }
    catch (...)
    {
        compiledLexReleaseDeque(c);
        throw;
    }
    compiledLexReleaseDeque(c);        // ~deque：初始空节点 + map
}

// ---- CompiledDNFLex::~CompiledDNFLex()（ORIG 0x8ad0f20，D0 deleting）----
// 汇编：call D1 后尾调用 operator delete（0x8ad0f3a jmp _ZdlPv）。vtable
// 槽 1 经虚析构触发（如 delete compiledDnfLex_）。
extern "C" void sub_CompiledDNFLex_D0(void* self)
{
    sub_CompiledDNFLex_D1(self);
    operator delete(self);
}
