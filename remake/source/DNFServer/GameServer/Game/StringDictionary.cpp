// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StringDictionary/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "StringDictionary.h"

#include <algorithm>
#include <cctype>
#include <cstring>

// ORIG getStringHashValue 为文件局部 static（0x8ab21f8，nm t）。
//   文件级 `static`（非匿名 namespace）：mangled 恰为 _ZL18getStringHashValuePKc
//   与 ORIG 一致（匿名 namespace 会错发 _ZN12_GLOBAL__N_1…，Main 修正）。
//   hash = 0; for each non-null byte c: hash = hash*65599 + (signed char)c;
//   return hash.
static unsigned int getStringHashValue(const char* s)
{
    // [b28d-48] ORIG 0x08ab21f8：具名 char c 槽(-0x1) + `c=*s++` 先读后增
    // （@0x05 movzbl → @0x06 mov %al,-0x1 → @0x07 addl $1,0x8(%ebp) →
    //  @0x08 cmpb $0,-0x1 → je），for 形被 4.4 折成 ++s 后置。
    unsigned int hash = 0;
    char c;
    while (true)
    {
        c = *s++;
        if (c == '\0')
            break;
        // hash*0x40 + hash*0x10000 - hash + c = hash*0x40ff + c = hash*65599 + c
        hash = (hash << 6) + (hash << 16) - hash + (unsigned int)(signed char)c;
    }
    return hash;
}

// ----------------------------------------------------------------------------
// StringDictionary::StringDictionary()（0x8ab2240）
//   vector<unsigned char*> ctor；m_curPtr=0；m_curRemain=0；
//   vector<StringEntity> ctor；0x100 个桶迭代器默认构造。
//   注意：ORIG 不构造 buckets_[0x100]（末端迭代器由 freeze 写入），但为
//   与额外默认构造一致无 ABI 影响；这里显式将 0x101 个迭代器 value-init。
// ----------------------------------------------------------------------------
#include "StringDictionary/StringDictionary.inc"
    : blocks_(),
      m_curPtr(0),
      m_curRemain(0),
      entities_()
{
    // [b28c-11] AE：buckets_[0x101] 改为隐式数组构造（删显式 for）⇒ 4.4 复现
    // ORIG 倒计数构造循环（P151：mov $0x100,%esi; sub; cmp $-1; setne/test/jne），
    // 43/43 条全等（.o 重定位显示差）。
}

// ----------------------------------------------------------------------------
// StringDictionary::~StringDictionary()（0x8ab22be）
//   遍历 blocks_，delete[] 每个非空块；随后 ~vector<StringEntity>、~vector<uchar*>。
// ----------------------------------------------------------------------------
#include "StringDictionary/dtor_StringDictionary.inc"
{
    // [b28d-48] N93/N91 封顶（NDIFF 73，84/73 条）：ORIG 无迭代器默认构造
    // （begin/end 直接调）、4.4 多 setne 物化与默认构造槽；P50 形实测反劣已回退。
    // [b28d-48] P50 先声明后赋值形实测反劣（73→84→93 条），回退 for 形；
    // `*it != 0` 改 `*it` 消 setne 物化（ORIG 0x8ab22be 为 test+je 直判）。
    // [L6 round3 · P141 实测] 删显式 `if (*it)` 包裹：ORIG 只有 `delete[]` 自带的
    // 一次空指针守卫（`test %eax,%eax; je`），我方显式 if 使其物化为
    // `test; setne %al; test %al,%al; je`（OURS-only setne/test×2/je + 3 call，+11 条）。
    // [b28e-S4 回滚] ORIG 0x08ab22c6~0x08ab233c 虽为「begin/end 各一次 + 后缀
    //   it++」，但 for-init 双声明 + it++ 实测 76→**80**（N3 编译读数）：
    //   4.4 在 for-init 双声明下于尾部多发 `~vector<StringEntity>`/`~vector<uchar*>`
    //   两次析构调用（+4 条，ORIG 无）⇒ 该形比原形更远。回滚到 for(cond 内
    //   blocks_.end(), ++it) 形（76 vs ORIG 73 为既有封顶态）。
    // [P6S4 2026-09-11 负结论 · 按 p167fix 修正口径] ①ORIG 0x08ab22ed 只调一次
    // end()（槽 -0x14，循环外提升）⇒ 试 end() 具名缓存形：4.4 反在 EH 清理块多
    // 发两次 ~vector 析构 call，blocks 5→7 ⇒ 回滚。②ORIG 循环尾用 `ppEi`（后置，
    // 带返回临时槽 -0xc）⇒ 试 `it++`：76→80 条（4.4 postfix 临时 + EH 清理块多
    // 2 次析构 call）⇒ 回滚；③`it` 循环外声明 + `++it` 实测与原 for-init 形逐条
    // 相同（76/blocks 5）⇒ 维持原形。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N3/N91/N93）→ 试=①删尾部
    // `entities_.clear(); blocks_.clear();`（ORIG 0x08ab22be 循环外仅两次成员自动
    // 析构：0x08ab234d/0x08ab2361 `add $0x14` 的 `~vector<StringEntity>` 与
    // 0x08ab2389 的 `~vector<uchar*>`；p167 EXTRA `vector<...>::clear()`×2 直证
    // 我方多写 clear）②`++it`→`it++`（ORIG 0x08ab2320 调 `_ppEi`（后置，带返回
    // 临时槽 -0xc 与 `movl $0x0,0x8(%esp)`））③end() 提升为具名 `last`（ORIG
    // 0x08ab22ed 仅一次 end()，槽 -0x14，循环外）⇒ **推翻**：n1_v=AE（73=73 条,
    // gap=0）；TU 四数 49/125/0/1/34 → 49/127/0/0/29（diff 1→0、missing −5）。
    // 关键：P6S4 三项负结论均在**带 clear 行**的旧形上测得（尾部 EH 清理块含
    // clear 临时）；删 clear 后同轴翻转 ⇒ 三项限制不适用于本形。
    std::vector<unsigned char*>::iterator it = blocks_.begin();
    std::vector<unsigned char*>::iterator last = blocks_.end();
    for (; it != last; it++)
    {
        delete[] *it;
    }
}

