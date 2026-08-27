#ifndef DNF_GAME_CMCAP_MANAGER_H_
#define DNF_GAME_CMCAP_MANAGER_H_

#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE（getPointPerHackType 形参）
class cSecuUserAction;   // 全局命名空间（ORIG _ZN15cSecuUserAction…）；定义暂在 CUserOnlyOrig.cpp
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

    // ORIG T 0x080f8be0 _ZN8WongWork12CMCAPManager19setPointPerHackTypeEPi
    static void setPointPerHackType(int* data);   // 定义 CMCAPManager.cpp；拷贝 0xf3c 项
    // ORIG T 0x080f8aba（CtorProducers_2C.cpp 实现；CDataManager::init 静态调用）。
    static void init();
// 静态掉落点数表：ORIG B 0x093fad20，nm 尺寸 15600 = 3900×4（setPointPerHackType
// 循环上界 0xf3c=3900 实证；init 的 memset 长度 0x3cf0 同证）。
static int pointPerHackType_[3900];   // _ZN8WongWork12CMCAPManager17pointPerHackType_E
    void resetExposedCount();
    void reset();                  // ORIG W 0x8694fa0（定义见 CMCAPManager.cpp）
    unsigned int getPoint() const;             // ORIG T 0x0822eea4：max(min(10000,+0x10),0)
    void incExposedCount();                    // ORIG W 0x08695038：m_point += 1
    bool isSaveCleanPadPoint() const;          // ORIG T 0x0822eede：读 +0x14
    void disableCleanPadPoint();               // ORIG T 0x0822eeea：写 +0x14=0
    unsigned int getExposedCount() const;      // ORIG W 0x0869504c：读 +0x10（m_point）

    char m_pad00[0x10];      // +0x00..0x0f
    unsigned int m_point;    // +0x10（getPoint 原始点数；0x8e400）
    bool m_saveCleanPadPoint;  // +0x14（IsSaveCleanPadPoint；0x8e414）
    char m_pad15[3];         // +0x15..0x17

    bool m_field18;     // +0x18（0x8e408 IsRecvEvent）
    char m_pad19[3];    // +0x19..0x1b
    int m_field1c;      // +0x1c（0x8e40c GetAutoClientRate）
    char m_pad20[8];         // +0x20..0x27
    char m_pcRoomIp[16];     // +0x28（0x8e418 PC房 IP 字符串；CHades::GetIP @0x84b9774 回退源实证）
    char m_pad38[0x44 - 0x38];   // +0x38..0x43
    short m_timeChkMinA;         // +0x44（GetItemTimeChecker 窗口 min val1）
    short m_timeChkMaxA;         // +0x46（max val1）
    short m_timeChkMinB;         // +0x48（min val2）
    short m_timeChkMaxB;         // +0x4a（max val2）
    unsigned int m_timeChkBaseSec; // +0x4c（nowSec 差窗基准）
    int m_timeChkCount;          // +0x50（命中计数/重置清零）
    short m_timeChkDiffA;        // +0x54（|maxA-minA|）
    short m_timeChkDiffB;        // +0x56（|maxB-minB|）
    char m_pad58[0x80 - 0x58];   // +0x58..0x7f
    short m_serialLastIdx;       // +0x80（GetItemSerialChecker 序列基准 idx）
    short m_serialLastVal;       // +0x82（序列基准值）
    int m_serialHitCount;        // +0x84（连续命中计数）
    char m_pad88[8];   // +0x88..0x8f
    int m_field90;     // +0x90
    char m_pad94[0x544 - 0x94];
    bool m_field544;   // +0x544（0x8e934 setRestingUserRestrict）
    char m_pad545[1];  // +0x545
    short m_field546;  // +0x546（0x8e936 setPunishTradeAlert）
    int m_lastLotteryTime;  // +0x548（0x8e938 getLastLotteryTime）
    char m_field54c;   // +0x54c
    char m_pad54d[1];                   // +0x54d
    unsigned short m_cleanPadVeriA;     // +0x54e（GenerateCleanPadVeriData randInt(0xffff)+1）
    unsigned char m_cleanPadVeriB;      // +0x550（randInt(6)）
    char m_pad551[1];                   // +0x551
    unsigned short m_cleanPadVeriC;     // +0x552（randInt(0xffff)+1）
    char m_pad554[4];                   // +0x554..0x558
    ::cSecuUserAction* m_secuUserAction; // +0x558（0x8e948；InitOnAccept 0x086493c8 new 后挂接并 Init()）
    unsigned int m_capchaReqTick;       // +0x55c（reqHumanCertify 写 getCurTickCount）
    char m_pad560[1];                   // +0x560
    bool m_field561;   // +0x561（0x8e951 isARSUserKick）
    char m_pad562[0x564 - 0x562];

    unsigned int getPoint();                   // ORIG W 0x0822eea4 _ZN...8getPointEv
    void setPoint(unsigned short point);       // ORIG W 0x082870de _ZN...8setPointEt
    void incPoint(int value);                  // ORIG W 0x084eb53e _ZN...8incPointEi
    void decIncRate(int value);                // ORIG W 0x08694ff0 _ZN...10decIncRateEi
    void resetIncRate();                       // ORIG W 0x08694fe0 _ZN...12resetIncRateEv
    bool IsSaveCleanPadPoint();                // ORIG W 0x0822eede _ZN...19IsSaveCleanPadPointEv
    int getPointPerHackType(WongWork::ENUM_HACKTYPE type);
                                               // ORIG W 0x08695025 _ZN...19getPointPerHackTypeENS_13ENUM_HACKTYPEE
};
}

#endif  // DNF_GAME_CMCAP_MANAGER_H_