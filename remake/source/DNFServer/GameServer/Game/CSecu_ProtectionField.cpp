// df_game_r Game/ CSecu_ProtectionField 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CInventory.h"

// SECURITY_PROTCTION_TYPES：ORIG mangled _Z23ExceptionProtectionTypei23SECURITY_
// PROTECTION... 所需枚举名（仅本 TU 使用）。
enum SECURITY_PROTCTION_TYPES
{
    SECURITY_PROTCTION_TYPES_0 = 0
};

// ORIG 0x8288074：对 base 依次检查 9 个类型参数，为 0 则清对应位（bit0..bit8）。
int ExceptionProtectionType(int value, SECURITY_PROTCTION_TYPES t0,
                            SECURITY_PROTCTION_TYPES t1, SECURITY_PROTCTION_TYPES t2,
                            SECURITY_PROTCTION_TYPES t3, SECURITY_PROTCTION_TYPES t4,
                            SECURITY_PROTCTION_TYPES t5, SECURITY_PROTCTION_TYPES t6,
                            SECURITY_PROTCTION_TYPES t7, SECURITY_PROTCTION_TYPES t8)
{
    if (t0 == 0) value &= ~0x01;
    if (t1 == 0) value &= ~0x02;
    if (t2 == 0) value &= ~0x04;
    if (t3 == 0) value &= ~0x08;
    if (t4 == 0) value &= ~0x10;
    if (t5 == 0) value &= ~0x20;
    if (t6 == 0) value &= ~0x40;
    if (t7 == 0) value &= ~0x80;
    if (t8 == 0) value &= ~0x100;
    return value;
}

int CSecu_ProtectionField::Check(CUser* user, SECURITY_PROTCTION type)
{
    // ORIG 0x08288a02：thiscall 成员函数（_ZN21CSecu_ProtectionField5CheckEP5CUser18SECURITY_PROTCTION）。
    if (user == 0) return 1;
    unsigned int protType = GetProtectionType(type);
    if (protType == 0) return 0;
    int ret = CheckRestoreType(user, protType);
    if (ret != 0) return ret;
    ret = CheckTradingType(user, protType, type);
    if (ret != 0) return ret;
    ret = CheckEtcType(user, type);
    if (ret != 0) return ret;
    return 0;
}

int CSecu_ProtectionField::GetOppositeErr(int err)
{
    if (err == 0x88) return 0xec;
    if (err < 0x89) {
        if (err == 0x7a) return 0xee;
        if (err == 0x7b) return 0x7c;
        if (err == 0x72) return 0x73;
    } else {
        if (err == 0x89) return 0x8a;
        if ((unsigned int)(err - 0xe4) < 2) return 0x73;
    }
    return 1;
}

unsigned int CSecu_ProtectionField::GetProtectionType(SECURITY_PROTCTION type)
{
    // ORIG 0x82880dc：map 查找，命中返回 second，否则 0。
    std::map<SECURITY_PROTCTION, unsigned int>::iterator it = m_typeMap.find(type);
    if (it == m_typeMap.end()) return 0;
    return it->second;
}

// ORIG 0x82881a2：把商城/保护类型错误码映射为 CCeraShop 可识别的错误号。
// 逐 switch 分支（报告汇编）：0x72->0x87, 0x73->0x82, 0x78->0x8b, 0x7a->0x83,
// 0x7b->0x84, 0x88->0x85, 0x89->0x86, 0xcf->0x8a, 0xe4->0x89, 0xe5->0x88, 其余->1。
int CSecu_ProtectionField::GetCashShopErr(int err)
{
    if (err == 0x7b) return 0x84;
    if (err < 0x7c) {
        if (err == 0x73) return 0x82;
        if (err < 0x74) {
            if (err == 0x72) return 0x87;
        } else {
            if (err == 0x78) return 0x8b;
            if (err == 0x7a) return 0x83;
        }
    } else {
        if (err == 0xcf) return 0x8a;
        if (err < 0xd0) {
            if (err == 0x88) return 0x85;
            if (err == 0x89) return 0x86;
        } else {
            if (err == 0xe4) return 0x89;
            if (err == 0xe5) return 0x88;
        }
    }
    return 1;
}

