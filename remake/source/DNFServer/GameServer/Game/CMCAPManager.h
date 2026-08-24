#ifndef DNF_GAME_CMCAP_MANAGER_H_
#define DNF_GAME_CMCAP_MANAGER_H_

namespace WongWork
{
// WongWork::CMCAPManager —— 唯一声明点（自 CUser.h 收敛；ORIG reset() 弱符号
// 0x8694fa0 / init() T 0x080f8aba，CDataManager::init 静态调用）。
// 布局：CUser 内嵌于 +0x8e3f0（sizeof ≈ 0x564），字段语义按 ORIG 访问器具名。
class CMCAPManager
{
public:
    CMCAPManager() {}
    ~CMCAPManager() {}

    static void init();            // ORIG _ZN8WongWork12CMCAPManager4initEv T 0x080f8aba
                                   // （定义 CtorProducers_2C.cpp；double-memset 忠实转写）
// 静态掉落点数表：ORIG B 0x093fad20，nm 尺寸 15600 = 3900×4（setPointPerHackType
// 循环上界 0xf3c=3900 实证；init 的 memset 长度 0x3cf0 同证）。
static int pointPerHackType_[3900];   // _ZN8WongWork12CMCAPManager17pointPerHackType_E
    void resetExposedCount();
    void reset();                  // ORIG W 0x8694fa0（定义见 CMCAPManager.cpp）

    char m_pad[0x18];   // +0x00..0x17
    bool m_field18;     // +0x18（0x8e408 IsRecvEvent）
    char m_pad19[3];    // +0x19..0x1b
    int m_field1c;      // +0x1c（0x8e40c GetAutoClientRate）
    char m_pad20[0x84 - 0x20];
    int m_field84;     // +0x84
    char m_pad88[8];   // +0x88..0x8f
    int m_field90;     // +0x90
    char m_pad94[0x544 - 0x94];
    bool m_field544;   // +0x544（0x8e934 setRestingUserRestrict）
    char m_pad545[1];  // +0x545
    short m_field546;  // +0x546（0x8e936 setPunishTradeAlert）
    int m_lastLotteryTime;  // +0x548（0x8e938 getLastLotteryTime）
    char m_field54c;   // +0x54c
    char m_pad54d[0x561 - 0x54d];
    bool m_field561;   // +0x561（0x8e951 isARSUserKick）
    char m_pad562[0x564 - 0x562];
};
}

#endif  // DNF_GAME_CMCAP_MANAGER_H_