// ============================================================================
// PackSet.cpp —— Nexon FilePack（../../Include/FilePack/Pack.cpp）重建实现
//
// 覆盖 ORIG 域符号：
//   PackSet 32 符号（/tmp/only_orig/PackSet.txt，docs/class_func_reports/PackSet/）
//   IndexEntry 22 符号（/tmp/only_orig/IndexEntry.txt）
//   IPackFileProcess 7 符号 + 静态数据（s_common_buffer_ 等）
//   Crc32 2 符号 + TU 局部 make_crc_table/crc_table（ORIG _ZL14make_crc_tablev
//     t @0x08ad3420 / b _ZL9crc_table @0x09501a40）
//   Pack.cpp 静态 helper：_encrypt/_decrypt_N_crc32/_hash/_index_less/
//     _index_less2/_write_hash/_fread_str/_fwrite_str/_mread_str +
//     d 数据 TEMP_HEADER_FILENAME("header.tmp")
//   _correct_filename（T 全局 @0x08ac3b9a）
//
// XOR_KEY(B @0x0950189c，初值 0) / g_AdditionalKey(D @0x093f5680，初值
// 0x81a79011) 为 ORIG extern "C" 全局 —— 本 TU 唯一 owner。
// ============================================================================
#include "PackSet.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stack>

#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy
#include "SyncScriptDeps.h"                               // toMbcs/toTString
#include "Tick.h"                                         // verify() 计时

// [b22] char*→char[]：ORIG 在所有 fprintf 错误串将该源文件串作立即数直压
// （@0x8e2e7d0），char* 全局指针会 deref（每处 +1 mov）；char[] 在实参处衰减为
// 地址立即数，与 ORIG 同形（P26/P183 型）。
static const char k_srcFile[] = "../../Include/FilePack/Pack.cpp";

// ---------------------------------------------------------------------------
// Pack.cpp 域静态数据（ORIG .data/.bss）
// ---------------------------------------------------------------------------
unsigned char* IPackFileProcess::s_common_buffer_ = 0;       // 0x095018a0 B
unsigned int   IPackFileProcess::s_common_buffer_size_ = 0;  // 0x095018a4 B
int            IPackFileProcess::s_ref_count_ = 0;           // 0x095018a8 B

extern "C" unsigned int XOR_KEY = 0;                   // 0x0950189c B
extern "C" unsigned int g_AdditionalKey = 0x81a79011u; // 0x093f5680 D

static char const* TEMP_HEADER_FILENAME = "header.tmp"; // d @0x093f5684 → "header.tmp"

static unsigned int crc_table[256];                     // b _ZL9crc_table @0x09501a40

// make_crc_table —— docs/class_func_reports/_lt_global_gt_/make_crc_table.md
// （ORIG _ZL14make_crc_tablev t @0x08ad3420）
static void make_crc_table()
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ad3420 size 0x8d）：
    // ①槽序：ORIG i=-0xc / j=-0x8 / h=-0x4，且 4.4 全头部声明时与 4.1 同为
    //   「先声明深」单调（P212）⇒ 三个局部全部提至函数头、声明序 i,j,h，
    //   块内仅赋值（旧形 h 先声明得 -0xc、for-init 的 i/j 得 -0x8/-0x4 全错位）。
    // ②三目极性：ORIG `and $1; test %al; je <mov $0>; mov $poly; jmp; <mov $0>`
    //   ⇒ 条件取真走 poly 的 fall-through，源写正极性 `(h & 1u) ? poly : 0`
    //   （旧形 `==0 ? 0 : poly` 反产 jne 跳 poly）。
    unsigned int i, j, h;
    h = 1u;
    crc_table[0] = 0;
    for (i = 0x80u; i != 0; i >>= 1)
    {
        h = (h >> 1) ^ ((h & 1u) ? 0xedb88320u : 0u);
        for (j = 0u; j < 0x100u; j += i * 2u)
        {
            crc_table[i + j] = crc_table[j] ^ h;
        }
    }
}

// Crc32 —— docs/class_func_reports/Crc32/{crc32,getCrc32Table}.md
unsigned int Crc32::crc32(unsigned int crc, char const* buf, unsigned int len)
{
    // b28d-solo-PackSet 判词（N 封顶，ORIG 0x08ad34ae size 0x55）：body 的
    // `crc>>8` 求值序已由 `(crc>>8) ^ table[...]` 书写序翻正；唯一残差 = cond 段
    // `subl $1,len` 位置——ORIG `cmp;setne;subl;test;jne`（条件先物化后递减），
    // 4.4 恒 `subl;cmp;setne;test`（递减提前）。for/while/前置/后置四形实测均
    // 不可翻 ⇒ 4.4 循环旋转固有差（N82/P112 族），语义等价。
    if (crc_table[255] == 0)
    {
        make_crc_table();
    }
    crc = ~crc;
    // b28d-solo-PackSet（P243 同族）：ORIG body 首两条即 `mov crc,%eax; mov %eax,%edx;
    // shr $8,%edx`（右操作数 `crc>>8` 先求值）⇒ 源把 `crc >> 8` 写在 `^` 左侧。
    while (len != 0)
    {
        crc = (crc >> 8) ^ crc_table[(static_cast<unsigned char>(*buf) ^ crc) & 0xffu];
        ++buf;
        len--;
    }
    return ~crc;
}

unsigned int* Crc32::getCrc32Table()
{
    if (crc_table[255] == 0)
    {
        make_crc_table();
    }
    return crc_table;
}

// ---------------------------------------------------------------------------
// Pack.cpp 静态 helper（ORIG t @0x08ac3714..0x08ac3b98）
// ---------------------------------------------------------------------------

// _hash —— _lt_global_gt_/_hash.md（t @0x08ac3b04）：DJB2 变体 h=h*33+(signed)c
static hash_t _hash(char const* src)
{
    signed char c = static_cast<signed char>(src[0]);
    unsigned long h = 0;
    h = 0x1505;   // ORIG movl $0 + movl $0x1505（O0 保留死赋值）
    while (c != 0)
    {
        c = static_cast<signed char>(src[0]);   // ORIG 循环体开头重读
        ++src;
        h = (h << 5) + h + c;
    }
    return h;
}

// _index_less —— _lt_global_gt_/_index_less.md（t @0x08ac3adc）
static bool _index_less(PackSet::index_pair_t const& lhs, PackSet::index_pair_t const& rhs)
{
    return lhs.first < rhs.first;
}

// _index_less2 —— _lt_global_gt_/_index_less2.md（t @0x08ac3af0）
static bool _index_less2(PackSet::index_pair2_t const& lhs, PackSet::index_pair2_t const& rhs)
{
    return lhs.first < rhs.first;
}

// _write_hash —— _lt_global_gt_/_write_hash.md（t @0x08ac3b55）
// 结果 = (fileHash & 0xffff0000) | (dirHash >> 16)
static hash_t _write_hash(char const* file, char const* dir)
{
    unsigned long fileHash = _hash(toMbcs(file));
    unsigned long dirHash = _hash(toMbcs(dir));
    return (fileHash & 0xffff0000ul) | (dirHash >> 16);
}

