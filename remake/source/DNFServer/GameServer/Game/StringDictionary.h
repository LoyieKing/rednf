#ifndef GAME_STRINGDICTIONARY_H_
#define GAME_STRINGDICTIONARY_H_

#include <string>
#include <vector>

// ============================================================================
// StringDictionary（df_game_r 重建，G2 字符串表批次）
// ----------------------------------------------------------------------------
// ORIG 类：10 个强符号 T
//   StringDictionary::StringDictionary()  _ZN16StringDictionaryC1Ev  0x8ab2240
//   StringDictionary::~StringDictionary() _ZN16StringDictionaryD1Ev  0x8ab22be
//   add(const char*, const char*)         _ZN16StringDictionary3addEPKcS1_ 0x8ab2398
//   freeze()                              _ZN16StringDictionary6freezeEv   0x8ab240e
//   get(const char*) const                _ZNK16StringDictionary3getEPKc    0x8ab2528
//   getCount() const                      _ZNK16StringDictionary8getCountEv 0x8ab2612
//   getKey(int) const                     _ZNK16StringDictionary6getKeyEi   0x8ab2628
//   getValue(int) const                   _ZNK16StringDictionary8getValueEi 0x8ab2668
//   setValue(int, const char*)            _ZN16StringDictionary8setValueEiPKc 0x8ab26a8
//   addString(const char*)                _ZN16StringDictionary9addStringEPKc 0x8ab2772
//
// 权威布局（sizeof == 0x424）：
//   +0x00  std::vector<unsigned char*>   blocks_     字符串大块存储（12 字节 {begin,end,cap}）
//   +0x0c  int                           m_curPtr    当前写入指针（按地址存的 int）
//   +0x10  int                           m_curRemain 当前块剩余可写字节
//   +0x14  std::vector<StringEntity>     entities_   实体表 { hash; key; value }（12 字节）
//   +0x20  std::vector<StringEntity>::iterator buckets_[0x101]  桶表 0x101*4=0x404
//   （0x100 个桶迭代器 + 末端迭代器 buckets_[0x100]，总 0x20+0x404 = 0x424）
//
// getStringHashValue 为本 TU static（0x8ab21f8，nm t），在此声明为私有 static。
// kBufSize = 0x100（0x8e2ca64，本 TU static const）。
// ============================================================================
class StringDictionary
{
public:
    StringDictionary();
    ~StringDictionary();

    void add(const char* key, const char* value);
    const char* addString(const char* s);
    void freeze();
    const char* get(const char* key) const;
    int getCount() const;
    const char* getKey(int index) const;
    const char* getValue(int index) const;
    void setValue(int index, const char* value);

    struct StringEntity
    {
        unsigned int hash;   // +0x00
        const char*  key;    // +0x04
        const char*  value;  // +0x08

        // ORIG StringEntity::operator<（0x8ab2842）仅比较 hash 字段。
        bool operator<(const StringEntity& rhs) const
        {
            return hash < rhs.hash;
        }
    };

    std::vector<unsigned char*>             blocks_;     // +0x00
    int                                     m_curPtr;    // +0x0c
    int                                     m_curRemain; // +0x10
    std::vector<StringEntity>               entities_;   // +0x14
    std::vector<StringEntity>::iterator     buckets_[0x101]; // +0x20
};

#endif  // GAME_STRINGDICTIONARY_H_
