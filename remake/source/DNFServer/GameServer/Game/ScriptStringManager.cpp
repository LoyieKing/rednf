// ============================================================================
// ScriptStringManager 底层引擎（G4 词法批次；ORIG 地址与汇编证据见
// docs/class_func_reports/ScriptStringManager.md）
// ----------------------------------------------------------------------------
// 本 TU 提供（符号名与 ORIG ABI 一致，由真实 C++ 类成员函数定义生成）：
//   ScriptStringManager::ScriptStringManager()   _ZN19ScriptStringManagerC1Ev
//                                                          ORIG 0x8acf110
//   ScriptStringManager::~ScriptStringManager()  _ZN19ScriptStringManagerD1Ev
//                                                          ORIG 0x8acf5f0
//                                                _ZN19ScriptStringManagerD0Ev
//                                                          ORIG 0x8acf700
//   ScriptStringManager::importFromBuffer(...)   _ZN19ScriptStringManager16importFromBufferEPKhi
//                                                          ORIG 0x8acf970
//   ScriptStringManager::importFromFile(...)     _ZN19ScriptStringManager14importFromFileEPKc
//                                                          ORIG 0x8acfd00
//   ScriptStringManager::clear()                 _ZN19ScriptStringManager5clearEv
//                                                          ORIG 0x8acf550
//   ScriptStringManager::exportToBuffer(...)     _ZNK19ScriptStringManager14exportToBufferEPhi
//                                                          ORIG 0x8acf420
//   ScriptStringManager::exportToFile(...)       _ZNK19ScriptStringManager14exportToFileEPKc
//                                                          ORIG 0x8acf2a0
//   ScriptStringManager::getString(int,str*)     _ZNK19ScriptStringManager9getStringEiPSs
//                                                          ORIG 0x8acf160
//   ScriptStringManager::getString(int)          _ZNK19ScriptStringManager9getStringEi
//                                                          ORIG 0x8acf1f0
//   ScriptStringManager::insertString(...)       _ZN19ScriptStringManager12insertStringEPKc
//                                                          ORIG 0x8acf720
//
// 类布局依据 ORIG ScriptStringManager::ScriptStringManager（0x8acf110）：
//   +0x00 vptr（虚析构；CompiledDNFLex::destroy 0x8ad0370 经 vptr[1]=D0 释放）
//   +0x04 strings_    std::vector<std::string>（4.4 COW，元素 4 字节）
//   +0x10 string_map_ std::map<std::string,int>
//     （c6root 4.4.4 bits/stl_tree.h：_Rb_tree_impl{_Node_allocator(EBO),
//      _M_key_compare(4), _M_header(16), _M_node_count(4)} = 24 字节；
//      header 起于 map+4 → 对象 +0x14 color/+0x18 parent/+0x1c left/
//      +0x20 right/+0x24 node_count，与 ORIG 构造逐条吻合）
//   总大小 0x28，与 CompiledDNFLex::create（0x8ad03d0）
//   operator new(0x28) 一致。
//
// 字符串表二进制格式（exportToBuffer 0x8acf420 写出，本函数读回）：
//   [0:4]       count（字符串条数）
//   [4:4]       offsets[0] = 4 + 4*count（理论字符串区起点，仅差值有意义）
//   [4+4*i:4]   offsets[i] = offsets[i-1] + len(str[i-1])，i = 1..count-1
//   [4*count+4:4] offsets[count] = total（哨兵，= 4*count+4+Σlen，比文件
//                总大小 4*count+8+Σlen 小 4；import 只用相邻差值）
//   [4*count+8: ] 字符串数据：第 i 个字符串长 offsets[i+1]-offsets[i]，
//               数据区顺序排布（export 从 buffer+4*count+8 起写）
//
// importFromBuffer 语义（0x8acf970）：
//   1. buffer==NULL 或 buffSize<4 → false
//   2. 读 count；逐步校验 buffSize 后读入 count+1 个 offsets
//   3. reserve 字符串向量 count；i=1..count：
//        len = offsets[i]-offsets[i-1]；拷贝到 1024 栈缓冲并 NUL 终止
//        （len>1023 时原版只拷前 1023 字节——栈缓冲截断，行为保留）
//        构造 std::string push 到 strings_；缓冲越界 → false
//        （已入列部分保留，ORIG 如此）
//   4. 全部成功 → true
//
// importFromFile（0x8acfd00）：fopen "rb" → fseek END → ftell → fseek 0 →
//   new[size] → fread → fclose → importFromBuffer（结果忽略）→ delete[] →
//   true；任一步失败 → false（ORIG 在 importFromBuffer 后无条件 return
//   true，忠实保留）。
//
// clear（0x8acf550）：strings_ 各元素释放（COW refcount 递减）后 finish 重置
//   为 start，string_map_ 整树 _M_erase 后 header 复位。等价于标准
//   strings_.clear(); string_map_.clear();（二者均不释放 capacity）。
//
// exportToBuffer（0x8acf420，const）：按文件头格式写 count + count+1 个
//   offset 字 + 字符串数据到用户缓冲；返回写入总字节数，buffSize 不足
//   任一段时返回 -1（写前逐段检查：offsets 区每字、数据区每串 memcpy 后）。
//   注意 ORIG 无 buffer==NULL 检查（UB），此处以返回 -1 作护栏。
//
// exportToFile（0x8acf2a0，const）：fopen(fileName,"wb") → fwrite count →
//   count>0 时逐串 fwrite 累加后的 offset 字 → fwrite 最终 total 哨兵 →
//   count>0 时逐串 fwrite 数据（len!=0 才写）→ fclose，全程 true；任一
//   fwrite 失败 → fclose 后 false；fopen 失败 → false（无 fclose）。
//
// getString(int,string*)（0x8acf160，const，bool）：先 _M_mutate(0,len,0)
//   清空输出串，再 assign(strings_[index])，成功 true。ORIG 的负 index
//   检查是死代码（负 index 恒走越界 assign，UB），此处按 DWARF 意图以
//   越界返回 false 作护栏。
//
// getString(int)（0x8acf1f0，const，返回指针）：函数内静态 DEFAULT_STRING
//   （guard 0x9501a00 惰性初始化，构造自 ""）；index<0 或 index>=条数时
//   返回 &DEFAULT_STRING，否则返回 &strings_[index]。ORIG 返回非常量
//   string*（DWARF），const 成员下取成员地址为 const，此处按 const 返回。
//
// insertString（0x8acf720，int）：字符串表 intern：已在 string_map_ 中 →
//   返回既有 index；否则 push 到 strings_ 并 map 插入 {string,新 index}，
//   返回新 index。ORIG 的"已存在"路径返回树节点内字段（无意义值），
//   插入路径返回新 index（8acf891 计算）；本实现统一按 intern 语义
//   返回 index。ORIG 无 string==NULL 检查（UB），此处以空串作护栏。
// ============================================================================

