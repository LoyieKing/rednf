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
        // ORIG 25 insns 形（0x080802a6）：`movl $0,-0x8` 后**先跳测试块**，测试块为
        // `mov 0xc(%ebp),%eax; movzbl (%eax),%eax; mov %al,-0x1(%ebp);
        //  addl $1,0xc(%ebp); cmpb $0,-0x1(%ebp); je exit` ⇒ 赋值嵌入条件的
        //  `while ((c = *s++) != 0)` 形（**测试在环头**、体内无 setne）。4.4 对 while
        //  条件形会旋转成尾部 `setne+test` 双判（+3 insns），故改写为等价的
        //  `while (true) { c = *s++; if (c == 0) break; … }` 顶部测试形，
        //  与 ORIG 逐条同序：取字符→存槽→指针前移→cmpb→je 出口→体→jmp 环头。
        size_t operator()(const char* s) const
        {
            size_t h = 0;
            char c;
            while (true)
            {
                c = *s++;
                if (c == 0)
                {
                    break;
                }
                h = (h << 6) + (h << 16) - h + c;
            }
            return h;
        }
    };

    struct EqualKey
    {
        bool operator()(const char* a, const char* b) const
        {
            if (a == b || strcmp(a, b) == 0)
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

    // ORIG W 0x080809fe（int 实例）：返回 **pair<iterator,bool>**（prologue 首参即
    // 隐藏 sret 指针，epilogue `ret $0x4`），流程 ①_Hashtable::find ②_Hashtable::end
    // ③__detail::operator!= 命中则 ④`std::make_pair(it,false)`（mangled
    // _ZSt9make_pairIRNSt8__detail19_Hashtable_iterator…Eb…）直接返回；未命中则
    // ⑤ValueEx C1 ⑥CharString::operator=(char const*) ⑦m_value 解引用赋值
    // ⑧CharString::c_str() 作键 ⑨pair<const char*,ValueEx> 转换构造
    // ⑩_Hashtable::insert 结果 sret 直落本函数返回槽。旧重建返回 void ⇒ 调用方
    // （CNRDScriptData::import 五处）少传隐藏 sret 参、少 `sub $0x4,%esp`，
    // 每站 −2 insns（该函数 23 insns 缺口即全在此）。
    typedef std::pair<typename MapType::iterator, bool> InsertResult;

    InsertResult insert(const char* key, const ValueType& value)
    {
        typename MapType::iterator it = m_map.find(key);
        if (it != m_map.end())
            return std::make_pair(it, false);
        ValueEx v;
        v.m_key = key;
        v.m_value = value;
        // ORIG 侧该处的 pair 构造调用为 `_ZNSt4pairIKPKc...ValueExEEC1IS1_RS6_EEOT_OT0_`
        // （converting ctor，实参 = char const* + **ValueEx 左值引用**），且**无**
        // `std::make_pair` 调用、也无 pair 临时量的额外析构 ⇒ 源形是直接用
        // value_type 构造，而非 make_pair。
        return m_map.insert(typename MapType::value_type(v.m_key.c_str(), v));
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
