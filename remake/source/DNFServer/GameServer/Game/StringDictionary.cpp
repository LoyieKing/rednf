// ============================================================================
// df_game_r StringDictionary（G2 字符串表批次 A）
// ----------------------------------------------------------------------------
// 实现 StringDictionary 全部 10 个 ORIG 强符号（T），语义逐函数对照
// docs/class_func_reports/StringDictionary/*.
//
// 关键事实：
//   * sizeof(StringDictionary) == 0x424（loadStringFile 中 operator_new(0x424)）。
//   * addString：静态 kBufSize = 0x100（0x8e2ca64）。块管理：m_curRemain 不足时
//     分配 max(need, kBufSize) 字节新块并入 blocks_，指针存入 m_curPtr；否则在
//     当前块内划 need 字节。返回首部指针后 memcpy。
//   * getStringHashValue：static，0x8ab21f8；hash = hash*0x10000 + hash*0x40 - hash
//     + (signed char)c，即 hash*65599 + signed char（每个非'\0'字节）。
//   * StringEntity::operator< 仅比较 hash（0x8ab2842）。
//   * freeze：sort(entities_)（默认 operator<）；对桶 i（0..0xff）用合成实体
//     {hash=i<<24,key=0,value=0} 经 lower_bound 定位桶起点写 buckets_[i]；
//     最后 buckets_[0x100] = entities_.end()。
//   * get：hash=hash(key)，桶 = hash>>24；对 [buckets_[桶], buckets_[桶+1])
//     equal_range 后线性 strcmp 命中即返回 value。
// ============================================================================

#include "StringDictionary.h"

#include <algorithm>
#include <cctype>
#include <cstring>

namespace
{
// ORIG getStringHashValue 为文件局部 static（0x8ab21f8，nm t）。
//   hash = 0; for each non-null byte c: hash = hash*65599 + (signed char)c;
//   return hash.
unsigned int getStringHashValue(const char* s)
{
    unsigned int hash = 0;
    for (; *s != '\0'; ++s)
    {
        // hash*0x40 + hash*0x10000 - hash + c = hash*0x40ff + c = hash*65599 + c
        hash = (hash << 6) + (hash << 16) - hash + (unsigned int)(signed char)(*s);
    }
    return hash;
}
}  // namespace

// ----------------------------------------------------------------------------
// StringDictionary::StringDictionary()（0x8ab2240）
//   vector<unsigned char*> ctor；m_curPtr=0；m_curRemain=0；
//   vector<StringEntity> ctor；0x100 个桶迭代器默认构造。
//   注意：ORIG 不构造 buckets_[0x100]（末端迭代器由 freeze 写入），但为
//   与额外默认构造一致无 ABI 影响；这里显式将 0x101 个迭代器 value-init。
// ----------------------------------------------------------------------------
StringDictionary::StringDictionary()
    : blocks_(),
      m_curPtr(0),
      m_curRemain(0),
      entities_()
{
    for (int i = 0; i < 0x101; ++i)
    {
        buckets_[i] = std::vector<StringEntity>::iterator();
    }
}

// ----------------------------------------------------------------------------
// StringDictionary::~StringDictionary()（0x8ab22be）
//   遍历 blocks_，delete[] 每个非空块；随后 ~vector<StringEntity>、~vector<uchar*>。
// ----------------------------------------------------------------------------
StringDictionary::~StringDictionary()
{
    for (std::vector<unsigned char*>::iterator it = blocks_.begin();
         it != blocks_.end(); ++it)
    {
        if (*it != 0)
        {
            delete[] *it;
        }
    }
    entities_.clear();
    blocks_.clear();
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
const char* StringDictionary::addString(const char* s)
{
    // ORIG 函数级 static（0x8e2ca64 = 0x10000；块分配默认容量）。
    static const int kBufSize = 0x10000;
    const size_t len = strlen(s);
    const int need = static_cast<int>(len) + 1;

    unsigned char* ret = 0;
    if (m_curRemain < need)
    {
        int cap = (need > kBufSize) ? need : kBufSize;
        unsigned char* ptr = new unsigned char[cap];
        blocks_.push_back(ptr);
        m_curPtr = reinterpret_cast<int>(ptr) + need;
        m_curRemain = cap - need;
        ret = ptr;
    }
    else
    {
        ret = reinterpret_cast<unsigned char*>(m_curPtr);
        m_curPtr += need;
        m_curRemain -= need;
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
void StringDictionary::add(const char* key, const char* value)
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
void StringDictionary::freeze()
{
    std::sort(entities_.begin(), entities_.end());

    for (int i = 0; i < 0x100; ++i)
    {
        StringEntity probe;
        probe.hash = static_cast<unsigned int>(i) << 24;
        probe.key = 0;
        probe.value = 0;
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
const char* StringDictionary::get(const char* key) const
{
    const unsigned int h = getStringHashValue(key);
    const unsigned int bucket = h >> 24;

    StringEntity probe;
    probe.hash = h;
    probe.key = key;
    probe.value = 0;

    std::pair<std::vector<StringEntity>::iterator,
              std::vector<StringEntity>::iterator> range =
        std::equal_range(buckets_[bucket], buckets_[bucket + 1], probe);

    for (std::vector<StringEntity>::iterator it = range.first;
         it != range.second; ++it)
    {
        if (std::strcmp(it->key, key) == 0)
        {
            return it->value;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// getCount() const（0x8ab2612）
//   return entities_.size();
// ----------------------------------------------------------------------------
int StringDictionary::getCount() const
{
    return static_cast<int>(entities_.size());
}

// ----------------------------------------------------------------------------
// getKey(int index) const（0x8ab2628）
//   越界（<0 或 >= size）返回 0，否则 entities_[index].key。
// ----------------------------------------------------------------------------
const char* StringDictionary::getKey(int index) const
{
    if (index < 0 || index >= static_cast<int>(entities_.size()))
    {
        return 0;
    }
    return entities_[index].key;
}

// ----------------------------------------------------------------------------
// getValue(int index) const（0x8ab2668）
//   越界返回 0，否则 entities_[index].value。
// ----------------------------------------------------------------------------
const char* StringDictionary::getValue(int index) const
{
    if (index < 0 || index >= static_cast<int>(entities_.size()))
    {
        return 0;
    }
    return entities_[index].value;
}

// ----------------------------------------------------------------------------
// setValue(int index, const char* value)（0x8ab26a8）
//   越界跳过；否则重新 addString(key) 与 addString(value)（保持存储内一致性），
//   entities_[index] = { getStringHashValue(newKey), newKey, newValue }。
// ----------------------------------------------------------------------------
void StringDictionary::setValue(int index, const char* value)
{
    if (index < 0 || index >= static_cast<int>(entities_.size()))
    {
        return;
    }

    const char* newKey = addString(entities_[index].key);
    const char* newValue = addString(value);

    StringEntity e;
    e.hash = getStringHashValue(newKey);
    e.key = newKey;
    e.value = newValue;
    entities_[index] = e;
}