// _encrypt —— _lt_global_gt_/_encrypt.md（t @0x08ac38be）
// 每双字 rotr26(rotl6) 后与 key 异或；4 字节对齐校验失败返回 PACK_ERROR。
static pack_ret_t _encrypt(unsigned int key, unsigned char* buffer, unsigned int size)
{
    if ((size & 3u) != 0u)
    {
        fprintf(stdout, "%s\n", "4byte align required");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5b, __FUNCTION__);
        return PACK_ERROR;
    }
    if (XOR_KEY != 0u)
    {
        key ^= XOR_KEY;
    }
    unsigned int* p = reinterpret_cast<unsigned int*>(buffer);
    unsigned int* e = p + (size >> 2);        // ORIG shr+shl+add p 槽形态
    for (; p != e; ++p)
    {
        *p = (((*p) >> 0x1a) | ((*p) << 6)) ^ key;   // ORIG ror$26 单读：rotate 识别消 v 槽
        if (g_AdditionalKey != 0u)
        {
            *p ^= g_AdditionalKey;
        }
    }
    return PACK_OK;
}

// _decrypt_N_crc32 —— _lt_global_gt_/_decrypt_N_crc32.md（t @0x08ac398b）
// 解密（rotl6+key）并逐字节 CRC32 校验；解得 ~crc==checksum 返回 1。
static int _decrypt_N_crc32(unsigned long crc, unsigned long checksum,
                            unsigned long key, unsigned char* buffer,
                            unsigned int size)
{
    // b28d-solo-PackSet 判词（N 封顶，ORIG 0x08ac398b size 0x150）：e 的
    // `shr $2; shl $2`（`p + (size>>2)*4` 形）、rotate 单表达式、`(x>>8)^table`
    // 求值序、crc 自身累积（无 tmp 槽）均已对齐；唯一残差 = 帧 ORIG `sub $0x18`
    // vs 4.4 `sub $0x10`（0x18 非 16k 量化值；P190② 4.4 带 call 帧恒 16k+4
    // 量化，dead 局部/double pad 两形实测不可凑）⇒ 槽位整体浅 8，语义等价封顶。
    if ((size & 3u) != 0u)
    {
        return 0;
    }
    unsigned int const* table = Crc32::getCrc32Table();
    crc = ~crc;
    if (XOR_KEY != 0u)
    {
        key ^= XOR_KEY;
    }
    unsigned int* p = reinterpret_cast<unsigned int*>(buffer);
    // b28d-solo-PackSet：ORIG 0x08ac39c7 为 `shr $2; shl $2`（= (size>>2)<<2），
    // 非 `and $0xfffffffc`（4.4 对 &~3 折叠成单条 and，两条形不同）。
    unsigned int* e = p + ((size >> 2) * 4);
    for (; p != e; ++p)
    {
        if (g_AdditionalKey != 0u)
        {
            *p ^= g_AdditionalKey;
        }
        // b28d-solo-PackSet（P0）：ORIG 无 v 栈槽（`mov (%eax),%eax; xor key; mov
        // %eax,%edx; ror $6,%edx; mov %edx,(%eax)`）⇒ 原码是单表达式右值直传，
        // 4.4 对同式识别 rotate 并共用单次 `*p` 装载。
        *p = ((*p ^ key) >> 6) | ((*p ^ key) << 0x1a);
        unsigned char const* bytes = reinterpret_cast<unsigned char const*>(p);
        // b28d-solo-PackSet（P243 同族）：ORIG 每段先算 `x >> 8` 到 %edx 再取表值
        // ⇒ `(x >> 8) ^ table[...]` 书写序。
        // ORIG 每段把结果写回 crc 形参槽（`mov %eax,0x8(%ebp)`）⇒ 无 tmp 具名局部，
        // 直接用 crc 自身累积（tmp 会多占一个栈槽使全帧浅移 4）。
        crc = (crc >> 8) ^ table[(bytes[0] ^ crc) & 0xffu];
        crc = (crc >> 8) ^ table[(bytes[1] ^ crc) & 0xffu];
        crc = (crc >> 8) ^ table[(bytes[2] ^ crc) & 0xffu];
        crc = (crc >> 8) ^ table[(bytes[3] ^ crc) & 0xffu];
    }
    return (~crc == checksum) ? 1 : 0;
}

// _fwrite_str —— _lt_global_gt_/_fwrite_str.md（t @0x08ac3714）：len(4B)+内容
static pack_ret_t _fwrite_str(char const* src, FILE* fp)
{
    if (src == 0 || fp == 0)
    {
        return PACK_ERROR;
    }
    char const* mb = toMbcs(src);
    unsigned int len = strlen(mb);
    fwrite(&len, 4, 1, fp);
    fwrite(mb, len, 1, fp);
    return PACK_OK;
}

// _fread_str —— _lt_global_gt_/_fread_str.md（t @0x08ac3793）：len(<0x104)+内容
static pack_ret_t _fread_str(std::string& dst, FILE* fp)
{
    if (fp == 0)
    {
        return PACK_ERROR;
    }
    // ORIG 无 len 预清槽（fread 失败路径 len 未初始化为原码遗留，P171）。
    unsigned int len;
    fread(&len, 4, 1, fp);
    if (len > 0x103u)
    {
        return PACK_ERROR;
    }
    char buf[256] __attribute__((aligned(8)));   // ORIG 槽 -0x110（8 对齐放置；aligned(4)/(8)/(16) 探针实测仅 8 命中 0x8ac37f5）
    fread(buf, len, 1, fp);
    buf[len] = '\0';
    dst = toTString(buf);
    return PACK_OK;
}

// _mread_str —— _lt_global_gt_/_mread_str.md（t @0x08ac3832）：len(<0x104)+内容
static pack_ret_t _mread_str(std::string& dst, unsigned char*& mem)
{
    unsigned int len = *reinterpret_cast<unsigned int*>(mem);
    mem += 4;
    if (len > 0x103u)
    {
        return PACK_ERROR;
    }
    char buf[256] __attribute__((aligned(8)));   // 同 _fread_str（ORIG 槽 -0x110 @0x8ac3882）
    memcpy(buf, mem, len);
    mem += len;
    buf[len] = '\0';
    dst = toTString(buf);
    return PACK_OK;
}

