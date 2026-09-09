// ============================================================================
// MonsterWrapLCB.cpp — LinkClosureB 批 B 怪物包装传递依赖收口（唯一 owner TU）
//
// 全部函数依据 df_game_r objdump/docs/class_func_reports 报告逐指令转写。
// 覆盖符号：
//   _ZNK12CAICharacter13IsHellMonsterEv                      W @0x08539de8
//   _ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE
//                                                            T @0x0834a15e
//   _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE
//                                                            T @0x0834f8a6
//   _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE
//                                                            T @0x0834f8e8
//   _Z22generateDropAvatarItemRK23stEnamyDropAvatarInfo_thhRSt6vectorI10Inven_ItemSaIS3_EE
//                                                            T @0x08349da0 (0x1ab)
//   _Z24generateCatchMonsterItemRKSt6vectorI13CatchItemInfoSaIS0_EERS_I10Inven_ItemSaIS5_EE
//                                                            T @0x08349f4b (0x11a)
// CLuckPoint::UseLuckPoint(int,int,int,int) @0x08550b14 与
// CLuckPoint::sendItemDropRate(char const*) @0x08550ef0 已由 CClearReward.cpp
// 提供唯一 owner（LinkKillChain 批次，语义经 ORIG 报告逐分支核对一致），
// 本 TU 不重复定义。
// ============================================================================
#include "MonsterIndependentDrop.h"

#include "CItemList.h"   // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMonster.h"   // CMonster::getMonsterScript / STMonsterScript 完整类型

// 全局生成函数自由声明（此前仅 mangled 符号存在于 ORIG；本 TU 为权威 owner）
void generateDropAvatarItem(const stEnamyDropAvatarInfo_t& info,
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out);   // ORIG 0x08349da0
void generateCatchMonsterItem(const std::vector<CatchItemInfo>& items,
                              std::vector<Inven_Item>& out);  // ORIG 0x08349f4b

