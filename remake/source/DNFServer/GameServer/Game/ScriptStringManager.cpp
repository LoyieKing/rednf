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

#include "ScriptStringManager.h"

#include <cstdio>
#include <cstring>

// 类定义见 ScriptStringManager.h（唯一声明点）。

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
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N7**（44 vs 44 条，p167 CLEAR）。
// 残差 = ①装载寄存器角色互换（ORIG `mov 0x4(%edi),%eax; lea (%eax,%esi,4),%eax`
// vs 我方 %ecx 中转）；②`xor %eax,%eax`（false 常量）提前物化（ORIG 在 epilogue
// 前）。实测旋钮均不翻：纯 register 声明（P231）、const 引用局部、具名 count 中转。
bool ScriptStringManager::getString(int index, std::string* outputString) const
{
    outputString->clear();  // ORIG: _M_mutate(0, length, 0)
    // [b23S3C] ORIG 0x8acf160：正索引无界检查直 assign(start[index])；
    // 负索引仅 (int)size()<index 死检查（js 分离形）。勿加 unsigned 界保护改行为。
    if (index < 0)
    {
        if (static_cast<int>(strings_.size()) < index)
            return false;
    }
    outputString->assign(strings_[index]);  // [b23S3C] begin() 形实测反劣化（+2 out-of-line），保 operator[]
    return true;
}

// getString(int) @ 0x8acf1f0：函数内静态 DEFAULT_STRING（guard 0x9501a00
// 惰性初始化，ORIG 经 _ZNSsC1EPKcRKSaIcE 自 "" 构造，对象 0x9501a08）。
// index<0 或 index>=条数 → &DEFAULT_STRING（8acf221），否则返回
// &strings_[index]（8acf226）。ORIG 返回非常量 string*（DWARF 原型），
// const 成员取成员地址为 const，此处按 const std::string* 返回。
// b28d-solo-ScriptStringManager 判词：**NEAR**（45 vs 45 条，p167 CLEAR）。
// 残差仅 .o 未解析重定位的 call/guard 标签显示差。关键：具名 `result` 单出口形
// 消掉 4.4 的双 epilogue（嵌套 if / 三目恒 53 条），与 ORIG 共享尾块同构。
const std::string* ScriptStringManager::getString(int index) const
{
    // [b28c-21] ORIG 8acf1f0：正极性嵌套 if（js 负索引 → 尾部 DEFAULT），
    // `||` 合并形 4.4 生 setne 物化 + 3 callee-saved（81 vs 45 insn）。
    static const std::string DEFAULT_STRING("");  // ORIG DEFAULT_STRING
    const std::string* result = &DEFAULT_STRING;
    if (index >= 0 && index < static_cast<int>(strings_.size()))
        result = &strings_[index];
    return result;
}

// insertString @ 0x8acf720：字符串表 intern。已存在（map 树中找到，
// 8acf7f5 后 8acf802 分支）→ 返回既有 index；否则 push 到 strings_
// （8acf8f2 _M_insert_aux / 8acf85c 原地拷贝构造）→ map _M_insert_unique
// 插入 {string, 新 index}（8acf8b7）→ 返回新 index（8acf891 计算，
// (finish-start)>>2 - 1）。ORIG"已存在"路径返回树节点内字段（无意义
// 值），本实现统一按 intern 语义返回 index。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N89**（259 vs 196 条；
// p167: 2 MISSING 右值模板实例 + 4 EXTRA）。ORIG 3×`std::string(string)` 临时 +
// out-of-line `~string`（_ZNSsD1Ev×4）；4.4.7 头在 -O2 把 `~basic_string` 内联
// 展开（4× `_M_destroy` + pthread_cancel 段）净 +~60 条。MISSING
// `_M_insert_auxIISsEE…DpOT_`（C++0x 右值版）来自 ORIG 归档 4.4 libstdc++；
// gnu++0x 实测更差（282 条 / TU diff 12 vs 10）⇒ 不可翻。
int ScriptStringManager::insertString(const char* string)
{
    // ORIG 0x08acf720：**两次**独立构造临时 std::string(string)（8acf73d find 用、
    // 8acf89b insert 用），无 string==NULL 护栏（P171）；index 在 push_back
    // **之后**按 (finish-start)>>2 - 1 计算（8acf891）。
    std::map<std::string, int>::const_iterator it =
        string_map_.find(std::string(string));      // 8acf73d
    if (it != string_map_.end())
        return it->second;                          // 8acf807
    strings_.push_back(std::string(string));        // 8acf85c 原地 / 8acf8f2 扩容
    const int index = static_cast<int>(strings_.size()) - 1;  // 8acf891
    string_map_.insert(
        std::map<std::string, int>::value_type(std::string(string), index));  // 8acf8b7
    return index;
}

