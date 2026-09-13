#ifndef DNF_GAME_PACKSET_H_
#define DNF_GAME_PACKSET_H_

// ============================================================================
// Pack 文件系统包装（Nexon FilePack/Pack.cpp 区）权威声明
//
// ORIG df_game_r 证据：
//   PackSet            T @0x08ac47f4..0x08ac76xx  （docs/class_func_reports/PackSet/）
//   IndexEntry         T @0x08ac3de2..            （docs/class_func_reports/IndexEntry/）
//   IPackFileProcess   T @0x08ac768c..            （docs/class_func_reports/IPackFileProcess/）
//   Crc32              T @0x08ad34ae..            （docs/class_func_reports/Crc32/）
//
// 布局（objdump/nm 核实）：
//   PackSet:  +0x00 pack_file_(FILE*)  +0x04 revision_(uint) +0x08 header_size_(uint)
//             +0x0c type_(pack_load_type_t)  +0x10 indexes_  +0x1c addeds_
//             +0x28 m_bUseEncryptedFile(bool)   sizeof = 0x2c
//   IndexEntry: +0x00 type_(entry_type_t) +0x04 name_(std::string) +0x08 size_
//             +0x0c aligned_size_ +0x10 crc_(ulong) +0x14 offset_
//             +0x18 content_(uchar*)            sizeof = 0x1c
//   IPackFileProcess: vptr@0；虚表 (_ZTV16IPackFileProcess @0x08e2eaf0):
//             slot0=D1 0x08ac76a8, slot1=D0 0x08ac7716, slot2=__cxa_pure_virtual
// ============================================================================
class IPackFileProcess;


#include <cstdio>
#include <string>
#include <utility>
#include <vector>

typedef unsigned long hash_t;   // ORIG ulong（elf32-i386 → 4 字节）
typedef std::string tstring;

enum pack_load_type_t
{
    // [b24FIX · 取值方向纠正] ORIG 实证（PackSet::open 0x08ac5a52）：
    //   0x08ac5b8d `cmp DWORD PTR [ebp+0x10],0x0` + `je 0x08ac5c21` ⇒ **type==0 走
    //   .ani 过滤分支**（该分支在 `call update_content_from_memory`(0x08ac605b) 之前先
    //   `IndexEntry::name()` → `std::string::find(".ani",0)`（串 @0x8e2e8cf = ".ani\0"）
    //   → 与 npos 比较后跳过不命中者）；**type!=0 才走全量装载分支**（0x08ac5ef1 调用点
    //   无过滤，mem = 整个文件缓冲）。
    //   ⇒ PACK_LOAD_INDEX = 0、PACK_LOAD_FULL = 1。旧表把两者写反，导致 InitRDARScript
    //   的 `open(name, 0, PACK_LOAD_INDEX)` 被当成 FULL ⇒ 对不含 .ani 的条目按无效
    //   offset_ 执行 memcpy(源 = mem + header_size() + offset_) ⇒ 野指针 SIGSEGV
    //   （实机栈实证：返回址 0x0829eeb4、size=0x50、src=0xd3e4f6a4）。
    PACK_LOAD_INDEX = 0,        // 仅索引 + .ani 条目内容（open 的 ==0 分支）
    PACK_LOAD_FULL  = 1         // 全量装载（open 的非 0 分支；modified/added/deleted/
                                // write/sort 均以 != PACK_LOAD_FULL 判「需全量」）
};

enum entry_type_t
{
    ENTRY_PACK = 0,             // IndexEntry::clear() 置 0
    ENTRY_FILE = 1              // IndexEntry::update_content_from_file 设置
};

enum pack_ret_t
{
    PACK_OK    = 0,
    PACK_ERROR = 1
};

class PackSet;

class IndexEntry
{
public:
    IndexEntry();                                // _ZN10IndexEntryC1Ev  0x08ac3de2
    IndexEntry(const IndexEntry& rhs);           // _ZN10IndexEntryC1ERKS_ W 0x08ac7a50
    ~IndexEntry();                               // _ZN10IndexEntryD1Ev  0x08ac3e04
    IndexEntry& operator=(const IndexEntry& rhs);// _ZN10IndexEntryaSERKS_ W 0x08ac9d92

    void clear();                                // 0x08ac3e1a

    entry_type_t type() const;                   // 0x08ac3e5a
    const tstring& name() const;                 // 0x08ac3e64（返回 &name_）
    unsigned int size() const;                   // 0x08ac3e70
    unsigned int aligned_size() const;           // 0x08ac3e7c
    hash_t crc() const;                          // 0x08ac3e88
    unsigned int offset() const;                 // 0x08ac3e94
    unsigned char* content() const;              // 0x08ac3ea0

    pack_ret_t read_info(unsigned char*& mem);   // 0x08ac4200
    pack_ret_t write_info(FILE* fp);             // 0x08ac4284

    pack_ret_t get_content(PackSet* owner, hash_t code, unsigned char* dst,
                           unsigned int dst_size, unsigned int* content_size);   // 0x08ac3eac
    pack_ret_t get_content(PackSet* owner, hash_t code, unsigned char** dst,
                           unsigned int* content_size);                          // 0x08ac5880
    pack_ret_t get_contentEncryptedFile(PackSet* owner, hash_t code, unsigned char* dst,
                                        unsigned int dst_size, unsigned int* content_size); // 0x08ac41f6（恒返 PACK_ERROR）

