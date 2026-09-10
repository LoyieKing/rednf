// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PackSet/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "PackSet/crc32.inc"
}

#include "PackSet/getCrc32Table.inc"
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
}

// ---------------------------------------------------------------------------
// IndexEntry（ORIG 0x08ac3de2..0x08ac5b4x）
// ---------------------------------------------------------------------------
#include "PackSet/IndexEntry.inc"
}

// ORIG W @0x08ac7a50：隐式逐成员拷贝
#include "PackSet/IndexEntry_1.inc"
}

// ORIG @0x08ac3e04：仅析构 name_，不释放 content_
#include "PackSet/dtor_IndexEntry.inc"
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
}

#include "PackSet/type.inc"
#include "PackSet/name.inc"
#include "PackSet/size.inc"
#include "PackSet/aligned_size.inc"
#include "PackSet/crc.inc"
#include "PackSet/offset.inc"
#include "PackSet/content.inc"

#include "PackSet/read_info.inc"
}

#include "PackSet/write_info.inc"
}

// 主版本（dst 预分配）@0x08ac3eac
// b28d-solo-PackSet 判词（AE）：p167 20=20 CLEAR；旧 b28d-36「N95 帧差封顶」
// 判词已作废——消除 pos 具名局部（实参内联，P0/P52）后帧差归零。
#include "PackSet/get_content.inc"
}

// 分配版本 @0x08ac5880：new[] 后转发到主版本
#include "PackSet/get_content_1.inc"
}

// 加密文件分支 @0x08ac41f6：ORIG 恒返 PACK_ERROR
#include "PackSet/get_contentEncryptedFile.inc"
}

// 内存整体装载 @0x08ac4378
#include "PackSet/update_content_from_memory.inc"
}

// 从外部文件装载 @0x08ac4476；process 非空时走虚接口 loadFile
#include "PackSet/update_content_from_file.inc"
}

// ---------------------------------------------------------------------------
// IPackFileProcess（ORIG 0x08ac768c..0x08ac776x）
// ---------------------------------------------------------------------------
#include "PackSet/IPackFileProcess.inc"
}

#include "PackSet/dtor_IPackFileProcess.inc"
}

#include "PackSet/getCommonBuffer.inc"
}

#include "PackSet/getCommonBuffer_1.inc"
}

// ---------------------------------------------------------------------------
// PackSet（ORIG 0x08ac47f4..0x08ac76xx）
// ---------------------------------------------------------------------------

#include "PackSet/PackSet.inc"
}

#include "PackSet/dtor_PackSet.inc"
}

#include "PackSet/clear_1.inc"
}

#include "PackSet/close.inc"
}

#include "PackSet/find.inc"
}

#include "PackSet/find_const.inc"
}

#include "PackSet/is_exist.inc"
}

#include "PackSet/is_ready.inc"
}

#include "PackSet/is_dirty.inc"
}

#include "PackSet/indexes.inc"
}

#include "PackSet/indexes_1.inc"
}

#include "PackSet/revision.inc"
}

#include "PackSet/header_size.inc"
}

#include "PackSet/get.inc"
}

#include "PackSet/get_1.inc"
}

#include "PackSet/added.inc"
}

#include "PackSet/modified.inc"
}

#include "PackSet/deleted.inc"
}

#include "PackSet/freeContent.inc"
}

#include "PackSet/sort.inc"
}

#include "PackSet/open.inc"
}

#include "PackSet/verify.inc"
}

#include "PackSet/output_file_list.inc"
}

#include "PackSet/write.inc"
}

#include "PackSet/pack_file.inc"
}

#include "PackSet/UsingEncryptedFile.inc"
}

#include "PackSet/UseEncryptedFile.inc"
}

#include "PackSet/openEncryptedFile.inc"
}

#include "PackSet/closeEncryptedFile.inc"
}

#include "PackSet/verifyEncryptedFile.inc"
}
