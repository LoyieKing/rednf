// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PackSet/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "PackSet/crc32.inc"
{
    // b28d-solo-PackSet 判词（AE / ndiff=0，ORIG 0x08ad34ae size 0x55）：①body
    // 的 `crc>>8` 求值序由 `(crc>>8) ^ table[...]` 书写序翻正（P243）；②cond 段
    // `cmp;setne;subl $1,len;test;jne`（旧值先物化、后置递减）由
    // `while (len-- != 0)` 复现——前轮「N82 不可翻」判词作废：for/while/前置三形
    // 确不可翻，**后置递减写进条件**才是唯一解。
    if (crc_table[255] == 0)
    {
        make_crc_table();
    }
    crc = ~crc;
    // b28d-solo-PackSet（P243 同族）：ORIG body 首两条即 `mov crc,%eax; mov %eax,%edx;
    // shr $8,%edx`（右操作数 `crc>>8` 先求值）⇒ 源把 `crc >> 8` 写在 `^` 左侧。
    // b28d-solo-PackSet（P225 变体）：ORIG cond 段为 `cmp; setne %al; subl $1,len;
    // test %al,%al; jne`（旧值先物化、后置递减）⇒ `len--` 写进条件。
    while (len-- != 0)
    {
        crc = (crc >> 8) ^ crc_table[(static_cast<unsigned char>(*buf) ^ crc) & 0xffu];
        ++buf;
    }
    return ~crc;
}

