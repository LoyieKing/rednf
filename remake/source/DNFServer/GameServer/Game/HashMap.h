#ifndef DNF_GAME_HASH_MAP_H_
#define DNF_GAME_HASH_MAP_H_

// ============================================================================
// df_game_r 还原 —— StringUnorderedMap / CharStringHashTrait
// ORIG: StringUnorderedMap<HashTrait, ValueType> 内部使用 std::_Hashtable，
// key 为 const char*，value 为 ValueEx<ValueType>。
// ============================================================================

#include <unordered_map>
#include <string>

struct CharStringHashTrait
{
    struct HashKey
    {
        size_t operator()(const char* s) const
        {
            size_t h = 0;
            while (*s) h = h * 131 + *s++;
            return h;
        }
    };

    struct EqualKey
    {
        bool operator()(const char* a, const char* b) const
        {
            return strcmp(a, b) == 0;
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
        ValueEx() : m_value() {}
        ValueEx(const ValueEx& other) : m_value(other.m_value) {}
        ~ValueEx() {}

        ValueEx& operator=(const ValueEx& other)
        {
            m_value = other.m_value;
            return *this;
        }

        ValueType m_value;
    };

    typedef std::unordered_map<const char*, ValueEx,
                               typename HashTrait::HashKey,
                               typename HashTrait::EqualKey> MapType;

    StringUnorderedMap() {}
    ~StringUnorderedMap() {}

    void clear() { m_map.clear(); }

    void insert(const char* key, const ValueType& value)
    {
        ValueEx v;
        v.m_value = value;
        m_map[key] = v;
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