    pack_ret_t update_content_from_memory(unsigned char* mem, PackSet* owner);    // 0x08ac4378
    pack_ret_t update_content_from_file(char const* name, PackSet* owner,
                                        IPackFileProcess* process);               // 0x08ac4476
    // ORIG 同 TU（Pack.cpp）直访字段，保持 public：
    entry_type_t   type_;

    tstring        name_;
    unsigned int   size_;
    unsigned int   aligned_size_;
    hash_t         crc_;
    unsigned int   offset_;
    unsigned char* content_;
};

class IPackFileProcess
{
public:
    IPackFileProcess();                          // 0x08ac768c
    virtual ~IPackFileProcess();                 // D1 0x08ac76a8 / D0 0x08ac7716

    // 虚表 slot2 = __cxa_pure_virtual；由 update_content_from_file 调用，
    // 返回 1 视为失败（0x08ac4632 call，cmp $0x1）。
    virtual pack_ret_t loadFile(char const* name, unsigned char** content,
                                unsigned int* size) = 0;

    unsigned char* getCommonBuffer();            // 0x08ac7734（返回 s_common_buffer_）
    unsigned char* getCommonBuffer(unsigned int needSize);  // 0x08ac773e

    static unsigned char* s_common_buffer_;      // 0x095018a0 B
    static unsigned int   s_common_buffer_size_; // 0x095018a4 B
    static int            s_ref_count_;          // 0x095018a8 B
};

class Crc32
{
public:
    static unsigned int crc32(unsigned int crc, char const* buf, unsigned int len); // 0x08ad34ae T
    static unsigned int* getCrc32Table();                                           // 0x08ad3504 T
};

class PackSet
{
public:
    typedef std::pair<hash_t, IndexEntry>  index_pair_t;
    typedef std::vector<index_pair_t>      index_vector_t;
    typedef index_vector_t::iterator       index_vector_it;
    typedef index_vector_t::const_iterator index_vector_const_it;
    typedef std::pair<hash_t, IndexEntry*> index_pair2_t;
    typedef std::vector<index_pair2_t>     index_vector2_t;
    typedef index_vector2_t::iterator      index_vector2_it;

    PackSet();                                    // C1/C2 0x08ac47f4
    ~PackSet();                                   // D1 0x08ac4862

    void clear();                                 // 0x08ac48d2
    void close();                                 // 0x08ac49da

    index_vector_it find(char const* name);       // 0x08ac4e00（sret）
    index_vector_const_it find_const(char const* name);  // 0x08ac4b6c（sret）

    bool is_exist(char const* name);              // 0x08ac498c
    bool is_ready() const;                        // 0x08ac4954
    bool is_dirty() const;                        // 0x08ac4970

    index_vector_t& indexes();                    // 0x08ac4948
    const index_vector_t& indexes() const;        // 0x08ac493c
    unsigned int revision();                      // 0x08ac4924
    unsigned int header_size();                   // 0x08ac4930

    pack_ret_t get(char const* name, unsigned char* buffer,
                   unsigned int buffer_size, unsigned int* content_size);          // 0x08ac5060
    pack_ret_t get(char const* name, unsigned char** content,
                   unsigned int* content_size);                                    // 0x08ac51ce

    pack_ret_t added(char const* name, IPackFileProcess* process);                 // 0x08ac56a2
    pack_ret_t modified(char const* name, IPackFileProcess* process);              // 0x08ac54ee
    pack_ret_t deleted(char const* name);                                          // 0x08ac5332
    void freeContent(char const* name);                                            // 0x08ac5448

    pack_ret_t sort();                            // 0x08ac58ee

    pack_ret_t open(char const* name, char const* tag, pack_load_type_t type);     // 0x08ac5a52
    pack_ret_t verify();                          // 0x08ac6c8a

    pack_ret_t output_file_list(char const* pszOutputPath) const;                  // 0x08ac6988
    pack_ret_t write(char const* name, char const* tag, unsigned int rev);         // 0x08ac60fa

    FILE* pack_file();                            // 0x08ac7608

    bool UsingEncryptedFile();                    // 0x08ac7674
    void UseEncryptedFile();                      // 0x08ac7680
    pack_ret_t openEncryptedFile(char const* name, char const* tag,
                                 pack_load_type_t type);  // 0x08ac60f0（恒返 PACK_ERROR）
    void closeEncryptedFile();                    // 0x08ac4b66（空体）
    pack_ret_t verifyEncryptedFile();             // 0x08ac75fe（恒返 PACK_ERROR）

private:
    FILE*            m_packFile;          // +0x00 DWARF: pack_file_
    unsigned int     m_revision;          // +0x04 revision_
    unsigned int     m_headerSize;        // +0x08 header_size_
    pack_load_type_t m_type;              // +0x0c type_
    index_vector_t   m_indexes;           // +0x10 indexes_
    index_vector_t   m_addeds;            // +0x1c addeds_
    bool             m_bUseEncryptedFile; // +0x28
};

static_assert(sizeof(IndexEntry) == 0x1c, "IndexEntry layout");
static_assert(sizeof(PackSet) == 0x2c, "PackSet layout");

#endif // DNF_GAME_PACKSET_H_