// ----------------------------------------------------------------------------
// ORIG W 0x08539de8 _ZNK12CAICharacter13IsHellMonsterEv（0xc）：
//   movzbl 0x70(this) 直返。mirror 类 +0x30..+0x6b 为 m_dropAvatarInfo，
//   +0x70 rarity 标志未具名建模，按字节视图读取。
unsigned char CAICharacter::IsHellMonster() const
{
    // [b28c-35] ORIG 0x08539de8 为单条 `movzbl 0x70(%eax),%eax`；裸偏移算术链
    // （reinterpret_cast + 0x70）4.4 发 `add $0x70,%eax; movzbl (%eax)`（+1 条）。
    // 改具名尾部视图成员访问（P82）即折叠单条 disp；视图只读该字节，不改类布局。
    struct Tail { char m_pad[0x70]; unsigned char m_flag; };
    return reinterpret_cast<const Tail*>(this)->m_flag;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834a15e _ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   lea 0x30(this) 取 m_dropAvatarInfo 后转发自由函数。
void CAICharacter::generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                          std::vector<Inven_Item>& out) const
{
    ::generateDropAvatarItem(m_dropAvatarInfo, rarity, grade, out);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834f8a6 _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0xcf8 —— 即 m_script(+0x60) 内
//   m_dropAvatarInfo(+0xc98) 子对象，具名访问后转发自由函数。
void CMonster::generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                      std::vector<Inven_Item>& out) const
{
    // ORIG 无 call：this+0x60+0xc98=+0xcf8 直算（成员直访折叠，无 script 槽）。
    ::generateDropAvatarItem(m_script.m_dropAvatarInfo, rarity, grade, out);
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834f8e8 _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0x70(script) 取 m_catchItems 转发。
void CMonster::generateCatchMonsterItem(std::vector<Inven_Item>& out) const
{
    // P134：ORIG call 后结果寄存器直用（lea 0x70(%eax)），无 script 局部槽。
    ::generateCatchMonsterItem(
        static_cast<STMonsterScript*>(getMonsterScript())->m_catchItems, out);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08349da0 _Z22generateDropAvatarItemRK23stEnamyDropAvatarInfo_thhRSt6vectorI10Inven_ItemSaIS3_EE
// （size 0x1ab）头像随机掉落主逻辑：
//   ① 入门阈值：threshold = (int)((double)m_field00 * T_A[(u8)rarity]
//                                 * T_B[(u8)grade - 1])；
//     T_A 基址 info+0x18（flds 0x8(%eax,%edx,4)，edx=(u8)rarity+4）、T_B 基址
//     info+0x8、下标 (u8)grade-1 —— 两片均为 clear 清零区浮点表的重叠视图，
//     按指针视图逐指令转写；fistpll 截断取低 dword。
//     get_rand_int(100000) > threshold 时直接返回。
//   ② 共 rounds = *(unsigned*)(info+0x04)（无符号 seta 比较）轮：
//       每轮 r = get_rand_int(*(int*)(info+0x2c))；
//       自 m_elements.begin() 重新遍历：首个 elem.m_field04 > r（无符号）时，
//       Inven_Item{}；item.m_addInfo(ORIG 写 +0x02 视图) ← elem.m_field00；
//       G_CDataManager()->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)1,
//       item, 1)；out.push_back(item)；命中后不推进迭代器即退出本轮
//       （ORIG jmp +0x170 → 轮数 +1 继续）。
// [b28d-L14 N18a] ORIG 0x08598f34；p167 缺 1 次 const_iterator ptEv；残差=槽深/寄存器。
void generateDropAvatarItem(const stEnamyDropAvatarInfo_t& info,
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out)
{
    const char* base = reinterpret_cast<const char*>(&info);

    // ① 入门阈值（ORIG fildl/fmul/fistpll 序列）
    double thresholdD =
        static_cast<double>(static_cast<long long>(info.m_field00));
    const float& tableA =
        *reinterpret_cast<const float*>(base + 0x8 +
                                        4 * (static_cast<unsigned>(rarity) + 4));
    const float& tableB =
        *reinterpret_cast<const float*>(base + 0x8 +
                                        4 * (static_cast<unsigned>(grade) - 1));
    thresholdD *= tableA;
    thresholdD *= tableB;
    const int threshold = static_cast<int>(thresholdD);
    if (get_rand_int(100000) > threshold)
        return;

    // ② roll 轮次
    const unsigned int rounds = (unsigned int)info.m_field04;
    for (unsigned int round = 0; round < rounds; ++round)
    {
        const int r = get_rand_int(info.m_field2c);
        std::vector<stEnamyDropAvatarElement_t>::const_iterator it =
            info.m_elements.begin();
        while (it != info.m_elements.end())
        {
            if (static_cast<unsigned int>(it->m_field4) >
                static_cast<unsigned int>(r))
            {
                Inven_Item item;
                item.m_addInfo = it->m_field0;              // ORIG 写 +0x02 视图
                G_CDataManager()->m_itemList->create_item(
                    (ENUM_ITEM_CREATE_TYPE)1, item, 1);
                out.push_back(item);
                break;                                      // 不推进迭代器（ORIG jmp +0x170）
            }
            it++;
        }
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08349f4b _Z24generateCatchMonsterItemRKSt6vectorI13CatchItemInfoSaIS0_EERS_I10Inven_ItemSaIS5_EE
// （size 0x11a）：CatchItemInfo 三成员均 int 存储，第 3 成员由 flds 按 float
// 位型消费，故以 reinterpret 视图承载。
//   count(m_field4)==0 → 跳过；
//   rate(m_field8 视 float)==0.0f → 跳过（fnstsw+sahf 仅判 ZF：NaN 继续走）；
//   r = get_rand_int(0x64)；(double)rate > (double)r 才生成；
//   Inven_Item{}；item.m_addInfo = m_field0；create_item((T)1,item,1)；
//   push_back count 次（signed setg 比较）。
// [b28d-L14 P167/N90] ORIG 0x08598d2c；已改 m_itemList 直取（消 EXTRA getItemList）；
// 残差=12B 结构体按值拷贝形（N90 族）。
void generateCatchMonsterItem(const std::vector<CatchItemInfo>& items,
                              std::vector<Inven_Item>& out)
{
    for (std::vector<CatchItemInfo>::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        const CatchItemInfo entry = *it;                    // [0x8349f86 de 三字段拷贝]
        const float rate =
            *reinterpret_cast<const float*>(&entry.m_field8);

        if (entry.m_field4 == 0)                            // [0x8349fa7 test/je]
            continue;
        if (rate == 0.0f)                                   // [0x8349fb2 fucompp: je NaN 续行]
            continue;

        const int r = get_rand_int(0x64);                   // [0x8349fc2 $0x64]
        if (!(static_cast<double>(rate) > static_cast<double>(r)))
            continue;                                       // [0x8349fd1 seta]

        Inven_Item item;
        item.m_addInfo = entry.m_field0;                    // [0x8349fe5 写 +0x02 视图]
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)1, item, 1);             // [0x8349ffd create_item(1,.,1)]
        for (int j = 0; j < entry.m_field4; ++j)            // [0x834a031 setg signed]
            out.push_back(item);
    }
}