// exportToBuffer @ 0x8acf420：count + (count+1) 个 offset 字 + 字符串数据；
// 返回写入总字节数，任一段缓冲不足返回 -1。检查时机与 ORIG 逐点一致：
// offsets[i] 写前查 4+4*i<=buffSize（8acf480/8acf491）、数据区前查
// 总大小 offset<=buffSize（8acf4bd）、每串 memcpy 后查游标<=buffSize
// （8acf528）。ORIG 无 buffer==NULL 检查（UB），此处护栏返回 -1。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N82**（92 vs 102 条，
// p167 CLEAR）。字节游标 `p`/`pos` 具名形已复现 ORIG 的 strength-reduced 形
// （`mov -0x8(%edi,%eax,1)`，非 scaled index）+ `buffSize>7` 入口门（P171：
// 原码 buffSize<=7 时不越界写 buffer+8）；残差 = 槽序（ORIG
// count@-0x1c/p@-0x20/pos@-0x24 vs 我方 p@-0x1c/count@-0x20/finish@-0x24，
// 4.4 槽按首次使用分配、声明序不可翻 = N98）＋ %esi/%edi 角色互换。
int ScriptStringManager::exportToBuffer(unsigned char* buffer, int buffSize) const
{
    const int count = static_cast<int>(strings_.size());
    *reinterpret_cast<int*>(buffer) = count;        // 8acf43d：words[0] 无条件写
    if (buffSize <= 3)
        return -1;                                  // 8acf444：需能写 buffer+4
    int offset = 4 + 4 * count;                     // offsets[0] = 4 + 4*count
    // ORIG 8acf44e-8acf4b8：字节游标 `p`（写指针）+ `pos`（游标字节数）两具名局部
    // 跨循环使用 ⇒ 4.1 分别 spill 到 -0x20/-0x24；do-while 形（入口 jle + 尾部
    // 回跳，无旋转）。哨兵写用 break 前的 p（Ghidra C `*local_24 = iVar2`）。
    int* p = reinterpret_cast<int*>(buffer + 4);
    int pos = 8;
    if (count > 0)
    {
        *p = offset;                                // 8acf472
        // ORIG 8acf47c `cmpl $0x7; jg`：buffSize<=7 时不进循环、直接 -1
        // （照抄原码不越界写 buffer+8，P171）。
        if (buffSize > 7)
        {
            int i = 0;
            do
            {
                p = reinterpret_cast<int*>(buffer + pos);
                ++i;
                offset += static_cast<int>(strings_[i - 1].size());  // 8acf4a7
                // ORIG 8acf4ad `jl`（i<count 落 *p 写点）⇒ 正极性包裹（P222）。
                if (i < count)
                {
                    *p = offset;                    // 8acf488
                    pos += 4;
                }
                else
                {
                    pos += 4;                       // 8acf4af：读满 → 写哨兵
                    goto after_offsets;
                }
            } while (pos <= buffSize);              // 8acf491：越界 → -1
        }
        return -1;  // 8acf482（buffSize<=7）/ 8acf52d（循环越界）
    }
after_offsets:
    *p = offset;                                    // 8acf4b8：哨兵 offsets[count]
    if (pos > buffSize)                             // 8acf4c5
        return -1;
    unsigned char* dst = reinterpret_cast<unsigned char*>(p + 1);
    for (int i = 0; i < static_cast<int>(strings_.size()); ++i)
    {
        const std::size_t len = strings_[i].size();
        std::memcpy(dst, strings_[i].data(), len);  // 8acf515
        dst += len;
        pos += static_cast<int>(len);
        if (pos > buffSize)                         // 8acf528：memcpy 后检查
            return -1;
    }
    return static_cast<int>(dst - buffer);          // 写入总字节数
}