// ---------------------------------------------------------------------------
// _correct_filename —— _lt_global_gt_/_correct_filename.md（T @0x08ac3b9a）
// 路径规范化：分隔符统一 '/'、大写字母小写化、按分量栈消解 "."/".."；
// 结果写入 dst；若另提供 dir/dir_size，则把消解后的目录前缀拷入 dir。
// 失败返回 1，成功 0。
// ---------------------------------------------------------------------------
int _correct_filename(char const* src, char* dst, unsigned int dst_size,
                      char* dir, unsigned int dir_size)
{
    if (src == 0 || dst == 0)
    {
        return 1;
    }

    // b28d-solo-PackSet 判词（N108 封顶，ORIG 0x08ac3b9a size 0x247）：
    // ①p167 判据：ORIG 全部 stack 调用为 **非 const** 版 `std::stack<char*>`
    //   （push(char* const&) 左值、top 返 char*）；旧 `stack<char const*>` 使 p167
    //   出现整组 EXTRA/MISSING（push 走右值转换 + top 返回类型不同）。改后 p167
    //   16=16 CLEAR。
    // ②ORIG 序：`movl $0,-0x18`(dead 局部) → w=dst(-0x44) → r=src(-0x10) →
    //   deque C1(-0x40) → stack C1(-0x6c) → push ⇒ 先赋值指针再构造 STL（P212）。
    int prev_tok = 0;                // -0x18（ORIG dead 局部，Ghidra local_1c 无读）
    (void)prev_tok;
    char* w = dst;                   // -0x44
    char const* r = src;             // -0x10
    std::stack<char*> st;            // 分量起点栈（ORIG stack<char*, deque<char*>>）
    st.push(dst);                    // 初始哨兵

    char c;
    for (;;)
    {
        c = *r++;
        if (c == 0)
        {
            break;
        }
        if (c == '/' || c == '\\')
        {
            *w++ = '/';
        }
        else
        {
            if ('A' <= c && c <= 'Z')
            {
                *w++ = static_cast<char>(c + 0x20);
            }
            else
            {
                *w++ = c;
            }
            continue;                 // 普通字符不触发分量结算（ORIG jmp 读下个字符）
        }

        // ---- 分量结算（ORIG @08ac3caa）----
        char* seg = st.top();             // -0x14
        unsigned long diff = static_cast<unsigned long>(w - seg);
        if (diff == 1)
        {
            w = seg;                                 // 段为空
        }
        else if (diff == 2 && seg[0] == '.')
        {
            w = seg;                                 // 抛弃 "./"
        }
        else if (diff == 3 && seg[0] == '.' && seg[1] == '.')
        {
            if (st.empty())
            {
                return 1;                            // 空栈分支实际不可达，忠实保留
            }
            st.pop();
            w = st.top();
        }
        else
        {
            st.push(w);
        }
    }

    *w = '\0';

    if (dir == 0)
    {
        return 0;
    }

    char* tail = st.top();
    ss_strcpy(dir, dir_size, tail);
    if (tail == dst)
    {
        tail[0] = '\0';
    }
    else
    {
        tail[-1] = '\0';                            // 去尾段（ORIG sptr[-1]=0）
    }
    return 0;
}

// ---------------------------------------------------------------------------
// IndexEntry（ORIG 0x08ac3de2..0x08ac5b4x）
// ---------------------------------------------------------------------------
IndexEntry::IndexEntry()
{
    clear();
}

// ORIG W @0x08ac7a50：隐式逐成员拷贝
IndexEntry::IndexEntry(const IndexEntry& rhs)
    : type_(rhs.type_), name_(rhs.name_), size_(rhs.size_),
      aligned_size_(rhs.aligned_size_), crc_(rhs.crc_), offset_(rhs.offset_),
      content_(rhs.content_)
{
}

// ORIG @0x08ac3e04：仅析构 name_，不释放 content_
IndexEntry::~IndexEntry()
{
}

// ORIG W @0x08ac9d92：隐式逐成员赋值
IndexEntry& IndexEntry::operator=(const IndexEntry& rhs)
{
    // over-guard 修正（见 docs/function_audit/OVER_GUARD_SWEEP.tsv）：ORIG @0x08ac9d92
    // 是**隐式逐成员赋值**，序言之后直接 `mov 0xc(%ebp),%eax` 开始逐字段拷贝，
    // **没有** `mov this; cmp &rhs; je` 自赋值检查。补上该检查会多出 3 条
    // （cmp + je + mov），使 ndiff=3 / gap=-3。与 P26「跳 pad 拷贝」同族：
    // 这类手写 operator= 不要习惯性加自赋值保护，以 ORIG 指令序为准。
    type_ = rhs.type_;
    name_ = rhs.name_;
    size_ = rhs.size_;
    aligned_size_ = rhs.aligned_size_;
    crc_ = rhs.crc_;
    offset_ = rhs.offset_;
    content_ = rhs.content_;
    return *this;
}

void IndexEntry::clear()   // @0x08ac3e1a
{
    type_ = ENTRY_PACK;
    size_ = 0;
    aligned_size_ = 0;
    crc_ = 0;
    offset_ = 0;
    content_ = 0;
}

entry_type_t IndexEntry::type() const          { return type_; }            // 0x08ac3e5a
const tstring& IndexEntry::name() const        { return name_; }            // 0x08ac3e64
unsigned int IndexEntry::size() const          { return size_; }            // 0x08ac3e70
unsigned int IndexEntry::aligned_size() const  { return aligned_size_; }    // 0x08ac3e7c
hash_t IndexEntry::crc() const                 { return crc_; }             // 0x08ac3e88
unsigned int IndexEntry::offset() const        { return offset_; }          // 0x08ac3e94
unsigned char* IndexEntry::content() const     { return content_; }         // 0x08ac3ea0

pack_ret_t IndexEntry::read_info(unsigned char*& mem)   // @0x08ac4200
{
    _mread_str(name_, mem);
    size_ = *reinterpret_cast<unsigned int*>(mem);
    mem += 4;
    aligned_size_ = (size_ + 3u) & ~3u;
    crc_ = *reinterpret_cast<unsigned long*>(mem);
    mem += 4;
    offset_ = *reinterpret_cast<unsigned int*>(mem);
    mem += 4;
    return PACK_OK;
}

pack_ret_t IndexEntry::write_info(FILE* fp)   // @0x08ac4284
{
    if (fp == 0)
    {
        fprintf(stdout, "%s\n", "fp is NULL");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1d0, __FUNCTION__);
        return PACK_ERROR;
    }
    _fwrite_str(name_.c_str(), fp);
    fwrite(&size_, 4, 1, fp);
    fwrite(&crc_, 4, 1, fp);
    fwrite(&offset_, 4, 1, fp);
    return PACK_OK;
}

