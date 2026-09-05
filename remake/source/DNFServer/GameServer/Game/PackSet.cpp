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

static const char* k_srcFile = "../../Include/FilePack/Pack.cpp";

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
    unsigned int h = 1u;
    crc_table[0] = 0;
    for (unsigned int i = 0x80u; i != 0; i >>= 1)
    {
        h = ((h & 1u) == 0u) ? 0u : 0xedb88320u;
        h ^= (h >> 1);
        for (unsigned int j = 0u; j < 0x100u; j += i * 2u)
        {
            crc_table[i + j] = crc_table[j] ^ h;
        }
    }
}

// Crc32 —— docs/class_func_reports/Crc32/{crc32,getCrc32Table}.md
unsigned int Crc32::crc32(unsigned int crc, char const* buf, unsigned int len)
{
    if (crc_table[255] == 0)
    {
        make_crc_table();
    }
    crc = ~crc;
    for (; len != 0; --len)
    {
        crc = crc_table[(static_cast<unsigned char>(*buf) ^ crc) & 0xffu] ^ (crc >> 8);
        ++buf;
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
    unsigned int* e = reinterpret_cast<unsigned int*>(buffer + (size & 0xfffffffcu));
    for (; p != e; ++p)
    {
        unsigned int v = *p;
        *p = ((v >> 0x1a) | (v << 6)) ^ key;
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
    unsigned int* e = reinterpret_cast<unsigned int*>(buffer + (size & 0xfffffffcu));
    for (; p != e; ++p)
    {
        if (g_AdditionalKey != 0u)
        {
            *p ^= g_AdditionalKey;
        }
        unsigned int v = (*p ^ key);
        *p = (v >> 6) | (v << 0x1a);
        unsigned char const* bytes = reinterpret_cast<unsigned char const*>(p);
        unsigned int tmp = table[(bytes[0] ^ crc) & 0xffu] ^ (crc >> 8);
        tmp = table[(bytes[1] ^ tmp) & 0xffu] ^ (tmp >> 8);
        tmp = table[(bytes[2] ^ tmp) & 0xffu] ^ (tmp >> 8);
        crc = table[(bytes[3] ^ tmp) & 0xffu] ^ (tmp >> 8);
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
    unsigned int len = 0;
    fread(&len, 4, 1, fp);
    if (len > 0x103u)
    {
        return PACK_ERROR;
    }
    char buf[256];
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
    char buf[256];
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

    std::stack<char const*> st;      // 分量起点栈（ORIG stack<char*, deque<char*>>）
    char* w = dst;                   // -0x44
    char const* r = src;             // -0x10
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
        char const* seg = st.top();       // -0x14
        unsigned long diff = static_cast<unsigned long>(w - seg);
        if (diff == 1)
        {
            w = const_cast<char*>(seg);              // 段为空
        }
        else if (diff == 2 && seg[0] == '.')
        {
            w = const_cast<char*>(seg);              // 抛弃 "./"
        }
        else if (diff == 3 && seg[0] == '.' && seg[1] == '.')
        {
            if (st.empty())
            {
                return 1;                            // 空栈分支实际不可达，忠实保留
            }
            st.pop();
            if (st.empty())
            {
                return 1;
            }
            w = const_cast<char*>(st.top());
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

    char* tail = const_cast<char*>(st.top());
    ss_strcpy(dir, dir_size, tail);
    if (tail == dst)
    {
        const_cast<char*>(tail)[0] = '\0';
    }
    else
    {
        const_cast<char*>(tail)[-1] = '\0';          // 去尾段（ORIG sptr[-1]=0）
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
    FILE* fp = owner->pack_file();
    if (fp == 0)
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

    if (content_ == 0)
    {
        if (type_ == ENTRY_FILE)
        {
            fprintf(stdout, "%s\n", "not implemented");
            fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x158, __FUNCTION__);
            return PACK_ERROR;
        }
        if (type_ == ENTRY_PACK && aligned_size_ != 0)
        {
            unsigned int pos = owner->header_size() + offset_;
            fseek(owner->pack_file(), static_cast<long>(pos), SEEK_SET);
            if (fread(dst, aligned_size_, 1, owner->pack_file()) != 1)
            {
                fprintf(stdout, "%s\n", "fread failed");
                fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x163, __FUNCTION__);
                return PACK_ERROR;
            }
        }
    }
    else
    {
        memcpy(dst, content_, aligned_size_);
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
        content_[0] = '\0';
        content_[1] = '\0';
        content_[2] = '\0';
        content_[3] = '\0';
    }
    else if (process == 0)
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
    else
    {
        fclose(fp);
        fp = 0;
        // vtable slot2 → loadFile(name,&content_,&size_)；返回 1 视为失败
        pack_ret_t ret = process->loadFile(dir, &content_, &size_);
        if (ret == PACK_ERROR)
        {
            return PACK_ERROR;
        }
        aligned_size_ = (size_ + 3u) & ~3u;
    }

    if (fp != 0)
    {
        fclose(fp);
    }

    hash_t hash = _hash(toMbcs(dir));
    crc_ = Crc32::crc32(static_cast<unsigned int>(hash),
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
            delete[] s_common_buffer_;
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
    if (s_common_buffer_size_ < needSize)
    {
        if (s_common_buffer_ != 0 && s_common_buffer_ != 0)
        {
            delete[] s_common_buffer_;
        }
        s_common_buffer_ = new unsigned char[needSize];
        s_common_buffer_size_ = needSize;
        if (s_common_buffer_ == 0)
        {
            s_common_buffer_size_ = 0;
        }
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
    if (UsingEncryptedFile())
    {
        closeEncryptedFile();
        return;
    }

    if (m_packFile != 0)
    {
        fclose(m_packFile);
    }

    // 释放两份索引中的内容缓冲（operator delete[]）
    for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        if (it->second.content_ != 0)
        {
            delete[] it->second.content_;
            it->second.content_ = 0;
        }
    }
    for (index_vector_it it = m_addeds.begin(); it != m_addeds.end(); ++it)
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
    char file[260];
    IndexEntry dummy;
    unsigned long code = 0;

    _correct_filename(name, file, 0x104, 0, 0);
    code = _hash(toMbcs(file));

    std::pair<index_vector_it, index_vector_it> er =
        std::equal_range(m_indexes.begin(), m_indexes.end(),
                         index_pair_t(code, dummy), &_index_less);

    if (er.first == m_indexes.end() || (*er.first).first != code)
    {
        return m_indexes.end();
    }

    for (index_vector_it it2 = er.first;; ++it2)
    {
        bool keep = ((*it2).first == code && it2 != er.second);
        if (!keep)
        {
            break;
        }
        if (strcmp((*it2).second.name_.c_str(), file) == 0)
        {
            return it2;
        }
    }
    return m_indexes.end();
}

PackSet::index_vector_const_it PackSet::find_const(char const* name)   // sret @0x08ac4b6c
{
    char file[260];
    IndexEntry dummy;
    unsigned long code = 0;

    _correct_filename(name, file, 0x104, 0, 0);
    code = _hash(toMbcs(file));

    std::pair<index_vector_const_it, index_vector_const_it> er =
        std::equal_range(m_indexes.begin(), m_indexes.end(),
                         index_pair_t(code, dummy), &_index_less);

    if (er.first == m_indexes.end() || (*er.first).first != code)
    {
        return m_indexes.end();
    }

    for (index_vector_const_it it2 = er.first;; ++it2)
    {
        bool keep = ((*it2).first == code && it2 != er.second);
        if (!keep)
        {
            break;
        }
        if (strcmp((*it2).second.name_.c_str(), file) == 0)
        {
            return it2;
        }
    }
    return m_indexes.end();
}

bool PackSet::is_exist(char const* name)     // @0x08ac498c
{
    return find(name) != m_indexes.end();
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
    if (name == 0 || buffer == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x312, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x319, __FUNCTION__);
        return PACK_ERROR;
    }

    hash_t code = (*it).first;
    if ((*it).second.get_content(this, code, buffer, buffer_size, content_size) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 799, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

pack_ret_t PackSet::get(char const* name, unsigned char** content,
                        unsigned int* content_size)                             // 0x08ac51ce
{
    if (name == 0 || content == 0 || content_size == 0)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x32c, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
    if (it == m_indexes.end())
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x333, __FUNCTION__);
        return PACK_ERROR;
    }

    hash_t code = (*it).first;
    if ((*it).second.get_content(this, code, content, content_size) == PACK_ERROR)
    {
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x339, __FUNCTION__);
        return PACK_ERROR;
    }
    return PACK_OK;
}

pack_ret_t PackSet::added(char const* name, IPackFileProcess* process)   // 0x08ac56a2
{
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x38a, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
    if (it != m_indexes.end())
    {
        fprintf(stdout, "find %s", name);
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x390, __FUNCTION__);
        return PACK_ERROR;
    }

    index_pair_t entry;
    entry.first = 0;
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
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x369, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
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
        entry.offset_ = 0;
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
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x346, __FUNCTION__);
        return PACK_ERROR;
    }

    index_vector_it it = find(name);
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
    index_vector_it it = find(name);
    if (it != m_indexes.end() && it->second.content_ != 0)
    {
        delete[] it->second.content_;
        it->second.content_ = 0;
    }
}

