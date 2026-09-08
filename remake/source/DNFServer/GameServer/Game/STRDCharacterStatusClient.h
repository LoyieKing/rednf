// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/STRDCharacterStatusClient/
// STRDCharacterStatusClient：ORIG 导出 operator=(STRDCharacterStatus const&) T @0x08a0cab8；
// 布局由双向 operator= 汇编（0x08a0c942 / 0x08a0cab8）逐偏移互证：
//   +0x00/+0x04  hpMax/mpMax（dword 直拷；与 Status 侧符号性不一致，双向 op= 均生
//                mov %eax,%edx 中转 @0x08a0c942/0x08a0cab8 ⇒ 一侧 unsigned）
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

    unsigned int m_hpMax;        // +0x00（与 Status 侧 int 类型不一致，op= 中转实证）
    unsigned int m_mpMax;        // +0x04
    unsigned int m_physAttack;   // +0x08
    unsigned int m_physDefense;  // +0x0c
    unsigned int m_magAttack;    // +0x10
    unsigned int m_magDefense;   // +0x14
    int m_elemResist[4];         // +0x18..+0x24
    int m_activeResist[17];      // +0x28..+0x68
    int m_inventoryLimit;        // +0x6c
    int m_hpRegen;               // +0x70
    int m_mpRegen;               // +0x74
    unsigned int m_moveSpeed;    // +0x78（同上，双向 op= 中转实证）
    unsigned int m_attackSpeed;  // +0x7c
    unsigned int m_castSpeed;    // +0x80
    int m_hitRecovery;           // +0x84
    int m_jumpPower;             // +0x88
    int m_weight;                // +0x8c
    unsigned int m_field52;      // +0x90（byte 展开）
    float m_field53;             // +0x94（b28c-28：float，与 STRDCharacterStatus 同型；
                                 //   ORIG 双向 op= 0x08a0cab0/0x08a0cc31 均整数 mov 直传）
};  // sizeof == 0x98

typedef char assert_strd_client_size[
    (sizeof(STRDCharacterStatusClient) == 0x98) ? 1 : -1];

#endif  // DNF_GAME_STRDCHARACTERSTATUSCLIENT_H_