// 主版本（dst 预分配）@0x08ac3eac
// b28d-solo-PackSet 判词（AE）：p167 20=20 CLEAR；旧 b28d-36「N95 帧差封顶」
// 判词已作废——消除 pos 具名局部（实参内联，P0/P52）后帧差归零。
pack_ret_t IndexEntry::get_content(PackSet* owner, hash_t code, unsigned char* dst,
                                   unsigned int dst_size, unsigned int* content_size)
{
    if (owner == 0 || dst == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x138, __FUNCTION__);
        return PACK_ERROR;
    }

    if (owner->UsingEncryptedFile())
    {
        return get_contentEncryptedFile(owner, code, dst, dst_size, content_size);
    }

    *content_size = 0;
    // [b23S3C] P198②：ORIG 直测 call 返回寄存器（test/sete 形，无 fp 槽）——谓词内联，勿提具名局部。
    if (owner->pack_file() == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x145, __FUNCTION__);
        return PACK_ERROR;
    }
    if (dst_size < size_)
    {
        fprintf(stdout, "%s\n", "buffer size is too small");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x14b, __FUNCTION__);
        return PACK_ERROR;
    }

    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac3eac size 0x36c）：
    // ORIG `mov 0x18(%eax),%eax; test; je +0x173`（content_==0 走**跳转**臂，
    // memcpy 直落 then 位）⇒ 源写正极性 `if (content_ != 0) { memcpy } else {...}`
    // （旧形 `==0` 反产 `jne` 跳 else、两块整体互换；P214①/P220/P222）。
    if (content_ != 0)
    {
        memcpy(dst, content_, aligned_size_);
    }
    else
    {
        if (type_ == ENTRY_FILE)
        {
            fprintf(stdout, "%s\n", "not implemented");
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x158, __FUNCTION__);
            return PACK_ERROR;
        }
        if (type_ == ENTRY_PACK && aligned_size_ != 0)
        {
            // b28d-solo-PackSet（P0/P52）：ORIG 无 pos 栈槽（`add %edx,%eax;
            // mov %eax,%ebx` 直接升格 callee-saved 跨 pack_file() 保活）⇒ 实参内联。
            fseek(owner->pack_file(),
                  static_cast<long>(owner->header_size() + offset_), SEEK_SET);
            if (fread(dst, aligned_size_, 1, owner->pack_file()) != 1)
            {
                fprintf(stdout, "%s\n", "fread failed");
                fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x163, __FUNCTION__);
                return PACK_ERROR;
            }
        }
    }

    *content_size = size_;
    if (_decrypt_N_crc32(code, crc_, crc_, dst, aligned_size_) == 0)
    {
        fprintf(stdout, "%s\n", "CRC failed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x16e, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

// 分配版本 @0x08ac5880：new[] 后转发到主版本
pack_ret_t IndexEntry::get_content(PackSet* owner, hash_t code,
                                   unsigned char** dst, unsigned int* content_size)
{
    if (dst == 0)
    {
        return PACK_ERROR;
    }
    *dst = new unsigned char[aligned_size_];
    if (*dst == 0)
    {
        return PACK_ERROR;
    }
    return get_content(owner, code, *dst, aligned_size_, content_size);
}

// 加密文件分支 @0x08ac41f6：ORIG 恒返 PACK_ERROR
pack_ret_t IndexEntry::get_contentEncryptedFile(PackSet*, hash_t,
                                                unsigned char*, unsigned int,
                                                unsigned int*)
{
    return PACK_ERROR;
}

// 内存整体装载 @0x08ac4378
pack_ret_t IndexEntry::update_content_from_memory(unsigned char* mem, PackSet* owner)
{
    if (mem == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1e0, __FUNCTION__);
        return PACK_ERROR;
    }
    if (type_ == ENTRY_FILE)
    {
        fprintf(stdout, "%s\n", "not implemented");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1e6, __FUNCTION__);
        return PACK_ERROR;
    }
    if (type_ == ENTRY_PACK)
    {
        content_ = new unsigned char[aligned_size_];
        memcpy(content_, mem + owner->header_size() + offset_, aligned_size_);
    }
    return PACK_OK;
}

// 从外部文件装载 @0x08ac4476；process 非空时走虚接口 loadFile
pack_ret_t IndexEntry::update_content_from_file(char const* name, PackSet* owner,
                                                IPackFileProcess* process)
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4476 size 0x37d）：本轮修 5 处——
    // ①content_ 4 字节整型清零（ORIG 单条 movl）；②分支序 size_==0/process!=0/else；
    // ③loadFile 首参为 **name**（ORIG +0x1af `mov 0xc(%ebp)`，真语义修正）；
    // ④删 ret 具名局部（谓词内联）；⑤crc32 首参直传 _hash 返回值（无 cast）+
    // 单表达式形复现 content_/aligned_size_ 的 %ebx/%esi 预载（push esi/ebx）。
    // p167 CLEAR、帧 0x130 与 ORIG 全等。
    char dir[260];
    _correct_filename(name, dir, 0x104, 0, 0);

    FILE* fp = fopen(dir, "rb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed %s", dir);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x1fb, __FUNCTION__);
        return PACK_ERROR;
    }

    type_ = ENTRY_FILE;
    name_ = dir;

    fseek(fp, 0, SEEK_END);
    long fpos = ftell(fp);
    if (fpos < 0)
    {
        fclose(fp);
        return PACK_ERROR;
    }

    size_ = static_cast<unsigned int>(fpos);
    fseek(fp, 0, SEEK_SET);
    aligned_size_ = (size_ + 3u) & ~3u;

    if (size_ == 0)
    {
        content_ = new unsigned char[4];
        // b28d-solo-PackSet（P171/P0）：ORIG `mov 0x18(%eax),%eax; movl $0,(%eax)`
        // = 单条 4 字节清零（单次 content_ 装载）⇒ 源为 4 字节整型写，
        // 逐字节 `content_[k]=0` 生 4×(重载+movb)。
        *reinterpret_cast<unsigned int*>(content_) = 0u;
    }
    // b28d-solo-PackSet（P214①/P272）：ORIG `cmpl $0,0x14(%ebp); je +0x1eb` ⇒
    // 分支序为 size_==0 / **process != 0** / else（process==0 走跳转臂）；
    // 旧形 `else if (process == 0)` 使 4.4 反产 `jne` 且两臂整体互换。
    else if (process != 0)
    {
        fclose(fp);
        fp = 0;
        // vtable slot2 → loadFile(name,&content_,&size_)；返回 1 视为失败。
        // b28d-solo-PackSet：ORIG `call *%edx; cmp $1,%eax; sete; test; je` 无 ret 槽
        // ⇒ 谓词内联（具名 ret 多占 4 字节槽使 dir/fp/fpos 整体深移 4）。
        // b28d-solo-PackSet 真语义修正：ORIG +0x1af 压入 `mov 0xc(%ebp),%eax`
        // （name 形参）而非规范化后的 dir ⇒ 原码传 name。
        if (process->loadFile(name, &content_, &size_) == PACK_ERROR)
        {
            return PACK_ERROR;
        }
        aligned_size_ = (size_ + 3u) & ~3u;
    }
    else
    {
        content_ = new unsigned char[aligned_size_];
        memset(content_ + size_, 0, aligned_size_ - size_);
        if (fread(content_, size_, 1, fp) != 1)
        {
            fprintf(stdout, "fread failed %s", dir);
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x226, __FUNCTION__);
            fclose(fp);
            return PACK_ERROR;
        }
    }

    if (fp != 0)
    {
        fclose(fp);
    }

    // ORIG 无 hash 具名局部（`call _hash; mov %eax,(%esp)` 直传）+ 第 1 实参无 cast；
    // 且 content_/aligned_size_ 在 toMbcs 之前预载 %ebx/%esi 跨两 call 保活 ⇒ 单表达式。
    crc_ = Crc32::crc32(_hash(toMbcs(dir)),
                        reinterpret_cast<char const*>(content_), aligned_size_);
    if (_encrypt(crc_, content_, aligned_size_) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x24e, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

// ---------------------------------------------------------------------------
// IPackFileProcess（ORIG 0x08ac768c..0x08ac776x）
// ---------------------------------------------------------------------------
IPackFileProcess::IPackFileProcess()
{
    s_ref_count_ += 1;                       // ORIG +0x19 处 ref++
}

IPackFileProcess::~IPackFileProcess()       // D1 @0x08ac76a8（D2 别名）
{
    s_ref_count_ -= 1;
    if (s_ref_count_ < 1)
    {
        if (s_common_buffer_ != 0 && s_common_buffer_ != 0)
        {
            operator delete[]((void*)s_common_buffer_);   // 4.4 delete[] 自带判空，显式调用保 ORIG 双 test 形
        }
        s_common_buffer_ = 0;
        s_common_buffer_size_ = 0;
    }
}

unsigned char* IPackFileProcess::getCommonBuffer()              // @0x08ac7734
{
    return s_common_buffer_;
}

unsigned char* IPackFileProcess::getCommonBuffer(unsigned int needSize)  // @0x08ac773e
{
    if (!(needSize <= s_common_buffer_size_))   // ORIG cmp %eax,mem; jbe 方向
    {
        if (s_common_buffer_ != 0 && s_common_buffer_ != 0)
        {
            operator delete[]((void*)s_common_buffer_);
        }
        s_common_buffer_ = new unsigned char[needSize];
        s_common_buffer_size_ = (s_common_buffer_ != 0) ? needSize : 0;   // ORIG 三元 je/两臂形
    }
    return s_common_buffer_;
}

// ---------------------------------------------------------------------------
// PackSet（ORIG 0x08ac47f4..0x08ac76xx）
// ---------------------------------------------------------------------------

PackSet::PackSet()      // C1/C2 @0x08ac47f4：ctor vectors 与 clear()
    : m_indexes(), m_addeds()
{
    // 对齐 ORIG：构造两个空 vector 后调 clear() 重置全部字段
    clear();
}

PackSet::~PackSet()     // D1 @0x08ac4862：close() 后析构两个 vector
{
    close();
}

void PackSet::clear()   // @0x08ac48d2
{
    m_packFile = 0;
    m_revision = 0;
    m_headerSize = 0;
    m_type = PACK_LOAD_FULL;
    m_indexes.clear();
    m_addeds.clear();
    m_bUseEncryptedFile = false;
}

void PackSet::close()   // @0x08ac49da
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac49da size 0x18b）：
    // ORIG else 分支开头即 `lea -0x14; call iterator::C1Ev`（fclose 检查**之前**），
    // 两个循环各自 `it = …begin()` 复用同一槽（-0x14）⇒ 原码是**函数级**
    // `index_vector_it it;` 声明 + for 内赋值（P28/P50/P212）；旧形两个 for 各在
    // init 处直构，既缺 C1Ev 又使 begin sret/end 临时槽整体漂移。
    if (UsingEncryptedFile())
    {
        closeEncryptedFile();
        return;
    }

    index_vector_it it;
    if (m_packFile != 0)
    {
        fclose(m_packFile);
    }

    // 释放两份索引中的内容缓冲（operator delete[]）
    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
    for (it = m_addeds.begin(); it != m_addeds.end(); ++it)
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
    clear();
}