#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>

// ---------------------------------------------------------------------------
// 类定义：成员顺序/类型与 ORIG 布局一致（见文件头）。
// 构造函数/虚析构显式定义（非 inline）以按 ORIG 符号名生成
// _ZN19ScriptStringManagerC1Ev / D1Ev / D0Ev；vtable（key function=虚析构）
// 由本 TU 生成，槽位 0=D1（complete）、槽位 1=D0（deleting），
// 与 CompiledDNFLex::destroy 经 vptr[1] 释放的调用约定匹配。
// ---------------------------------------------------------------------------
class ScriptStringManager
{
public:
    ScriptStringManager();
    virtual ~ScriptStringManager();

    bool importFromBuffer(const unsigned char* buffer, int buffSize);
    bool importFromFile(const char* fileName);
    void clear();
    int exportToBuffer(unsigned char* buffer, int buffSize) const;
    bool exportToFile(const char* fileName) const;
    bool getString(int index, std::string* outputString) const;
    const std::string* getString(int index) const;
    int insertString(const char* string);

private:
    std::vector<std::string> strings_;      // +0x04
    std::map<std::string, int> string_map_; // +0x10
};

// clear @ 0x8acf550：逐元素释放 strings_（COW refcount 递减，8acf56a 检查
// empty rep 0x948ccf0 后 _M_destroy）→ finish 重置为 start（8acf57e）→
// string_map_ 整树 _M_erase（8acf58b）→ header left/right=自身、parent=0、
// node_count=0（8acf590-8acf5a0）。标准 strings_.clear(); string_map_.clear();
// 语义完全等价（二者均保留 capacity）。
void ScriptStringManager::clear()
{
    strings_.clear();
    string_map_.clear();
}