void CSecu_ProtectionField::init()
{
    // ORIG 0x828824e。两轮异常表（.rodata 0x8c141c0 / 0x8c14260）；
    // 注意 ORIG 第二轮前把整表重置回 0x1ff（第一轮结果被覆盖，忠实保留），
    // 最终以第二轮结果插入 map（i=0..0x46）。
    static const int kTableA[37] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
        0x22, 0x24, 0x1f, 0x27, 0x25, 0x26, 0x2c, 0x2d, 0x2f, 0x37,
        0x41, 0x42, 0x43, 0x3a, 0x3b, 0x20, 0x0b };
    static const int kTableB[29] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x11, 0x14,
        0x16, 0x17, 0x18, 0x1d, 0x22, 0x24, 0x2c, 0x2d, 0x1f, 0x20,
        0x2f, 0x41, 0x42, 0x43, 0x38, 0x39, 0x3a, 0x3b, 0x0b };

    int values[0x47];

    // 第一轮（结果随后被重置，ORIG 原样保留）
    for (int i = 0; i < 0x47; ++i) values[i] = 0x1ff;
    for (int i = 0; i < 0x47; ++i)
    {
        for (int j = 0; j < 37; ++j)
        {
            if (i == kTableA[j])
                values[i] = ExceptionProtectionType(values[i], (SECURITY_PROTCTION_TYPES)0,
                    (SECURITY_PROTCTION_TYPES)0, (SECURITY_PROTCTION_TYPES)0,
                    (SECURITY_PROTCTION_TYPES)0, (SECURITY_PROTCTION_TYPES)0x10,
                    (SECURITY_PROTCTION_TYPES)0, (SECURITY_PROTCTION_TYPES)0,
                    (SECURITY_PROTCTION_TYPES)0x80, (SECURITY_PROTCTION_TYPES)0);
        }
    }

    // 第二轮
    for (int i = 0; i < 0x47; ++i) values[i] = 0x1ff;
    for (int i = 0; i < 0x47; ++i)
    {
        for (int j = 0; j < 29; ++j)
        {
            if (i == kTableB[j])
                values[i] = ExceptionProtectionType(values[i], (SECURITY_PROTCTION_TYPES)1,
                    (SECURITY_PROTCTION_TYPES)2, (SECURITY_PROTCTION_TYPES)4,
                    (SECURITY_PROTCTION_TYPES)8, (SECURITY_PROTCTION_TYPES)0x10,
                    (SECURITY_PROTCTION_TYPES)0x20, (SECURITY_PROTCTION_TYPES)0x40,
                    (SECURITY_PROTCTION_TYPES)0, (SECURITY_PROTCTION_TYPES)0x100);
        }
    }

    for (int i = 0; i < 0x47; ++i)
        m_typeMap.insert(std::make_pair((SECURITY_PROTCTION)i, (unsigned int)values[i]));
}

CSecu_ProtectionField::CSecu_ProtectionField()
{
    init();   // ORIG 0x82aa6d4：map 默认构造后调用 init()
}

// ORIG 0x0828873a（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTCTIONm）：
// 物品 index 命中受保护集合（0x1f70/0x1f71/0x989a13/0x292090e1）时委托 Check(user,type)，
// 否则直接 0。
int CSecu_ProtectionField::CheckItem(CUser* user, SECURITY_PROTCTION type,
                                     unsigned long itemIndex)
{
    if (itemIndex == 0x1f70 || itemIndex == 0x1f71 ||
        itemIndex == 0x989a13 || itemIndex == 0x292090e1)
    {
        return Check(user, type);
    }
    return 0;
}

// ORIG 0x08288786（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTECTIONPK5CItem）：
// item/user 任一为空则 1，否则取 item->get_index() 走上面的重载。
int CSecu_ProtectionField::CheckItem(CUser* user, SECURITY_PROTCTION type,
                                     const CItem* item)
{
    if (item == 0 || user == 0)
        return 1;
    return CheckItem(user, type, (unsigned long)item->get_index());
}

int CSecu_ProtectionField::CheckRestoreType(CUser* user, unsigned int protType)
{
    return 0;
}

int CSecu_ProtectionField::CheckTradingType(CUser* user, unsigned int protType,
                                             SECURITY_PROTCTION type)
{
    return 0;
}

int CSecu_ProtectionField::CheckEtcType(CUser* user, SECURITY_PROTCTION type)
{
    if (user == 0) return 1;
    if (user->isRestingUser()) return 0xcf;
    if ((type == (SECURITY_PROTCTION)8 || type == (SECURITY_PROTCTION)9) &&
        *(unsigned char*)((char*)user + 0x8e97d) == 0) return 0xe9;
    return 0;
}