PackSet::index_vector_it PackSet::find(char const* name)    // sret @0x08ac4e00
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4e00 size 0x22b）：
    // ①ORIG `lea -0x64; call pair<it,it>::C1Ev` 后 `aSEOS9_`（右值赋值）⇒ er
    //   先默认构造、equal_range 结果经临时槽赋回（P28/P50 两段式）；旧形
    //   `er = equal_range(...)` 初始化式被 4.4 折叠，缺 C1Ev 与临时槽。
    // ②ORIG 无 code 预清（直接 `call _hash; mov %eax,-0x40`）⇒ 原码 `hash_t code;`
    //   无初始化（P171 照抄不写）。
    hash_t code;
    IndexEntry dummy;
    std::pair<index_vector_it, index_vector_it> er;
    char file[260];

    _correct_filename(name, file, 0x104, 0, 0);
    code = _hash(toMbcs(file));

    er = std::equal_range(m_indexes.begin(), m_indexes.end(),
                          index_pair_t(code, dummy), &_index_less);

    // b28d-solo-PackSet（P167 判据）：ORIG 第一个循环调 ptEv（`it->`）而非 deEv
    // ⇒ 原码用 `->` 访问（`(*it).m` 会多实例化 operator* 且 p167 EXTRA deEv）。
    if (er.first == m_indexes.end() || er.first->first != code)
    {
        return m_indexes.end();
    }

    // b28d-solo-PackSet：ORIG +0x181 有独立 `iterator::C1Ev` 后 `it2 = er.first`
    // ⇒ 原码先声明后赋值（P50/P28 两段式）；初始化式形被 4.4 折叠。
    index_vector_it it2;
    it2 = er.first;
    // b28d-solo-PackSet（P283）：ORIG init 后 `jmp` 到底部条件块（cond 在 inc 之后），
    // 4.4 对 `for(;;)` 恒把 cond 置顶 ⇒ 用 goto 尾标签复现。
    goto find_cond;
find_body:
    if (strcmp(it2->second.name_.c_str(), file) == 0)
    {
        return it2;
    }
    ++it2;
find_cond:
    if (it2->first == code && it2 != er.second)
    {
        goto find_body;
    }
    return m_indexes.end();
}

PackSet::index_vector_const_it PackSet::find_const(char const* name)   // sret @0x08ac4b6c
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4b6c size 0x291）：
    // 与 0x08ac4e00 同族（const_iterator 版）：①声明序 code,dummy,er,file 且 code
    // 无预清；②er 先 C1Ev 再 `aSEOS9_` 赋值；③`it->`（ptEv）非 `(*it)`；④it2
    // 两段式（C1Ev + 赋值）；⑤P283 goto 尾标签复现底部条件块。
    // ORIG 局部一律**非 const** iterator（er/it2），仅返回处隐式转换为
    // const_iterator（ORIG 0x08ac4d43/0x08ac4dbb 两处 call
    // `__normal_iterator<const pair*>::C1<__normal_iterator<pair*>>` 转换构造，
    // nm 实证 @0x08ac7ce4）⇒ P96/P270 非 const 形；旧形全 const 使 4.4 直接位拷贝。
    hash_t code;
    IndexEntry dummy;
    std::pair<index_vector_it, index_vector_it> er;
    char file[260];

    _correct_filename(name, file, 0x104, 0, 0);
    code = _hash(toMbcs(file));

    er = std::equal_range(m_indexes.begin(), m_indexes.end(),
                          index_pair_t(code, dummy), &_index_less);

    if (er.first == m_indexes.end() || er.first->first != code)
    {
        return m_indexes.end();
    }

    index_vector_it it2;
    it2 = er.first;
    goto find_const_cond;
find_const_body:
    if (strcmp(it2->second.name_.c_str(), file) == 0)
    {
        return it2;
    }
    ++it2;
find_const_cond:
    if (it2->first == code && it2 != er.second)
    {
        goto find_const_body;
    }
    return m_indexes.end();
}

bool PackSet::is_exist(char const* name)     // @0x08ac498c
{
    // b28c-L8：ORIG 0x08ac498c 先 find(sret -0x10) 后 end(临时 -0xc)，
    // 即左操作数具名结果再比较；直接写二进制表达式 4.4 反序求值。
    index_vector_it rval = find(name);
    return rval != m_indexes.end();
}

bool PackSet::is_ready() const               // @0x08ac4954
{
    return m_indexes.size() != 0;
}

bool PackSet::is_dirty() const               // @0x08ac4970
{
    return m_addeds.size() != 0;
}

PackSet::index_vector_t& PackSet::indexes()                    // 0x08ac4948
{
    return m_indexes;
}

PackSet::index_vector_t const& PackSet::indexes() const        // 0x08ac493c
{
    return m_indexes;
}

unsigned int PackSet::revision()             // 0x08ac4924
{
    return m_revision;
}

unsigned int PackSet::header_size()          // 0x08ac4930
{
    return m_headerSize;
}