// getString(int,string*) @ 0x8acf160：先 _M_mutate(0,len,0) 清空输出串
// （8acf194），再 assign(strings_[index])（8acf1aa），成功返回 true。
// ORIG 的负 index 分支检查是死代码（负 index 恒满足 index<=size 而走
// 越界 assign，UB），此处按 DWARF 意图以越界返回 false 作护栏；
// outputString==NULL 亦为 ORIG UB，护栏返回 false。
bool ScriptStringManager::getString(int index, std::string* outputString) const
{
    if (outputString == 0)
        return false;
    outputString->clear();  // ORIG: _M_mutate(0, length, 0)
    if (index < 0 || static_cast<std::size_t>(index) >= strings_.size())
        return false;       // ORIG 死代码路径（DWARF 意图：越界失败）
    *outputString = strings_[static_cast<std::size_t>(index)];
    return true;
}

// getString(int) @ 0x8acf1f0：函数内静态 DEFAULT_STRING（guard 0x9501a00
// 惰性初始化，ORIG 经 _ZNSsC1EPKcRKSaIcE 自 "" 构造，对象 0x9501a08）。
// index<0 或 index>=条数 → &DEFAULT_STRING（8acf221），否则返回
// &strings_[index]（8acf226）。ORIG 返回非常量 string*（DWARF 原型），
// const 成员取成员地址为 const，此处按 const std::string* 返回。
const std::string* ScriptStringManager::getString(int index) const
{
    static const std::string empty("");  // ORIG DEFAULT_STRING
    if (index < 0 || static_cast<std::size_t>(index) >= strings_.size())
        return &empty;
    return &strings_[static_cast<std::size_t>(index)];
}

// insertString @ 0x8acf720：字符串表 intern。已存在（map 树中找到，
// 8acf7f5 后 8acf802 分支）→ 返回既有 index；否则 push 到 strings_
// （8acf8f2 _M_insert_aux / 8acf85c 原地拷贝构造）→ map _M_insert_unique
// 插入 {string, 新 index}（8acf8b7）→ 返回新 index（8acf891 计算，
// (finish-start)>>2 - 1）。ORIG"已存在"路径返回树节点内字段（无意义
// 值），本实现统一按 intern 语义返回 index；string==NULL 为 ORIG UB，
// 此处以空串作护栏。
int ScriptStringManager::insertString(const char* string)
{
    const std::string value = string ? string : "";
    std::map<std::string, int>::const_iterator it = string_map_.find(value);
    if (it != string_map_.end()) return it->second;
    const int index = static_cast<int>(strings_.size());
    strings_.push_back(value);
    string_map_[value] = index;
    return index;
}

