#ifndef DNF_GAME_CBUSINESSIMPL_H_
#define DNF_GAME_CBUSINESSIMPL_H_

// ============================================================================
// df_game_r 还原 —— IBusiness / CBusinessImpl（antibot 业务接口）。
//
// ORIG 证据（objdump df_game_r）：
//   _ZN9IBusinessC1Ev W @0x082a7c24：仅设 vptr=&_ZTV9IBusiness+8=0x8c1bad0；
//   _ZTV9IBusiness @0x8c1bac8 = [offset-top, typeinfo, 3×__cxa_pure_virtual]
//     —— 无虚析构槽（IBusiness 隐式非虚 dtor）。
//   _ZTV13CBusinessImpl @0x8b2bc50：
//     [SendBuffertoClient=0x080de240, PunishClient=0x080de3ee,
//      ProcessProtectMsg=0x080de4ec]；CBusinessImpl C1 0x082a7c32 =
//     [call IBusiness ctor, movl $0x8b2bc58,(this)]；D1 0x082a7c4e 仅重置 vptr。
// ============================================================================

class tagAntibotPunishInfo;    // antibot 域完整布局待专门批次
class tagTSafeModePkg;

enum EPUNISHMODE
{
    PUNISHMODE_PLACEHOLDER = 0   // 【推断】枚举成员值待 antibot 批次核实（不参与 mangling）
};

class IBusiness
{
public:
    IBusiness();                                                          // W 0x082a7c24
    // 返回类型 int：ORIG 0x080de3b9 尾部 `mov $0x0,%ebx … call CStreamGuardD1 …
    // mov %ebx,%eax; ret`（返回值跨 guard 析构保活）+ Ghidra 原型 undefined4；
    // void 版会被 gcc4.4 折成共享清理块（jmp），形态不符。mangled 不含返回类型。
    virtual int SendBuffertoClient(int a, int b, int c, unsigned int d,  // 槽0
                                    unsigned char* data, short len) = 0;
    virtual void PunishClient(int a, int b, unsigned int c,               // 槽1
                              EPUNISHMODE mode, tagAntibotPunishInfo* info,
                              char* reason) = 0;
    // ORIG 0x080de4ec 尾部 mov $0,%ebx/mov %ebx,%eax = int 返回 0（vtable 槽2）。
    virtual int ProcessProtectMsg(tagTSafeModePkg* pkg) = 0;             // 槽2
};

class CBusinessImpl : public IBusiness
{
public:
    CBusinessImpl();                                                      // C1 0x082a7c32
    ~CBusinessImpl();                                                     // D1 0x082a7c4e（非虚）
    int SendBuffertoClient(int a, int b, int c, unsigned int d,
                            unsigned char* data, short len);              // 0x080de240
    void PunishClient(int a, int b, unsigned int c, EPUNISHMODE mode,
                      tagAntibotPunishInfo* info, char* reason);          // 0x080de3ee
    int ProcessProtectMsg(tagTSafeModePkg* pkg);                          // 0x080de4ec
};

#endif  // DNF_GAME_CBUSINESSIMPL_H_