pack_ret_t PackSet::get(char const* name, unsigned char* buffer,
                        unsigned int buffer_size, unsigned int* content_size)   // 0x08ac5060
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac5060 size 0x168）：
    // 同 0x08ac51ce 重载：①`index_vector_it it;` 必须在三参守卫之前（ORIG 序言后
    // 立即 call iterator::C1Ev，it@-0x10）；②code 无槽往返（ORIG `mov (%eax),%ebx`
    // + push/pop %ebx）⇒ 裸 register 存储类提示（P231/P248，非 asm 绑定）。
    index_vector_it it;
    if (name == 0 || buffer == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x312, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x319, __FUNCTION__);
        return PACK_ERROR;
    }

    // P167：ORIG 调 `…ptEv`（it->），`(*it)` 形额外实例化 deEv。
    register hash_t code = it->first;
    if (it->second.get_content(this, code, buffer, buffer_size, content_size) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 799, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

pack_ret_t PackSet::get(char const* name, unsigned char** content,
                        unsigned int* content_size)                             // 0x08ac51ce
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac51ce size 0x163）：
    // ①ORIG 序言后**立即** `lea -0x10; call iterator::C1Ev`（默认构造）再做三参
    //   守卫 ⇒ 原码 `index_vector_it it;` 声明在参数检查之前（P28/P50 两段式）；
    //   旧形在守卫后才声明，it 槽漂到 -0x14、end 临时 -0x10（全帧错位）。
    // ②ORIG `mov (%eax),%ebx` + 尾 `mov -0x4(%ebp),%ebx`（push/pop %ebx）⇒ code
    //   无栈槽往返，跨第二次 operator-> 与 get_content 保活（P0/P231/P248 三条件：
    //   跨 callee call 保活 + ORIG 目标为 callee-saved %ebx）⇒ 裸 register
    //   存储类提示（非 asm 绑定，P0 合规）。
    index_vector_it it;
    if (name == 0 || content == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x32c, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x333, __FUNCTION__);
        return PACK_ERROR;
    }

    register hash_t code = it->first;
    if (it->second.get_content(this, code, content, content_size) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x339, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

pack_ret_t PackSet::added(char const* name, IPackFileProcess* process)   // 0x08ac56a2
{
    // b28c-L8：ORIG 函数头唯一的前置 ctor call 是 index_pair_t entry（sret
    // -0x30），迭代器 it 无默认 ctor（find 的 sret 直落其槽 -0x10）⇒ entry
    // 声明提到函数头、it 保持直接初始化（P28/P212 声明序=槽深序）。
    index_pair_t entry;
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x38a, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
    // b28d-36（P74② 实参序旋钮）：ORIG 0x08ac57a0 段 ne() 首参=&end(-0xc)、
    // 次参=&it(-0x10) ⇒ 原码 end() 在比较式**左侧**；写 `it != end()` 时 4.4
    // 反压。槽位不变，仅 0x4(%esp)/(%esp) 互换。
    if (m_indexes.end() != it)
    {
        fprintf(stdout, "find %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x390, __FUNCTION__);
        return PACK_ERROR;
    }

    // b28c-L8：ORIG 0x08ac56a2 无该 store —— pair<int,int> 的默认 ctor（函数头
    // 唯一前置 call，_ZNSt4pairIiiEC1Ev）已把 first 值初始化为 0，原源码不重复写。
    if (entry.second.update_content_from_file(name, this, process) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x396, __FUNCTION__);
        return PACK_ERROR;
    }
    entry.first = _hash(toMbcs(entry.second.name_.c_str()));
    m_addeds.push_back(entry);
    return PACK_OK;
}

pack_ret_t PackSet::modified(char const* name, IPackFileProcess* process)   // 0x08ac54ee
{
    index_vector_it it;   // b28c-L8：ORIG 默认 ctor 前置（P28 两段式）
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x369, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "find failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x370, __FUNCTION__);
        return PACK_ERROR;
    }

    IndexEntry& entry = it->second;
    if (entry.content_ != 0)
    {
        delete[] entry.content_;
        // b28c-L8 真语义 bug 修正：ORIG 0x08ac54ee 此处 store 目标为 +0x18，
        // 而 IndexEntry::offset_ 经权威 getter _ZNK10IndexEntry6offsetEv
        // @0x08ac3e94 = `mov 0x14(%eax),%eax` 实证在 +0x14，+0x18 是 content_
        // （_ZNK10IndexEntry7contentEv @0x08ac3ea0 = `mov 0x18(%eax),%eax`）。
        // 旧形清 offset_ 会留下已释放的 content_ 野指针 ⇒ 二次 delete[] 风险。
        entry.content_ = 0;
    }

    if (entry.update_content_from_file(name, this, process) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x37d, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

pack_ret_t PackSet::deleted(char const* name)   // 0x08ac5332
{
    index_vector_it it;   // b28c-L8：ORIG 默认 ctor 前置（P28 两段式）
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x346, __FUNCTION__);
        return PACK_ERROR;
    }

    it = find(name);
    if (it != m_indexes.end())
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
        }
        m_indexes.erase(it);
    }
    return PACK_OK;
}

void PackSet::freeContent(char const* name)     // 0x08ac5448
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac5448 size 0xa5）：b28c-L8 已修
    // （函数级 iterator 默认构造 + 三处 `(*it)` deEv + 嵌套 if bool 物化）；
    // 本轮 fresh 复验 near=0 且不在 DIFF 清单 ⇒ AE 保持。
    index_vector_it it;   // b28c-L8：ORIG 默认 ctor 前置（P28 两段式）
    it = find(name);
    // b28d-solo-PackSet（P167 判据，修正 b28c-L8 误判）：ORIG 0x08ac5448 三处
    // 成员访问经 `operator->`（nm 实证 0x08ac7a24 = _ZNK…ptEv），**非** deEv；
    // 旧形 `(*it).second` 使 p167 出现 3×MISSING ptEv / 3×EXTRA deEv。
    // 第二条件是独立嵌套 if（ORIG `test; setne %al; test %al,%al; je` bool
    // 物化形）——短路 && 合并形使 4.4 反产 `mov$1;jmp / mov$0` 岛（+3 条）。
    if (it != m_indexes.end())
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
}

pack_ret_t PackSet::sort()      // 0x08ac58ee
{
    index_vector_it it;   // b28c-L8：ORIG 0x08ac58ee 函数头默认 ctor，循环内赋值
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3b7, __FUNCTION__);
        return PACK_ERROR;
    }

    m_indexes.reserve(m_addeds.size());
    for (it = m_addeds.begin(); it != m_addeds.end(); ++it)
    {
        m_indexes.push_back(*it);
    }
    std::sort(m_indexes.begin(), m_indexes.end(), &_index_less);
    m_addeds.clear();
    return PACK_OK;
}