// exportToBuffer @ 0x8acf420：count + (count+1) 个 offset 字 + 字符串数据；
// 返回写入总字节数，任一段缓冲不足返回 -1。检查时机与 ORIG 逐点一致：
// offsets[i] 写前查 4+4*i<=buffSize（8acf480/8acf491）、数据区起点查
// 4+4*(count+1)<=buffSize（8acf4ba）、每串 memcpy 后查游标<=buffSize
// （8acf528）。ORIG 无 buffer==NULL 检查（UB），此处护栏返回 -1。
int ScriptStringManager::exportToBuffer(unsigned char* buffer, int buffSize) const
{
    const int count = static_cast<int>(strings_.size());
    *reinterpret_cast<int*>(buffer) = count;        // 8acf43d：words[0] 无条件写
    if (buffSize <= 3)
        return -1;                                  // 8acf444：需能写 buffer+4
    int offset = 4 + 4 * count;                     // offsets[0] = 4 + 4*count
    *reinterpret_cast<int*>(buffer + 4) = offset;   // 8acf472
    for (int i = 1; i <= count; ++i)                // offsets[1..count]（含哨兵）
    {
        if (4 + 4 * i > buffSize)                   // 写前检查（首轮须 buffSize>=8）
            return -1;
        offset += static_cast<int>(strings_[i - 1].size());
        *reinterpret_cast<int*>(buffer + 4 + 4 * i) = offset;
    }
    if (4 + 4 * (count + 1) > buffSize)             // 数据区起点检查
        return -1;
    unsigned char* dst = buffer + 4 + 4 * (count + 1);
    int pos = 4 + 4 * (count + 1);
    for (int i = 0; i < count; ++i)                 // 逐串 memcpy 到数据区
    {
        const std::size_t len = strings_[i].size();
        std::memcpy(dst, strings_[i].data(), len);
        dst += len;
        pos += static_cast<int>(len);
        if (pos > buffSize)                         // memcpy 后检查
            return -1;
    }
    return static_cast<int>(dst - buffer);          // 写入总字节数
}

// exportToFile @ 0x8acf2a0：fopen(fileName,"wb") → fwrite count(4B) →
// count>0 时逐串 fwrite 累加 offset 字（每串后 total+=len）→ fwrite 最终
// total 哨兵 → count>0 时逐串 fwrite 数据（len!=0 才写）→ fclose 后 true；
// 任一 fwrite 失败 → fclose 后 false；fopen 失败 → false（ORIG 无 fclose）。
// 布局与 exportToBuffer 完全一致。ORIG 无 fileName==NULL 检查（UB），
// 此处护栏返回 false。
bool ScriptStringManager::exportToFile(const char* fileName) const
{
    if (fileName == 0)
        return false;
    std::FILE* f = std::fopen(fileName, "wb");      // 8acf2b8（mode 0x8d0165b）
    if (f == 0)
        return false;
    const int count = static_cast<int>(strings_.size());
    if (std::fwrite(&count, 4, 1, f) != 1)          // 8acf2c8：写 count
    {
        std::fclose(f);
        return false;
    }
    int total = 4 + 4 * count;                      // local_24
    for (int i = 0; i < count; ++i)                 // 逐串写累加 offset 字
    {
        if (std::fwrite(&total, 4, 1, f) != 1)
        {
            std::fclose(f);
            return false;
        }
        total += static_cast<int>(strings_[i].size());
    }
    if (std::fwrite(&total, 4, 1, f) != 1)          // 最终 total 哨兵
    {
        std::fclose(f);
        return false;
    }
    for (int i = 0; i < count; ++i)                 // 逐串写数据（len!=0 才写）
    {
        const std::size_t len = strings_[i].size();
        if (len != 0 && std::fwrite(strings_[i].data(), len, 1, f) != 1)
        {
            std::fclose(f);
            return false;
        }
    }
    std::fclose(f);                                 // 成功路径
    return true;
}
// 构造 @ 0x8acf110：vptr 赋值 + strings_ 三指针清零 + string_map_ header
// 初始化（_M_key_compare 默认、_M_header{red,0,&header,&header}、
// _M_node_count=0）。标准成员初始化即等价。
ScriptStringManager::ScriptStringManager() {}

// 析构 @ 0x8acf5f0（D1）/ @ 0x8acf700（D0=delete 版）：string_map_ 整树
// 释放 + strings_ 各元素释放 + 向量缓冲释放。标准成员析构链即等价。
ScriptStringManager::~ScriptStringManager() {}