#include "PackSet/getCrc32Table.inc"
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
    // b28d-solo-PackSet 判词（N 封顶，ORIG 0x08ac398b size 0x150；本轮 ndiff
    // 54→52）：e 的 `shr $2; shl $2` 本轮改字节指针形复现、rotate 单表达式、
    // `(x>>8)^table` 求值序、crc 自身累积（无 tmp 槽）、尾部 `if/return 0` 极性
    // 均已对齐；残余 = 帧 ORIG `sub $0x18` vs 4.4 `sub $0x10`（0x18 非 16k 量化
    // 值；P190② 4.4 带 call 帧恒 16k+4 量化，dead 局部/double pad 两形实测不可
    // 凑）⇒ 槽位整体浅 8，语义等价封顶。
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
    // b28d-solo-PackSet：ORIG 0x08ac39c7 为 `shr $2; shl $2; add p`——`(size>>2)*4`
    // 是**字节**偏移直接加到字节地址；写成 `p + ((size>>2)*4)` 时 4.4 会把
    // 指针缩放的 *4 一并折成 `shl $4`（元素偏移）⇒ 用字节指针形式复现。
    unsigned int* e = reinterpret_cast<unsigned int*>(
        reinterpret_cast<unsigned char*>(p) + ((size >> 2) * 4));
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
    // b28d-solo-PackSet（P47）：ORIG 尾为 `notl; cmp; je <mov $1>` 形。
    if (~crc == checksum)
    {
        return 1;
    }
    return 0;
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
#include "PackSet/_correct_filename.inc"
                      char* dir, unsigned int dir_size)
{
    if (src == 0 || dst == 0)
    {
        return 1;
    }

    // b28d-solo-PackSet 判词（N 封顶：跨版本循环体/exit 头块序，ORIG 0x08ac3b9a
    // size 0x247；本轮 ndiff 164→29、gap -10→+1）：
    // ①p167：ORIG 全部 stack 调用为**非 const** `std::stack<char*>`（push 左值、
    //   top 返 char*）⇒ 16=16 CLEAR。
    // ②全槽位对齐：prev_tok -0x18 / w -0x44 / seg -0x14 / r -0x10 / c -0x9 /
    //   deque 临时 -0x40 / stack -0x6c。关键两处：**seg 声明先于 r**（两指针槽
    //   互换归位）；**尾部复用 seg 而非独立 tail 局部**（4.4 不复用两者槽 ⇒
    //   全帧深移 4）。
    // ③`while ((c = *r++) != 0)` 复现底部 cond 块 + `setne %al; addl $1,r;
    //   test %al,%al; jne` 物化（P225/P283；goto 尾标签形无 setne）。
    // ④`*(unsigned short*)seg == 0x2e2e` 复现 ORIG `movzwl; cmp $0x2e2e,%ax`
    //   （`seg[0]=='.'&&seg[1]=='.'` 在 4.4 生两段 movzbl+cmp）。
    // ⑤dir 单出口 `if (dir != 0) {…} return 0;` 复现 `cmpl $0,dir; je <共享
    //   return 块>`（P47）。
    // ⑥恒真守卫 `if (c != 0) { 结算 }` 复现 exit 头 `cmpb $0,c; jne +0x110`
    //   （break 出口与分隔符路径共用该检查）。
    // 残余（语义等价，N 封顶）：4.1 把结算段物理置于 cond/exit 头**之后**
    // （+0x110）；4.4 恒把结算段发射在 cond 之前且守卫生成 `je` 负极性 ⇒ 差
    // 1 条 jne + 1 条 jmp（ndiff 29 全为块序对齐残差，指令集只差这对跳转）。
    int prev_tok = 0;                // -0x18（ORIG dead 局部，Ghidra local_1c 无读）
    (void)prev_tok;
    char* w = dst;                   // -0x44
    char* seg;                       // -0x14
    char const* r = src;             // -0x10
    char c;                          // -0x9
    std::stack<char*> st;            // 分量起点栈（ORIG stack<char*, deque<char*>>）
    st.push(w);                      // 初始哨兵（ORIG push 实参 = w 局部 &-0x44，非形参 dst）

    // b28d-solo-PackSet（P283/P184）：ORIG 循环条件块在**物理底部**（+0xd9 读 c/
    // 物化 setne/`++r`/test/jne 回体），入口 +0x88 无条件 `jmp +0xd9`，且体首
    // `cmpb $0,c; je +0xf3` 为 4.1 保留的死块 ⇒ 用 while(赋值表达式) 形复现
    // （4.4 对 `for(;;)`+break 恒把条件置顶）。
    while ((c = *r++) != 0)
    {
        if (c == 0)
        {
            break;                    // ORIG 死块（4.1 保留）
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
        // b28d-solo-PackSet：ORIG exit 头 +0xf4 有 `cmpb $0,c; jne +0x110`
        // （break 出口与分隔符路径共用该检查；c 恒非 0）⇒ 显式写出恒真守卫，
        // 复现该 cmpb/jne 对（4.4 对无守卫形直接内联结算段，少 2 条）。
        if (c != 0)
        {
            seg = st.top();
            // b28d-solo-PackSet（P0/P183）：ORIG 无 diff 栈槽，三处 if 各自重算
            // `w - seg`（`mov w,%eax; mov %eax,%edx; mov seg,%eax; mov %edx,%ecx;
            // sub %eax,%ecx`）；具名 diff 会被 4.4 合并成单次计算 + 槽往返。
            if (static_cast<unsigned long>(w - seg) == 1)
            {
                w = seg;                             // 段为空
            }
            else if (static_cast<unsigned long>(w - seg) == 2 && seg[0] == '.')
            {
                w = seg;                             // 抛弃 "./"
            }
            // b28d-solo-PackSet：ORIG +0x176 `movzwl (%eax),%eax; cmp $0x2e2e,%ax`
            // ⇒ 原码是 16 位双字节比较（`seg[0]=='.'&&seg[1]=='.'` 在 4.4 生两段
            // movzbl+cmp）。
            else if (static_cast<unsigned long>(w - seg) == 3 &&
                     *reinterpret_cast<unsigned short const*>(seg) == 0x2e2eu)
            {
                if (st.empty())
                {
                    return 1;                        // 空栈分支实际不可达，忠实保留
                }
                st.pop();
                w = st.top();
            }
            else
            {
                st.push(w);
            }
        }
    }

    *w = '\0';

    // b28d-solo-PackSet（P0/P47）：ORIG `cmpl $0,dir; je +0x212`（跳到共享
    // return 块）⇒ 单出口 `if (dir != 0) {…} return 0;`；ORIG 尾部复用 seg 槽
    // -0x14（无独立 tail 局部），避免多占 4 字节使全帧深移。
    if (dir != 0)
    {
        seg = st.top();
        ss_strcpy(dir, dir_size, seg);
        if (seg == dst)
        {
            seg[0] = '\0';
        }
        else
        {
            seg[-1] = '\0';                         // 去尾段（ORIG sptr[-1]=0）
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// IndexEntry（ORIG 0x08ac3de2..0x08ac5b4x）
// ---------------------------------------------------------------------------
#include "PackSet/IndexEntry.inc"
{
    clear();
}

// ORIG W @0x08ac7a50：隐式逐成员拷贝
#include "PackSet/IndexEntry_1.inc"
    : type_(rhs.type_), name_(rhs.name_), size_(rhs.size_),
      aligned_size_(rhs.aligned_size_), crc_(rhs.crc_), offset_(rhs.offset_),
      content_(rhs.content_)
{
}

// ORIG @0x08ac3e04：仅析构 name_，不释放 content_
#include "PackSet/dtor_IndexEntry.inc"
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

#include "PackSet/clear.inc"
{
    type_ = ENTRY_PACK;
    size_ = 0;
    aligned_size_ = 0;
    crc_ = 0;
    offset_ = 0;
    content_ = 0;
}

#include "PackSet/type.inc"
#include "PackSet/name.inc"
#include "PackSet/size.inc"
#include "PackSet/aligned_size.inc"
#include "PackSet/crc.inc"
#include "PackSet/offset.inc"
#include "PackSet/content.inc"

#include "PackSet/read_info.inc"
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

#include "PackSet/write_info.inc"
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
#include "PackSet/get_content.inc"
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
#include "PackSet/get_content_1.inc"
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
#include "PackSet/get_contentEncryptedFile.inc"
                                                unsigned char*, unsigned int,
                                                unsigned int*)
{
    return PACK_ERROR;
}

// 内存整体装载 @0x08ac4378
#include "PackSet/update_content_from_memory.inc"
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
#include "PackSet/update_content_from_file.inc"
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
#include "PackSet/IPackFileProcess.inc"
{
    s_ref_count_ += 1;                       // ORIG +0x19 处 ref++
}

#include "PackSet/dtor_IPackFileProcess.inc"
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

#include "PackSet/getCommonBuffer.inc"
{
    return s_common_buffer_;
}

#include "PackSet/getCommonBuffer_1.inc"
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

#include "PackSet/PackSet.inc"
    : m_indexes(), m_addeds()
{
    // 对齐 ORIG：构造两个空 vector 后调 clear() 重置全部字段
    clear();
}

#include "PackSet/dtor_PackSet.inc"
{
    close();
}

#include "PackSet/clear_1.inc"
{
    m_packFile = 0;
    m_revision = 0;
    m_headerSize = 0;
    m_type = PACK_LOAD_FULL;
    m_indexes.clear();
    m_addeds.clear();
    m_bUseEncryptedFile = false;
}

#include "PackSet/close.inc"
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

#include "PackSet/find.inc"
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4e00 size 0x22b）：
    // ①ORIG `lea -0x64; call pair<it,it>::C1Ev` 后 `aSEOS9_`（右值赋值）⇒ er
    //   先默认构造、equal_range 结果经临时槽赋回（P28/P50 两段式）；旧形
    //   `er = equal_range(...)` 初始化式被 4.4 折叠，缺 C1Ev 与临时槽。
    // ②ORIG 无 code 预清（直接 `call _hash; mov %eax,-0x40`）⇒ 原码 `hash_t code;`
    //   无初始化（P171 照抄不写）。
    // [2026-09-11 L2Free4to15.S4 复核] fresh .o：orig=180 our=180、gap=0、AE ndiff=**0**
    //   （clean 口径全等）；strict 残差全部为「.o 未解析本地 call 标签」
    //   （`call <_ZN7PackSet4findEPKc+0x15>` vs 链接后符号名，N124）⇒ AE 即 .o 阶段上限，
    //   p167 CALLSITE 仅 C1/C2·D1/D2 同址别名。无需再改。
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

#include "PackSet/find_const.inc"
{
    // b28d-solo-PackSet 判词（AE，ORIG 0x08ac4b6c size 0x291）：
    // 与 0x08ac4e00 同族（const_iterator 版）：①声明序 code,dummy,er,file 且 code
    // 无预清；②er 先 C1Ev 再 `aSEOS9_` 赋值；③`it->`（ptEv）非 `(*it)`；④it2
    // 两段式（C1Ev + 赋值）；⑤P283 goto 尾标签复现底部条件块。
    // ORIG 局部一律**非 const** iterator（er/it2），仅返回处隐式转换为
    // const_iterator（ORIG 0x08ac4d43/0x08ac4dbb 两处 call
    // `__normal_iterator<const pair*>::C1<__normal_iterator<pair*>>` 转换构造，
    // nm 实证 @0x08ac7ce4）⇒ P96/P270 非 const 形；旧形全 const 使 4.4 直接位拷贝。
    // [2026-09-11 L2Free4to15.S4 复核] fresh .o：orig=192 our=192、gap=0、AE ndiff=**0**、
    //   p167 仅 C1/C2·D1/D2 同址别名（strict 残差 = .o 本地 call 标签，N124）⇒ 达 .o 上限。
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

#include "PackSet/is_exist.inc"
{
    // b28c-L8：ORIG 0x08ac498c 先 find(sret -0x10) 后 end(临时 -0xc)，
    // 即左操作数具名结果再比较；直接写二进制表达式 4.4 反序求值。
    index_vector_it rval = find(name);
    return rval != m_indexes.end();
}

#include "PackSet/is_ready.inc"
{
    return m_indexes.size() != 0;
}

#include "PackSet/is_dirty.inc"
{
    return m_addeds.size() != 0;
}

#include "PackSet/indexes.inc"
{
    return m_indexes;
}

#include "PackSet/indexes_1.inc"
{
    return m_indexes;
}

#include "PackSet/revision.inc"
{
    return m_revision;
}

#include "PackSet/header_size.inc"
{
    return m_headerSize;
}

#include "PackSet/get.inc"
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

#include "PackSet/get_1.inc"
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

#include "PackSet/added.inc"
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

#include "PackSet/modified.inc"
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

#include "PackSet/deleted.inc"
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

#include "PackSet/freeContent.inc"
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

#include "PackSet/sort.inc"
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

#include "PackSet/open.inc"
{
    if (UsingEncryptedFile())
    {
        return openEncryptedFile(name, tag, type);
    }

    // b28d-solo-PackSet 判词（N 封顶：两处跨版本 codegen 残差，ORIG 0x08ac5a52
    // size 0x69d；本轮 ndiff 330→9、gap→-1）：
    // ①ORIG is_ready() 之前 it C1Ev(-0x50) → index_tag C1Ev(-0x64)，两局部都在
    //   函数头、构造序 it→index_tag（P28/P50/P212）。
    // ②槽序 = 4.4 **声明序**分配：it -0x50 / idxHdrSize -0x54 / idxHdrCrc -0x58
    //   / idxHdrKey -0x5c / cur -0x60 / index_tag -0x64 ⇒ idxHdr*/cur 声明必须
    //   排在 index_tag **之前**（一处改动使 6 处 lea 立即数全对齐）。
    // ③三处遍历改 while 底部条件布局（P283）；④idxHdrSize/Crc/Key 去预清零
    //   （P171：ORIG fread 直接写槽）；⑤`cur = idxHdrData` 移到 resize 之前
    //   （ORIG +0x2ef）；⑥npos 用 volatile 视图复现内存装载。
    // 残余（语义等价，N 封顶）：a) ORIG `mov <A>,%edx; cmp %edx,%eax` 单条绝对
    //   地址装载 vs 4.4 `mov $A,%edx; mov (%edx),%edx`（非 volatile 会被折成
    //   `cmp $-1`）；b) fread 实参装载序（P75：ORIG 先 fp 后 fileSize，4.4 先把
    //   fileSize 载 %eax 再丢弃）——两处净差 1 条 mov。
    // [2026-09-11 L2Free4to15.S4] fresh .o 复核：orig=438 / our=439、gap=−1、ndiff **6**。
    //   试形：`*&tstring::npos`（去 volatile）⇒ 4.4 折 `cmp $<A>,%eax`（our 437、npos 降级
    //   为立即数）而 ndiff 不变 ⇒ 回滚维持 volatile 取址形。残余 a/b 两项均 N 封顶。
    index_vector_it it;
    unsigned int idxHdrSize;        // -0x54：索引头区大小
    hash_t idxHdrCrc;               // -0x58：头 CRC（checksum/key）
    hash_t idxHdrKey;               // -0x5c：解密种子兼条目数
    unsigned char* cur;             // -0x60：索引头游标
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

    // b28d-solo-PackSet：ORIG +0x2ef `mov -0x1c,%eax; mov %eax,-0x60` 在
    // `resize` 之前 ⇒ cur 赋值必须先于 resize。
    cur = idxHdrData;
    m_indexes.resize(static_cast<std::size_t>(idxHdrKey), index_pair_t());

    // b28d-solo-PackSet（P283）：ORIG 三处遍历均为 while 底部条件布局
    // （init → `jmp cond` → body → cond），4.4 对 for 恒把 cond 置顶。
    it = m_indexes.begin();
    while (it != m_indexes.end())
    {
        it->first = *reinterpret_cast<unsigned long*>(cur);
        cur += 4;
        it->second.read_info(cur);
        ++it;
    }
    delete[] idxHdrData;

    if (type == PACK_LOAD_FULL)
    {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        unsigned char* whole = new unsigned char[fileSize];
        fread(whole, fileSize, 1, fp);
        it = m_indexes.begin();
        while (it != m_indexes.end())
        {
            it->second.update_content_from_memory(whole, this);
            ++it;
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
            it = m_indexes.begin();
            while (it != m_indexes.end())
            {
                // b28d-solo-PackSet（p167）：ORIG 调 `IndexEntry::name()` getter
                // （0x08ac5ffa/0x08ac6010 两处 call _ZNK10IndexEntry4nameEv）
                // 而非直访 name_ 成员。
                it->second.name();
                // b28d-solo-PackSet（P304 同族）：ORIG `mov &_ZNSs4nposE,%edx;
                // cmp %edx,%eax` 为内存装载 npos；4.4 把 npos 折成立即数 ⇒
                // `*&` 强制取址装载。
                // [2026-09-11 试形记录] `*&tstring::npos`（去 volatile）虽消掉取址中转、
                //   但 4.4 折成 `cmp $<A>,%eax`（1 条）——比 ORIG 的 2 条
                //   （`mov <A>,%edx; cmp %edx,%eax`）**少** 1 条且 npos 语义降级为
                //   立即数；ndiff 不变（6）而 ours 438→437 ⇒ 回滚维持 volatile 取址形。
                if (it->second.name().find(".ani", 0) !=
                    *reinterpret_cast<volatile tstring::size_type const*>(&tstring::npos))
                {
                    it->second.update_content_from_memory(whole, this);
                }
                ++it;
            }
            delete[] whole;
        }
    }

    m_type = type;
    return PACK_OK;
}

#include "PackSet/verify.inc"
{
    if (UsingEncryptedFile())
    {
        return verifyEncryptedFile();
    }

    unsigned int bufferSize = 0x500000u;
    unsigned char* buff_file = 0;
    unsigned char* buff_pack = 0;
    unsigned int contentSize = 0;
    // b28d-solo-PackSet 判词（N 封顶：4.4 栈槽分配策略差异，ORIG 0x08ac6c8a
    // size 0x973）：
    // ①ORIG 序言 5 标量清零 → it C1Ev(-0x70) → is_ready；迭代器为函数级声明
    //   （p167 MISSING 该 C1Ev；两循环复用同一槽）。
    // ②两处遍历改 while 底部条件布局（P283）；③删 printf 首参 `+0.5f`（ORIG 为
    //   `flds 100.0f; fmulp; fistpl`，无 faddp）；④`if (1000 < since_refresh)`
    //   改 `while (…) { …; break; }` 复现 ORIG `cmp $0x3e8; setg %al;
    //   test %al,%al; je`（P225）。
    // 残余（语义等价，N82/N98）：4.1 按生命周期/类型分组分配（contentSize
    //   -0x6c、bufferSize -0x48、size_file -0x3c、proc_count -0x38…），4.4 按
    //   首次使用序紧密分配（contentSize -0x74、bufferSize -0x50…）⇒ 全帧槽号
    //   整体深 8、帧 0xac vs 0xcc。contentSize 声明位置两形实测均不翻 ⇒ 封顶。
    // [2026-09-11 L2Free4to15.S4] fresh .o 复核：orig=628 / our=634、gap=−6、ndiff **254**
    //   —— 254 行中仅 1 行为真 insert（fread(buff_file,size_file,1,fp) 实参装载序，P75），
    //   其余全是整体深 8 的槽号 replace（首槽基址常数 = N126/N82，非源级可翻）。
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

    // b28d-solo-PackSet（P283）：ORIG 两处遍历均为 while 底部条件布局。
    it = m_indexes.begin();
    while (it != m_indexes.end())
    {
        total_size += it->second.size_;
        ++it;
    }
    total_count = m_indexes.size();

    start_time = Tick::now();          // 进度更新基准
    frame_start = Tick::now();

    it = m_indexes.begin();
    while (it != m_indexes.end())
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
        // b28d-solo-PackSet（P225）：ORIG `cmp $0x3e8,%eax; setg %al; test %al,%al;
        // je` 为 4.1 条件物化 ⇒ while+break 形复现（4.4 对 if 恒直跳 jle）。
        while (1000 < since_refresh)
        {
            frame_start = Tick::now();
            run_time = Tick::now() - start_time;
            avg_time = static_cast<float>(run_time) / static_cast<float>(proc_size);
            remain_time = static_cast<int>((static_cast<float>(total_size - proc_size) * avg_time));
            // b28d-solo-PackSet：ORIG 无 `+0.5f` 的 faddp（序列为
            // `flds 100.0f; fmulp; fistpl`）⇒ 直接截断转换。
            printf("%3d%% (%2dmin %2dsec)/(%2dmin %2dsec) (%d/%d)    \n",
                   static_cast<int>((100.0f * (static_cast<float>(proc_size)
                                                / static_cast<float>(total_size)))),
                   run_time / 60000, (run_time % 60000) / 1000,
                   remain_time / 60000, (remain_time % 60000) / 1000,
                   static_cast<int>(proc_count), static_cast<int>(total_count));
            break;
        }
        ++it;
    }

    run_time = Tick::now() - start_time;
    avg_time = static_cast<float>(run_time) / static_cast<float>(proc_size);
    remain_time = static_cast<int>((static_cast<float>(total_size - proc_size) * avg_time));
    printf("%3d%% (%2dmin %2dsec)/(%2dmin %2dsec) (%d/%d)    \n",
           static_cast<int>((100.0f * (static_cast<float>(proc_size)
                                        / static_cast<float>(total_size)))),
           run_time / 60000, (run_time % 60000) / 1000,
           remain_time / 60000, (remain_time % 60000) / 1000,
           static_cast<int>(proc_count), static_cast<int>(total_count));

    delete[] buff_file;
    delete[] buff_pack;
    return PACK_OK;
}

#include "PackSet/output_file_list.inc"
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

#include "PackSet/write.inc"
{
    // b28d-solo-PackSet 判词（N 封顶：tail-merge 残差，ORIG 0x08ac60fa size
    // 0x88d；本轮 ndiff 455→32、gap→2）：
    // ①p167：ORIG 缺两个函数级迭代器默认构造 ⇒ 原码函数头声明 `it`/`it2`、
    //   循环内赋值（P28/P50 两段式）。
    // ②槽序 = 4.4「先声明深」：fp -0x18 / accumOffset -0x14 / sz -0x10 /
    //   headerBuf -0xc；发射序 fp 清零 → it(-0x34 C1Ev) → it2(-0x38) →
    //   writeIdx(-0x44) → headerBuf 清零（一处重排使 10+ 处槽引用归位）。
    // ③dir/file 声明序 file 先（ORIG dir@-0x260 深、file@-0x15c 浅）；
    // ④删 tmp.first/second 预清零（P171）；⑤accumOffset 改「值先」形 + int 化
    //   复现 `lea (%edx,%eax,1)` 序列；⑥hdrDataSz int 化复现两段装载。
    // 残余（语义等价，N121 tail-merge）：4.1 把 4 处 `return PACK_ERROR` 汇到
    //   +0x82d 共享块（`if(fp) fclose` + 双判空 delete[] + `mov $1,%ebx`），
    //   4.4 -O0 恒内联各失败路径（我们多 3 条 mov $1,%ebx、少共享块 2×cmpl/je
    //   与 jmp）。显式 goto 尾标签（+内层块）实测劣化槽位（ndiff 41/127）⇒ 保持内联。
    // [2026-09-11 L2Free4to15.S4] fresh .o 复核：orig=528 / our=526、gap=+2、ndiff **32**
    //   —— 32 行 = 4 处 `return PACK_ERROR` 未 tail-merge（我方各多 `mov $1,%ebx`、
    //   ORIG 共享块 2×`cmpl/je`+被跳过段）。属 N121（4.4 不合并条件尾块），非源级可翻。
    FILE* fp = 0;
    int accumOffset;                // -0x14（ORIG 在 sort 之后才清零）
    long sz;                        // -0x10
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
        // b28d-solo-PackSet：ORIG dir@-0x260 / file@-0x15c（先声明深）⇒ 4.4
        // 先声明浅，故 file 必须先声明。
        char file[260];
        char dir[260];
        for (it = m_indexes.begin(); it != m_indexes.end(); ++it)
        {
            index_pair2_t tmp;
            if (_correct_filename(it->second.name_.c_str(), file, 0x104,
                                  dir, 0x104) == PACK_ERROR)
            {
                fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x4ff, __FUNCTION__);
                return PACK_ERROR;      // ORIG tail-merge 至 +0x82d（N121）
            }
            tmp.first = _write_hash(file, dir);
            tmp.second = &(it->second);
            writeIdx.push_back(tmp);
        }
    }
    std::sort(writeIdx.begin(), writeIdx.end(), &_index_less2);

    accumOffset = 0;
    for (it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
    {
        it2->second->offset_ = accumOffset;
        // b28d-solo-PackSet（P38/P183）：ORIG 为值先 lea 形
        // （`mov 0xc(%eax),%edx; mov -0x14,%eax; lea (%edx,%eax,1),%eax;
        // mov %eax,-0x14`），`+=` 在 4.4 生 `add %eax,-0x14` 内存自增。
        accumOffset = it2->second->aligned_size_ + accumOffset;
    }

    fp = fopen(TEMP_HEADER_FILENAME, "wb");
    if (fp == 0)
    {
        fprintf(stdout, "_tfopen failed. %s", TEMP_HEADER_FILENAME);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x514, __FUNCTION__);
        return PACK_ERROR;              // ORIG tail-merge 至 +0x82d
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
    sz = ftell(fp);
    if (sz < 0)
    {
        // ORIG sz<0 不 fclose，直接返回（实证 0x08ac65ef）
        return PACK_ERROR;
    }
    fseek(fp, 0, SEEK_SET);

    int hdrDataSz = static_cast<int>(sz + 3) & ~3;
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

#include "PackSet/pack_file.inc"
{
    if (!m_bUseEncryptedFile)
    {
        return m_packFile;
    }
    fprintf(stdout, "%s\n", "_C_DNF_SCRIPT_CRYPTER_ is turned on!!");
    fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x677, __FUNCTION__);
    return m_packFile;
}

#include "PackSet/UsingEncryptedFile.inc"
{
    return m_bUseEncryptedFile;
}

#include "PackSet/UseEncryptedFile.inc"
{
    m_bUseEncryptedFile = true;
}

#include "PackSet/openEncryptedFile.inc"
{
    return PACK_ERROR;
}

#include "PackSet/closeEncryptedFile.inc"
{
}

#include "PackSet/verifyEncryptedFile.inc"
{
    return PACK_ERROR;
}