pack_ret_t PackSet::open(char const* name, char const* tag, pack_load_type_t type)   // 0x08ac5a52
{
    if (UsingEncryptedFile())
    {
        return openEncryptedFile(name, tag, type);
    }

    // b28d-solo-PackSet 判词（N 封顶，ORIG 0x08ac5a52 size 0x69d）：
    // ORIG 在 is_ready() 之前先 `lea -0x50; call iterator::C1Ev`（函数级迭代器）
    // 再 `lea -0x64; call tstring::C1Ev` ⇒ 两个局部都在函数头、构造序 it→index_tag
    // （P28/P50/P212）；旧形只在 for 内直构 it、index_tag 构造被 4.4 后置。
    index_vector_it it;
    tstring index_tag;

    if (is_ready())
    {
        fprintf(stdout, "%s\n", "Already opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3d8, __FUNCTION__);
        return PACK_ERROR;
    }

    FILE* fp = fopen(name, "rb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3e1, __FUNCTION__);
        return PACK_ERROR;
    }

    _fread_str(index_tag, fp);
    if (tag != 0)
    {
        if (strcmp(index_tag.c_str(), tag) != 0)
        {
            fclose(fp);
            fprintf(stdout, "tag mismatch. %s", index_tag.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3ec, __FUNCTION__);
            return PACK_ERROR;
        }
    }

    unsigned int idxHdrSize = 0;    // -0x54：索引头区大小
    hash_t idxHdrCrc = 0;           // -0x58：头 CRC（checksum/key）
    hash_t idxHdrKey = 0;           // -0x5c：解密种子兼条目数

    fread(&m_revision, 4, 1, fp);
    fread(&idxHdrSize, 4, 1, fp);
    fread(&idxHdrCrc, 4, 1, fp);
    fread(&idxHdrKey, 4, 1, fp);

    unsigned char* idxHdrData = new unsigned char[idxHdrSize];   // -0x1c
    fread(idxHdrData, idxHdrSize, 1, fp);

    m_headerSize = ftell(fp);       // 数据区起点 = 头之后偏移
    m_packFile = fp;

    if (_decrypt_N_crc32(idxHdrKey, idxHdrCrc, idxHdrCrc, idxHdrData, idxHdrSize) == 0)
    {
        delete[] idxHdrData;
        return PACK_ERROR;
    }

    m_indexes.resize(static_cast<std::size_t>(idxHdrKey), index_pair_t());

    unsigned char* cur = idxHdrData;
    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        it->first = *reinterpret_cast<unsigned long*>(cur);
        cur += 4;
        it->second.read_info(cur);
    }
    delete[] idxHdrData;

    if (type == PACK_LOAD_FULL)
    {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        unsigned char* whole = new unsigned char[fileSize];
        fread(whole, fileSize, 1, fp);
        for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
        {
            it->second.update_content_from_memory(whole, this);
        }
        delete[] whole;
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        if (fileSize > 0)
        {
            fseek(fp, 0, SEEK_SET);
            unsigned char* whole = new unsigned char[fileSize];
            fread(whole, fileSize, 1, fp);
            for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
            {
                // b28d-solo-PackSet（p167）：ORIG 调 `IndexEntry::name()` getter
                // （0x08ac5ffa/0x08ac6010 两处 call _ZNK10IndexEntry4nameEv）
                // 而非直访 name_ 成员。
                it->second.name();
                if (it->second.name().find(".ani", 0) != tstring::npos)
                {
                    it->second.update_content_from_memory(whole, this);
                }
            }
            delete[] whole;
        }
    }

    m_type = type;
    return PACK_OK;
}

pack_ret_t PackSet::verify()    // 0x08ac6c8a
{
    if (UsingEncryptedFile())
    {
        return verifyEncryptedFile();
    }

    unsigned int bufferSize = 0x500000u;
    unsigned char* buff_file = 0;
    unsigned char* buff_pack = 0;
    unsigned int contentSize = 0;
    // b28d-solo-PackSet 判词（N 封顶，ORIG 0x08ac6c8a size 0x973）：
    // ①ORIG 序言后 4 个标量清零 → `lea -0x70; call iterator::C1Ev` → is_ready
    //   ⇒ 迭代器为函数级声明（p167 MISSING 该 C1Ev；两循环复用同一槽）。
    // ②ORIG 在 new[] 之后才清零 proc_count/total_count/proc_size/total_size，
    //   再构造两个 Tick，最后清零 run_time/avg_time/remain_time ⇒ 声明序照抄。
    index_vector_it it;

    if (!is_ready())
    {
        fprintf(stdout, "%s\n", "Not yet opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5a5, __FUNCTION__);
        return PACK_ERROR;
    }

    buff_file = new unsigned char[bufferSize];
    buff_pack = new unsigned char[bufferSize];

    size_t proc_count = 0;
    size_t total_count = 0;
    unsigned int proc_size = 0;
    unsigned int total_size = 0;
    Tick start_time;                   // ORIG：先默认构造再赋值（TickC1+now+aSES）
    Tick frame_start;                  // 最后一次刷新时刻
    int run_time = 0;
    float avg_time = 0.0f;
    int remain_time = 0;

    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        total_size += it->second.size_;
    }
    total_count = m_indexes.size();

    start_time = Tick::now();          // 进度更新基准
    frame_start = Tick::now();

    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        if (bufferSize < it->second.aligned_size_)
        {
            delete[] buff_file;
            buff_file = 0;
            delete[] buff_pack;
            buff_pack = 0;
            bufferSize = it->second.aligned_size_;
            buff_file = new unsigned char[bufferSize];
            buff_pack = new unsigned char[bufferSize];
        }

        *buff_file = 0;
        *buff_pack = '\0';

        hash_t code = it->first;
        if (it->second.get_content(this, code, buff_pack, bufferSize, &contentSize) == PACK_ERROR)
        {
            fprintf(stdout, "get content. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5e0, __FUNCTION__);
            return PACK_ERROR;      // ORIG：此处不释放缓冲
        }

        FILE* fp = fopen(it->second.name_.c_str(), "rb");
        if (fp == 0)
        {
            fprintf(stdout, "fopen failed. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5e7, __FUNCTION__);
            return PACK_ERROR;
        }
        fseek(fp, 0, SEEK_END);
        std::size_t size_file = static_cast<std::size_t>(ftell(fp));
        fseek(fp, 0, SEEK_SET);
        fread(buff_file, size_file, 1, fp);
        fclose(fp);

        if (size_file != contentSize)
        {
            fprintf(stdout, "size is different. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5f4, __FUNCTION__);
            return PACK_ERROR;
        }

        if (memcmp(buff_file, buff_pack, contentSize) != 0)
        {
            fprintf(stdout, "contents is different. %s", it->second.name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5fa, __FUNCTION__);
            return PACK_ERROR;
        }

        proc_size += it->second.size_;
        proc_count += 1;

        int since_refresh = Tick::now() - frame_start;
        if (1000 < since_refresh)
        {
            frame_start = Tick::now();
            run_time = Tick::now() - start_time;
            avg_time = static_cast<float>(run_time) / static_cast<float>(proc_size);
            remain_time = static_cast<int>((static_cast<float>(total_size - proc_size) * avg_time));
            // ORIG ROUND(100.0f*(proc_size/total_size))：ROUND 宏按四舍五入推断
            printf("%3d%% (%2dmin %2dsec)/(%2dmin %2dsec) (%d/%d)    \n",
                   static_cast<int>((100.0f * (static_cast<float>(proc_size)
                                                / static_cast<float>(total_size))) + 0.5f),
                   run_time / 60000, (run_time % 60000) / 1000,
                   remain_time / 60000, (remain_time % 60000) / 1000,
                   static_cast<int>(proc_count), static_cast<int>(total_count));
        }
    }

    run_time = Tick::now() - start_time;
    avg_time = static_cast<float>(run_time) / static_cast<float>(proc_size);
    remain_time = static_cast<int>((static_cast<float>(total_size - proc_size) * avg_time));
    printf("%3d%% (%2dmin %2dsec)/(%2dmin %2dsec) (%d/%d)    \n",
           static_cast<int>((100.0f * (static_cast<float>(proc_size)
                                        / static_cast<float>(total_size))) + 0.5f),
           run_time / 60000, (run_time % 60000) / 1000,
           remain_time / 60000, (remain_time % 60000) / 1000,
           static_cast<int>(proc_count), static_cast<int>(total_count));

    delete[] buff_file;
    delete[] buff_pack;
    return PACK_OK;
}

pack_ret_t PackSet::output_file_list(char const* pszOutputPath) const    // 0x08ac6988
{
    if (!is_ready())
    {
        fprintf(stdout, "%s\n", "Not yet opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x573, __FUNCTION__);
        return PACK_ERROR;
    }

    FILE* fp = fopen(toMbcs(pszOutputPath), "wt");
    if (fp == 0)
    {
        fprintf(stdout, "%s\n", "Failed to output file open.");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x57a, __FUNCTION__);
        return PACK_ERROR;
    }

    fwrite("hash,type,name,size,aligned size,crc,offset\n", 1, 0x2c, fp);

    for (index_vector_t::const_iterator it = m_indexes.begin();
         it != m_indexes.end(); ++it)
    {
        fprintf(fp, "\"%d\"", static_cast<int>(it->first));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.type_));
        fputc(',', fp);
        fprintf(fp, "\"%s\"", toMbcs(it->second.name_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.size_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.aligned_size_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.crc_));
        fputc(',', fp);
        fprintf(fp, "\"%d\"", static_cast<int>(it->second.offset_));
        fputc('\n', fp);
    }
    fclose(fp);
    return PACK_OK;
}