// ----------------------------------------------------------------------------
// addString(const char*)（0x8ab2772）
//   len = strlen(s); need = len + 1;
//   if (m_curRemain < need) {
//       cap = max(need, kBufSize);
//       ptr = new unsigned char[cap];
//       blocks_.push_back(ptr);
//       m_curPtr = (int)ptr + need;
//       m_curRemain = cap - need;
//   } else {
//       ret = m_curPtr; m_curPtr += need; m_curRemain -= need;
//   }
//   memcpy(ret, s, need);
//   return (const char*)ret;
// ----------------------------------------------------------------------------
#include "StringDictionary/addString.inc"
{
    // [b28c-11] AE：m_curPtr 由 int 改 `unsigned char*`（ORIG `mov -0x1c,%edx`
    // 单段直落，int 形多一条 eax→edx 中转）。68/68 条全等（kBufSize=0x10000 经
    // ORIG rodata 0x8e2ca64 实测确认，头注释 0x100 为笔误）。
    // ORIG 函数级 static（0x8e2ca64 = 0x10000；块分配默认容量）。
    static const int kBufSize = 0x10000;
    const size_t len = strlen(s);
    const int need = static_cast<int>(len) + 1;

    unsigned char* ret;
    if (m_curRemain >= need)
    {
        ret = m_curPtr;
        m_curPtr += need;
        m_curRemain -= need;
    }
    else
    {
        unsigned int cap = std::max(need, kBufSize);
        unsigned char* ptr = new unsigned char[cap];
        blocks_.push_back(ptr);
        m_curPtr = ptr + need;
        m_curRemain = cap - static_cast<unsigned int>(need);
        ret = ptr;
    }

    memcpy(ret, s, need);
    return reinterpret_cast<const char*>(ret);
}

// ----------------------------------------------------------------------------
// add(const char* key, const char* value)（0x8ab2398）
//   keyP = addString(key); valP = addString(value);
//   StringEntity e{getStringHashValue(keyP), keyP, valP};
//   entities_.push_back(e);
// ----------------------------------------------------------------------------
#include "StringDictionary/add.inc"
{
    const char* keyP = addString(key);
    const char* valP = addString(value);

    StringEntity e = {};   // [b22] ORIG add @0x08ab2398 先 3×movl $0 值初始化再逐字段覆写
    e.hash = getStringHashValue(keyP);
    e.key = keyP;
    e.value = valP;
    entities_.push_back(e);
}

