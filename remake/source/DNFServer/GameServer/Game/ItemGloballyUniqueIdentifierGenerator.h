#ifndef DNF_GAME_ITEMGLOBALLYUNIQUEIDENTIFIERGENERATOR_H_
#define DNF_GAME_ITEMGLOBALLYUNIQUEIDENTIFIERGENERATOR_H_

// 迁移自 GameStubs.cpp（batch-5）。无共享头文件声明 itemGloballyUniqueIdentifier_t，
// 故本头文件定义之；CEquipItem.cpp / CStackableItem.cpp 各自持有同布局的本地拷贝。
// ORIG 符号：
//   _ZN30itemGloballyUniqueIdentifier_t5resetEv   (W 0x085139e8)
//   _ZN38CItemGloballyUniqueIdentifierGenerator8generateEP30itemGloballyUniqueIdentifier_ti
//                                                   (T 0x0889246c)
struct itemGloballyUniqueIdentifier_t
{
    void reset();
    char m_pad[0x11];
};

class CItemGloballyUniqueIdentifierGenerator
{
public:
    void generate(itemGloballyUniqueIdentifier_t* id, int field);
};

#endif  // DNF_GAME_ITEMGLOBALLYUNIQUEIDENTIFIERGENERATOR_H_