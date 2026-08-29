// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/STRDCharacterStatusClient/
// STRDCharacterStatusClient：ORIG 导出 operator=(STRDCharacterStatus const&) T @0x08a0cab8；
// 布局由双向 operator= 汇编（0x08a0c942 / 0x08a0cab8）逐偏移互证：
//   +0x00/+0x04  int hpMax/mpMax（dword 直拷）
//   +0x08..+0x14  uint phys/mag 攻防 4 项（ushort 零扩展）
//   循环 i<=3   +0x18+4i   int[4] 元素耐性（ushort 符号扩展 movzwl+movswl）
//   循环 k<=16  +0x28+4k   int[17] 状态耐性（同上）
//   +0x6c int inventoryLimit / +0x70/+0x74 int hpRegen/mpRegen（符号扩展）
//   +0x78 int moveSpeed / +0x7c/+0x80 uint attackSpeed/castSpeed（零扩展）
//   +0x84/+0x88 int hitRecovery/jumpPower（符号扩展）/ +0x8c int weight
//   +0x90 uint field52（byte 零扩展）/ +0x94 int field53
// sizeof == 0x98；反向（Status::operator=(Client) 0x08a0c942）按同偏移读回截断。
#ifndef DNF_GAME_STRDCHARACTERSTATUSCLIENT_H_
#define DNF_GAME_STRDCHARACTERSTATUSCLIENT_H_

class STRDCharacterStatus;  // 完整定义 STCharacterScript.h:38

struct STRDCharacterStatusClient
{
    STRDCharacterStatusClient& operator=(STRDCharacterStatus const&);  // ORIG T @0x08a0cab8

    int m_hpMax;                 // +0x00
    int m_mpMax;                 // +0x04
    unsigned int m_physAttack;   // +0x08
    unsigned int m_physDefense;  // +0x0c
    unsigned int m_magAttack;    // +0x10
    unsigned int m_magDefense;   // +0x14
    int m_elemResist[4];         // +0x18..+0x24
    int m_activeResist[17];      // +0x28..+0x68
    int m_inventoryLimit;        // +0x6c
    int m_hpRegen;               // +0x70
    int m_mpRegen;               // +0x74
    int m_moveSpeed;             // +0x78
    unsigned int m_attackSpeed;  // +0x7c
    unsigned int m_castSpeed;    // +0x80
    int m_hitRecovery;           // +0x84
    int m_jumpPower;             // +0x88
    int m_weight;                // +0x8c
    unsigned int m_field52;      // +0x90（byte 展开）
    int m_field53;               // +0x94
};  // sizeof == 0x98

typedef char assert_strd_client_size[
    (sizeof(STRDCharacterStatusClient) == 0x98) ? 1 : -1];

#endif  // DNF_GAME_STRDCHARACTERSTATUSCLIENT_H_