// ----------------------------------------------------------------------------
// freeze()（0x8ab240e）
//   std::sort(entities_.begin(), entities_.end());   // 默认 operator<（仅 hash）
//   for (i = 0; i < 0x100; ++i) {
//       StringEntity probe; probe.hash = i << 24; probe.key = 0; probe.value = 0;
//       buckets_[i] = std::lower_bound(entities_.begin(), entities_.end(), probe);
//   }
//   buckets_[0x100] = entities_.end();
// ----------------------------------------------------------------------------
#include "StringDictionary/freeze.inc"
{
    // [b28c-11] AE：probe 改 `StringEntity probe = {};`（三字段先清零再写 hash，
    // ORIG 顺序）+ 循环变量 unsigned（setbe 而非 setle，P2）。82/82 条全等。
    std::sort(entities_.begin(), entities_.end());

    for (unsigned int i = 0; i < 0x100; ++i)
    {
        StringEntity probe = {};
        probe.hash = i << 24;
        buckets_[i] = std::lower_bound(entities_.begin(), entities_.end(), probe);
    }
    buckets_[0x100] = entities_.end();
}

// ----------------------------------------------------------------------------
// get(const char* key) const（0x8ab2528）
//   h = getStringHashValue(key); 桶 = h >> 24;
//   pair = equal_range(buckets_[桶], buckets_[桶+1], probe{h,key,0});
//   在 [first, second) 内 strcmp(key) 命中则返回 entity.value，否则 0。
// ----------------------------------------------------------------------------
#include "StringDictionary/get.inc"
{
    // [b28d-48] N86 封顶（NDIFF 47，70/70 条）：equal_range 模板临时槽 +
    // 迭代器转换构造跨版本差（probe 已按 ORIG 改 `{}` 值初始化）。
    const unsigned int h = getStringHashValue(key);
    const unsigned int bucket = h >> 24;

    // [b28d-48] ORIG 0x08ab2528 段：先 3×`movl $0`（{} 值初始化 -0x20/-0x1c/-0x18）
    // 再 `mov -0x10,%eax; mov %eax,-0x20`（probe.hash=h）——逐字段赋值形顺序反。
    // [b28e-S4] ORIG 0x08ab2546~0x08ab255e 只写 probe.hash（3 条 movl $0 清零 +
    //   `mov -0x10,%eax; mov %eax,-0x20`），无 probe.key 写入 ⇒ 删该行。
    StringEntity probe = {};
    probe.hash = h;

    std::pair<std::vector<StringEntity>::iterator,
              std::vector<StringEntity>::iterator> range =
        std::equal_range(buckets_[bucket], buckets_[bucket + 1], probe);

    // [b28e-S4] ORIG 0x08ab2599 `jmp` 直落条件体（equal_range 结果原地用，无
    //   `it = range.first` 拷贝），循环体尾 `local_14 = it++`（后缀
    //   operator++(int)）⇒ 直接迭代 range.first 并用后缀 ++。
    for (; range.first != range.second; range.first++)
    {
        if (std::strcmp(range.first->key, key) == 0)
        {
            return range.first->value;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// getCount() const（0x8ab2612）
//   return entities_.size();
// ----------------------------------------------------------------------------
#include "StringDictionary/getCount.inc"
{
    return static_cast<int>(entities_.size());
}

// ----------------------------------------------------------------------------
// getKey(int index) const（0x8ab2628）
//   越界（<0 或 >= size）返回 0，否则 entities_[index].key。
// ----------------------------------------------------------------------------
#include "StringDictionary/getKey.inc"
{
    return (index >= 0 && index < static_cast<int>(entities_.size()))
               ? entities_[index].key : 0;
}

// ----------------------------------------------------------------------------
// getValue(int index) const（0x8ab2668）
//   越界返回 0，否则 entities_[index].value。
// ----------------------------------------------------------------------------
#include "StringDictionary/getValue.inc"
{
    return (index >= 0 && index < static_cast<int>(entities_.size()))
               ? entities_[index].value : 0;
}

// ----------------------------------------------------------------------------
// setValue(int index, const char* value)（0x8ab26a8）
//   越界跳过；否则重新 addString(key) 与 addString(value)（保持存储内一致性），
//   entities_[index] = { getStringHashValue(newKey), newKey, newValue }。
// ----------------------------------------------------------------------------
#include "StringDictionary/setValue.inc"
{
    // [b28c-11] AE：守卫改正向 `if (index >= 0 && index < size)` 复现 ORIG
    // js/jle/mov$1/jmp/mov$0/test/je 物化链；StringEntity e = {} 三清零先于 hash。
    // 59/59 条全等。
    if (index >= 0 && index < static_cast<int>(entities_.size()))
    {
        const char* newKey = addString(entities_[index].key);
        const char* newValue = addString(value);

        StringEntity e = {};
        e.hash = getStringHashValue(newKey);
        e.key = newKey;
        e.value = newValue;
        entities_[index] = e;
    }
}
