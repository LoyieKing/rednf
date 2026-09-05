#ifndef DNF_GAME_HASH_MAP_H_
#define DNF_GAME_HASH_MAP_H_

// ============================================================================
// df_game_r 还原 —— StringUnorderedMap / CharStringHashTrait
// ORIG: StringUnorderedMap<HashTrait, ValueType> 内部使用 std::_Hashtable，
// key 为 const char*，value 为 ValueEx<ValueType>。
// ValueEx<Ss> ORIG 实测（nm W，objdump）：C1 0x8080c9c（CharString::C1 @+0x00、
//   Ss::C1 @+0x04）、D1 0x8080ce2（Ss::D1 @+0x04 → CharString::D1）、
//   C1ERKS2_ 0x80817a8（两成员依次拷贝构造）⇒ 布局 {CharString m_key@0;
//   ValueType m_value@0x04}。
// ============================================================================

#include <unordered_map>
#include <string>
#include "StringTypes.h"   // CharString
struct CharStringHashTrait
{
    struct HashKey
    {
        // ORIG W 0x080802a6：h = (h<<6)+(h<<16)-h+c（即 h*65599+c，字符符号扩展）。
        size_t operator()(const char* s) const
        {
            size_t h = 0;
            char c;
            while ((c = *s++) != '\0')
            {
                h = (h << 6) + (h << 16) - h + c;
            }
            return h;
        }
    };

    struct EqualKey
    {
        bool operator()(const char* a, const char* b) const
        {
            if (a == b)
                return true;
            if (strcmp(a, b) == 0)
                return true;
            return false;
        }
    };
};

template <typename HashTrait, typename ValueType>
class StringUnorderedMap
{
public:
    class ValueEx
    {
    public:
        ValueEx() {}                              // ORIG C1：m_key CharString C1 + m_value C1
        ValueEx(const ValueEx& other)             // ORIG C1ERKS2_：两成员拷贝构造
            : m_key(other.m_key), m_value(other.m_value) {}

        ValueEx& operator=(const ValueEx& other)
        {
            m_key = other.m_key;
            m_value = other.m_value;
            return *this;
        }

        CharString m_key;    // +0x00（ORIG D1 逆序析构 string→CharString 实证）
        ValueType m_value;   // +0x04
    };

    typedef std::unordered_map<const char*, ValueEx,
                               typename HashTrait::HashKey,
                               typename HashTrait::EqualKey> MapType;

    StringUnorderedMap() {}
    ~StringUnorderedMap() {}

    void clear() { m_map.clear(); }

    void insert(const char* key, const ValueType& value)
    {
        // ORIG：find 命中则保持原值不覆盖；未命中才构造插入。
        typename MapType::iterator it = m_map.find(key);
        if (it != m_map.end())
            return;
        ValueEx v;
        v.m_key = key;
        v.m_value = value;
        m_map.insert(std::make_pair(key, v));
    }

    ValueType* get(const char* key)
    {
        typename MapType::iterator it = m_map.find(key);
        if (it != m_map.end())
            return &it->second.m_value;
        return 0;
    }

    const ValueType* get(const char* key) const
    {
        typename MapType::const_iterator it = m_map.find(key);
        if (it != m_map.end())
            return &it->second.m_value;
        return 0;
    }

    typename MapType::iterator begin() { return m_map.begin(); }
    typename MapType::iterator end() { return m_map.end(); }

private:
    MapType m_map;
};

#endif  // DNF_GAME_HASH_MAP_H_