// exportToFile @ 0x8acf2a0：fopen(fileName,"wb") → fwrite count(4B) →
// count>0 时逐串 fwrite 累加 offset 字（每串后 total+=len）→ fwrite 最终
// total 哨兵 → count>0 时逐串 fwrite 数据（len!=0 才写）→ fclose 后 true；
// 任一 fwrite 失败 → fclose 后 false；fopen 失败 → false（ORIG 无 fclose）。
// 布局与 exportToBuffer 完全一致。ORIG 无 fileName==NULL 检查（P171 照抄）。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N7**（112 vs 115 条，
// p167 CLEAR）。已复现：无 NULL 护栏、count/offset 预清零（P198①）、正向 if +
// 共享 fail 尾（P222）、第二循环 do-while 消 4.4 旋转（P283）。残差 = 第三段
// 循环 this/file/finish 的寄存器角色（ORIG file spill@-0x2c、this 保活 %ebx；
// 我方 this spill@-0x2c、file 保活 %esi）。
bool ScriptStringManager::exportToFile(const char* fileName) const
{
    // ORIG 8acf2af/8acf2b6：两个具名 int 局部在 fopen 前预清零（P198①），
    // 且**无** fileName==NULL 护栏（ORIG 直接 fopen，P171 照抄不写）。
    int count = 0;
    int offset = 0;
    std::FILE* file = std::fopen(fileName, "wb");   // 8acf2c8（mode "wb"）
    if (file == 0)
        return false;
    count = static_cast<int>(strings_.size());      // 8acf2d5
    // ORIG 8acf303 `je +0x78`：正向 if（成功路径跳 fail 块），失败路径
    // fall-through 到函数尾的共享 fclose+return false（Ghidra C 同形）。
    if (std::fwrite(&count, 4, 1, file) == 1)       // 8acf2fb：写 count
    {
        offset = 4 + 4 * static_cast<int>(strings_.size());  // 8acf321（local_24）
        // ORIG 8acf335-8acf375：入口 `jle` + 尾部回跳（4.1 while 未旋转）
        // ⇒ 写 do-while + 前置空检查复现（4.4 对 while 恒旋转）。
        if (0 < static_cast<int>(strings_.size()))
        {
            int i = 0;
            do
            {
                if (std::fwrite(&offset, 4, 1, file) != 1)  // 8acf352：写累加 offset
                    goto fail;
                offset += static_cast<int>(strings_[i].size());  // 8acf368
                ++i;
            } while (i < static_cast<int>(strings_.size()));
        }
        if (std::fwrite(&offset, 4, 1, file) == 1)  // 8acf391：最终 total 哨兵
        {
            for (int i = 0; i < static_cast<int>(strings_.size()); ++i)
            {
                const std::size_t len = strings_[i].size();
                if (len != 0 && std::fwrite(strings_[i].data(), len, 1, file) != 1)
                    goto fail;
            }
            std::fclose(file);                      // 8acf3fe：成功路径
            return true;
        }
    }
fail:
    std::fclose(file);                              // 8acf305：共享失败出口
    return false;
}
// 构造 @ 0x8acf110：vptr 赋值 + strings_ 三指针清零 + string_map_ header
// 初始化（_M_key_compare 默认、_M_header{red,0,&header,&header}、
// _M_node_count=0）。标准成员初始化即等价。
ScriptStringManager::ScriptStringManager() {}

// 析构 @ 0x8acf5f0（D1）/ @ 0x8acf700（D0=delete 版）：string_map_ 整树
// 释放 + strings_ 各元素释放 + 向量缓冲释放。标准成员析构链即等价。
ScriptStringManager::~ScriptStringManager() {}

// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N82**（238 vs 242 条；
// p167: 3 MISSING 右值模板实例 + 3 EXTRA）。已按 Ghidra C 逐句复现：双独立守卫、
// offset 循环尾读 + 哨兵 goto、memcpy 后查越界（P171）、删 len<0 护栏。
// 残差 = 槽整体深 4-8B（ORIG push_back 内联临时槽 -0x2c/-0x28，4.4 无）＋
// %esi/%ebx 角色互换；MISSING 为 C++0x 右值 `_M_insert_aux`（同 insertString）。
bool ScriptStringManager::importFromBuffer(const unsigned char* buffer,
                                           int buffSize)
{
    // ORIG 8acf97f-8acf992：两个**独立**守卫（buffer==NULL 早退 8acf984；
    // count 先读入槽 -0x440 再判 `buffSize > 3` 正向 8acf98c/992）。
    // [L4-r2 判据⑩ 一次性试验已回退] 试把 count 下移到 offsets 之后 ⇒ vector
    // ctor 被提到 NULL 守卫之前（多 3 条 movl $0）且 count 槽仍 -0x43c ⇒ 反劣，
    // 已还原。槽差归 N82/N18a（寄存器角色 %esi↔%ebx 同族）。
    unsigned int count;
    if (buffer == 0
        || (count = *reinterpret_cast<const unsigned int*>(buffer), buffSize < 4))
        return false;

    // 局部偏移向量（ORIG local_38 的 vector<int>，异常路径自动清理）
    std::vector<int> offsets;
    offsets.reserve(count);  // 8acf9bf

    const unsigned char* src = buffer + 4;    // local_440 / local_448
    unsigned int pos = 8;                     // local_44c：读取游标

    if (count != 0)
    {
        // ORIG 8acf9e0-8acfa60：先取 offsets[0]，循环尾读下一个；读满 count 个
        // 后 `pos += 4` 跳 LAB_08acfa69 补哨兵（Ghidra C 逐句同形）。
        unsigned int off = *reinterpret_cast<const unsigned int*>(buffer + 4);
        if (buffSize > 7)
        {
            unsigned int i = 0;
            do
            {
                src = buffer + pos;               // 8acf9f1：循环头
                offsets.push_back(off);           // 8acfa0b：内联原地/扩容
                if (count <= i + 1)
                {
                    pos += 4;                     // 8acfa60
                    goto after_offsets;
                }
                ++i;                              // 8acfa2b
                off = *reinterpret_cast<const unsigned int*>(buffer + pos);
                pos += 4;
            } while (pos <= static_cast<unsigned int>(buffSize));
        }
        return false;  // 8acfc10：buffSize 不足
    }
after_offsets:
    // LAB_08acfa69：哨兵 offsets[count] = *(uint*)src（src 停在最后一个条目）
    if (pos > static_cast<unsigned int>(buffSize))  // 8acfa77
        return false;
    offsets.push_back(*reinterpret_cast<const unsigned int*>(src));
    strings_.reserve(count);  // 8acfabb

    if (count != 0)
    {
        // 字符串区从 src+4 = buffer+4*count+8 起（export 写入起点一致）。
        // ORIG 8acfb20/8acfbfa：**先 memcpy 后查越界**（P171 照抄原码顺序），
        // 且无 len<0 护栏（ORIG 直接 `if ((int)__n < 0x400)`）。
        unsigned int i = 0;
        src += 4;  // local_448
        do
        {
            ++i;
            const int len = offsets[i] - offsets[i - 1];
            char temp[1024];  // ORIG local_438[1023] + local_39
            if (len < 0x400)
            {
                std::memcpy(temp, src, len);
                if (buffSize < static_cast<int>(pos + len))  // 8acfb20
                    return false;
                temp[len] = '\0';
            }
            else
            {
                // 长字符串：原版仅拷前 1023 字节（栈缓冲截断）
                std::memcpy(temp, src, 1023);
                if (buffSize < static_cast<int>(pos + len))  // 8acfbfa
                    return false;
                temp[1023] = '\0';
            }
            src += len;
            pos += len;
            strings_.push_back(std::string(temp));  // 8acfb4e/8acfb53 构造并入列
        } while (i < count);
        if (buffSize < static_cast<int>(pos))  // 8acfc66：文件总大小边界
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