pack_ret_t PackSet::sort()      // 0x08ac58ee
{
    if (m_type != PACK_LOAD_FULL)
    {
        fprintf(stdout, "%s\n", "PACK_LOAD_FULL needed");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x3b7, __FUNCTION__);
        return PACK_ERROR;
    }

    m_indexes.reserve(m_addeds.size());
    for (index_vector_it it = m_addeds.begin(); it != m_addeds.end(); ++it)
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
    for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
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
        for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
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
            for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
            {
                if (it->second.name_.find(".ani", 0) != tstring::npos)
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
    size_t proc_count = 0;
    size_t total_count = 0;
    unsigned int proc_size = 0;
    unsigned int total_size = 0;

    if (!is_ready())
    {
        fprintf(stdout, "%s\n", "Not yet opened");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x5a5, __FUNCTION__);
        return PACK_ERROR;
    }

    buff_file = new unsigned char[bufferSize];
    buff_pack = new unsigned char[bufferSize];

    int run_time = 0;
    float avg_time = 0.0f;
    int remain_time = 0;

    for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
    {
        total_size += it->second.size_;
    }
    total_count = m_indexes.size();

    Tick start_time;                   // ORIG：先默认构造再赋值（TickC1+now+aSES）
    start_time = Tick::now();          // 进度更新基准
    Tick frame_start;                  // 最后一次刷新时刻
    frame_start = Tick::now();

    for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
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
    FILE* fp = 0;
    unsigned char* headerBuf = 0;
    pack_ret_t ret;

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

    index_vector2_t writeIdx;
    {
        char dir[260];
        char file[260];
        for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
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
    for (index_vector2_it it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
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

    for (index_vector_it it = m_indexes.begin(); it != m_indexes.end(); ++it)
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

    for (index_vector2_it it2 = writeIdx.begin(); it2 != writeIdx.end(); ++it2)
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
    if (!(static_cast<bool>(m_bUseEncryptedFile) ^ true))     // !flag == false 即 flag 真
    {
        fprintf(stdout, "%s\n", "_C_DNF_SCRIPT_CRYPTER_ is turned on!!");
        fprintf(stdout, "%s(%d) - %s\n", k_srcFile, 0x677, __FUNCTION__);
    }
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