// importFromBuffer @ 0x8acf970
bool ScriptStringManager::importFromBuffer(const unsigned char* buffer,
                                           int buffSize)
{
    if (buffer == 0 || buffSize < 4)
        return false;  // 8acf97f-8acf98f

    const unsigned int count = *reinterpret_cast<const unsigned int*>(buffer);

    // 局部偏移向量（ORIG local_38 的 vector<int>，异常路径自动清理）
    std::vector<int> offsets;
    offsets.reserve(count);  // 8acf9bf

    unsigned int pos = 8;                     // local_44c：读取游标
    const unsigned char* src = buffer + 4;    // local_440 / local_448

    if (count > 0)
    {
        // 主循环：读入 offsets[0..count-1]（buffer+4 起每 4 字节一个）
        int off = *reinterpret_cast<const int*>(buffer + 4);
        unsigned int i = 0;
        for (;;)
        {
            offsets.push_back(off);
            ++i;
            if (count <= i)
            {
                pos += 4;
                break;  // 读满 count 个 → LAB_08acfa69
            }
            if (buffSize < (int)pos + 4)
                return false;  // 8acfc10
            off = *reinterpret_cast<const int*>(buffer + pos);
            pos += 4;
        }
        // LAB_08acfa69：补读 offsets[count]（buffer+4*count+4）
        if (buffSize < (int)pos)  // 8acfa77：条目区结束边界
            return false;
        offsets.push_back(*reinterpret_cast<const int*>(buffer + (pos - 4)));
        src = buffer + (pos - 4);  // = buffer + 4*count + 4（循环开头 local_440）
    }
    else
    {
        // count==0：直接到 LAB_08acfa69，读 buffer+4 作 offsets[0]
        if (buffSize < 8)  // 8acfa77：local_44c(=8) <= buffSize
            return false;
        offsets.push_back(*reinterpret_cast<const int*>(buffer + 4));
        src = buffer + 4;
    }

    strings_.reserve(count);  // 8acfabb

    if (count > 0)
    {
        // 字符串区从 src+4 = buffer+4*count+8 起（export 写入起点一致）
        const unsigned char* ptr = src + 4;
        unsigned int pos2 = pos;
        char temp[1024];  // ORIG local_438[1023] + local_39

        for (unsigned int i = 1; i <= count; ++i)
        {
            const int len = offsets[i] - offsets[i - 1];
            if (len < 0)
                return false;  // 护栏：负长度（ORIG 无此检查，UB）
            if (len <= 0x3ff)
            {
                // 短字符串：完整拷贝 + NUL
                if (buffSize < (int)pos2 + len)  // 8acfb20
                    return false;
                std::memcpy(temp, ptr, len);
                temp[len] = '\0';
            }
            else
            {
                // 长字符串：原版仅拷前 1023 字节（栈缓冲截断）
                if (buffSize < (int)pos2 + len)  // 8acfbfa
                    return false;
                std::memcpy(temp, ptr, 1023);
                temp[1023] = '\0';
            }
            ptr += len;
            pos2 += len;
            strings_.push_back(std::string(temp));  // 8acfb4e/8acfb53 构造并入列
        }
        if (buffSize < (int)pos2)  // 8acfc66：文件总大小边界
            return false;
    }
    return true;  // 8acfc71
}

// importFromFile @ 0x8acfd00
bool ScriptStringManager::importFromFile(const char* fileName)
{
    std::FILE* f = std::fopen(fileName, "rb");  // 8acfd1d（mode "rb"）
    if (f == 0)
        return false;
    if (std::fseek(f, 0, SEEK_END) != 0)        // 8acfd46
    {
        std::fclose(f);
        return false;
    }
    {
        long size = std::ftell(f);              // 8acfd51
        if (size < 0)
        {
            std::fclose(f);
            return false;
        }
        if (std::fseek(f, 0, SEEK_SET) != 0)    // 8acfd76
        {
            std::fclose(f);
            return false;
        }
        {
            unsigned char* buf = new unsigned char[size];  // 8acfd7b _Znaj
            if (buf == 0)
            {
                std::fclose(f);
                return false;
            }
            std::fread(buf, size, 1, f);        // 8acfd99（ORIG 忽略返回值）
            std::fclose(f);                     // 8acfda1
            importFromBuffer(buf, (int)size);   // 8acfdb4（ORIG 忽略返回值）
            delete[] buf;                       // 8acfdbc _ZdaPv
            return true;                        // 8acfdc1：无条件 true
        }
    }
}