pack_ret_t PackSet::write(char const* name, char const* tag, unsigned int rev)   // 0x08ac60fa
{
    // b28d-solo-PackSet 判词（N 封顶，ORIG 0x08ac60fa size 0x88d）：p167 判据——
    // ORIG 缺两个函数级迭代器默认构造（pair<ulong,IndexEntry> 与
    // pair<ulong,IndexEntry*> 的 `__normal_iterator::C1Ev`）⇒ 原码在函数头声明
    // `it`/`it2`、循环内赋值（P28/P50 两段式）。
    FILE* fp = 0;
    index_vector_it it;
    index_vector2_it it2;
    index_vector2_t writeIdx;
    unsigned char* headerBuf = 0;

    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4e8, __FUNCTION__);
        return PACK_ERROR;
    }

    if (is_dirty())
    {
        if (sort() == PACK_ERROR)
        {
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4f0, __FUNCTION__);
            return PACK_ERROR;
        }
    }

    {
        char dir[260];
        char file[260];
        for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
        {
            index_pair2_t tmp;
            tmp.first = 0;
            tmp.second = 0;
            if (_correct_filename(it->second.name_.c_str(), file, 0x104,
                                  dir, 0x104) == PACK_ERROR)
            {
                fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4ff, __FUNCTION__);
                return PACK_ERROR;      // writeIdx 由栈展开析构
            }
            tmp.first = _write_hash(file, dir);
            tmp.second = &(it->second);
            writeIdx.push_back(tmp);
        }
    }
    std::sort(writeIdx.begin(), writeIdx.end(), &_index_less2);

    unsigned int accumOffset = 0;
    for (it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
    {
        it2->second->offset_ = accumOffset;
        accumOffset += it2->second->aligned_size_;
    }

    fp = fopen(TEMP_HEADER_FILENAME, "wb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", TEMP_HEADER_FILENAME);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x514, __FUNCTION__);
        return PACK_ERROR;
    }

    for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        fwrite(&(it->first), 4, 1, fp);
        if (it->second.write_info(fp) == PACK_ERROR)
        {
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x51c, __FUNCTION__);
            fclose(fp);
            return PACK_ERROR;
        }
    }
    // ORIG 循环结束后不 fclose（直接重开同名文件，实证 0x08ac654f fopen）

    fp = fopen(TEMP_HEADER_FILENAME, "rb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x526, __FUNCTION__);
        return PACK_ERROR;
    }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    if (sz < 0)
    {
        // ORIG sz<0 不 fclose，直接返回（实证 0x08ac65ef）
        return PACK_ERROR;
    }
    fseek(fp, 0, SEEK_SET);

    unsigned int hdrDataSz = static_cast<unsigned int>(sz + 3) & ~3u;
    headerBuf = new unsigned char[hdrDataSz];
    memset(headerBuf, 0, hdrDataSz);
    fread(headerBuf, sz, 1, fp);
    fclose(fp);

    unsigned int entryCount = static_cast<unsigned int>(m_indexes.size());
    unsigned int hdrCrc = Crc32::crc32(entryCount,
                                       reinterpret_cast<char const*>(headerBuf), hdrDataSz);
    _encrypt(hdrCrc, headerBuf, hdrDataSz);

    fp = fopen(name, "wb");
    if (fp == 0)
    {
        delete[] headerBuf;
        fprintf(stdout, "_tfopen failed. %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x551, __FUNCTION__);
        return PACK_ERROR;
    }

    _fwrite_str(tag, fp);
    fwrite(&rev, 4, 1, fp);
    fwrite(&hdrDataSz, 4, 1, fp);
    fwrite(&hdrCrc, 4, 1, fp);
    fwrite(&entryCount, 4, 1, fp);
    fwrite(headerBuf, hdrDataSz, 1, fp);

    for (it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
    {
        if (it2->second->content_ == 0)
        {
            fprintf(stdout, "content is NULL. %s", it2->second->name_.c_str());
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x560, __FUNCTION__);
            fclose(fp);
            delete[] headerBuf;
            return PACK_ERROR;
        }
        fwrite(it2->second->content_, it2->second->aligned_size_, 1, fp);
    }
    fclose(fp);
    // ORIG 成功路径不回收 headerBuf（透明泄漏），忠实保留。
    return PACK_OK;
}

FILE* PackSet::pack_file()     // 0x08ac7608
{
    if (!m_bUseEncryptedFile)
    {
        return m_packFile;
    }
    fprintf(stdout, "%s\n", "_C_DNF_SCRIPT_CRYPTER_ is turned on!!");
    fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x677, __FUNCTION__);
    return m_packFile;
}

bool PackSet::UsingEncryptedFile()      // 0x08ac7674
{
    return m_bUseEncryptedFile;
}

void PackSet::UseEncryptedFile()        // 0x08ac7680
{
    m_bUseEncryptedFile = true;
}

pack_ret_t PackSet::openEncryptedFile(char const*, char const*, pack_load_type_t)   // 0x08ac60f0
{
    return PACK_ERROR;
}

void PackSet::closeEncryptedFile()      // 0x08ac4b66：ORIG 空体
{
}

pack_ret_t PackSet::verifyEncryptedFile()   // 0x08ac75fe：ORIG 恒返 PACK_ERROR
{
    return PACK_ERROR;
}
