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
    // ORIG W 0x08695068：m_point(+0)=1、m_rate(+4)=0x3e8。
    CMCAPManager() : m_point(1), m_rate(0x3e8) {}
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
    unsigned int getExposedCount() const;      // ORIG W 0x0869504c：读 +0x10
    void incExposedCount();                    // ORIG W 0x08695038：+0x10 += 1
    bool isSaveCleanPadPoint() const;          // ORIG T 0x0822eede：读 +0x14
    void disableCleanPadPoint();               // ORIG T 0x0822eeea：写 +0x14=0

    unsigned int m_point;    // +0x00（getPoint 原始点数；reset 置 1；ORIG getPoint 0x0822eea4 以 this 直读）
    int m_rate;              // +0x04（reset/resetIncRate 置 1000；decIncRate 递减；incPoint 乘数）
    int m_field08;           // +0x08（reset 拷贝 +0x00）
    int m_field0c;           // +0x0c（reset 置 1）
    int m_field10;           // +0x10（reset 置 0）
    bool m_saveCleanPadPoint;  // +0x14（IsSaveCleanPadPoint 读 / setPoint/incPoint 置 1）
    char m_pad15[3];         // +0x15..0x17

    bool m_recvEventWaiting; // +0x18（0x8e408；CUser::StartRecvEventWaiting ORIG 0x0867f1fe 清零启动等待）
    char m_pad19[3];    // +0x19..0x1b
    int m_field1c;      // +0x1c（0x8e40c GetAutoClientRate）
    char m_pad20[4];         // +0x20..0x23
    int m_field24;           // +0x24（0x8e414 登录时间戳槽：DisPatcher_Login::dispatch_sig
                             //   ORIG 写 getCurSec；SetUserManagerToolIP 读——登录 IP 关联时间）
    char m_pcRoomIp[16];     // +0x28（0x8e418 PC房 IP 字符串；CHades::GetIP @0x84b9774 回退源实证）
    char m_pad38[4];                 // +0x38..0x3b（保持）
    unsigned int m_sellTimeChkBaseSec; // +0x3c（0x8e42c；CUser::SellItemTimeChecker
                                     //   ORIG 0x08230256 的窗口基准秒，超窗重置分支写回 nowSec）
    int m_sellTimeHitCount;          // +0x40（0x8e430；同函数命中计数/重置清零）
    short m_timeChkMinA;         // +0x44（GetItemTimeChecker 窗口 min val1）
    short m_timeChkMaxA;         // +0x46（max val1）
    short m_timeChkMinB;         // +0x48（min val2）
    short m_timeChkMaxB;         // +0x4a（max val2）
    unsigned int m_timeChkBaseSec; // +0x4c（nowSec 差窗基准）
    int m_timeChkCount;          // +0x50（命中计数/重置清零）
    short m_timeChkDiffA;        // +0x54（|maxA-minA|）
    short m_timeChkDiffB;        // +0x56（|maxB-minB|）
    // +0x58（0x8e448）：ORIG CUser::st_monstorkill 实例所在区（CUser 直读
    //   user+0x8e448 调 _ZN5CUser13st_monstorkill5CheckEjttttj）。类型保持
    //   CUser 嵌套类避免拆类 ODR，本处按等尺寸字节域承载【推断归属 mcap】。
    char m_monsterKillWindow[0x14];
    char m_pad6c[0x80 - 0x6c];   // +0x6c..0x7f（原 m_pad58 余段）
    short m_serialLastIdx;       // +0x80（GetItemSerialChecker 序列基准 idx）
    short m_serialLastVal;       // +0x82（序列基准值）
    int m_serialHitCount;        // +0x84（连续命中计数）
    unsigned int m_blackIpPunishMask;   // +0x88（CUser+0x8e478；Inter_LoginInfo::dispatch_sig
                                        //   写入 CBlackIPMonitor::getPunishType 位标志，bit0/bit1 判定）
    bool m_blackIpTradeLockFlag;        // +0x8c（CUser+0x8e47c；bit1 命中时的交易封锁标志）
    char m_pad8d[3];                    // +0x8d..0x8f
    int m_monsterDieHackCnt;     // +0x90（0x8e480；DieMob/MoveMap 连续
                                 //   0xffff 击杀计数，>4 报 0xcd/0xce 并归零）
    char m_pad94[0x544 - 0x94];
    bool m_field544;   // +0x544（0x8e934 setRestingUserRestrict）
    char m_pad545[1];  // +0x545
    short m_field546;  // +0x546（0x8e936 setPunishTradeAlert）
    // +0x548（0x8e938 getLastLotteryTime）：b28d-solo-CUser 实证——setLastLotteryTime
    // ORIG 0x082307cb `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x548(%eax)`
    // = LHS-first（P211 同型律）⇒ 字段与形参 `unsigned long t` 同型；int 字段时 4.4
    // 恒 RHS-first（`mov 0xc(%ebp),%edx` 在前）。ia32 ulong=4B，偏移/sizeof 不变。
    unsigned long m_lastLotteryTime;
    char m_field54c;   // +0x54c
    char m_pad54d[1];                   // +0x54d
    unsigned short m_cleanPadVeriA;     // +0x54e（GenerateCleanPadVeriData randInt(0xffff)+1）
    unsigned char m_cleanPadVeriB;      // +0x550（randInt(6)）
    char m_pad551[1];                   // +0x551
    unsigned short m_cleanPadVeriC;     // +0x552（randInt(0xffff)+1）
    bool m_loadingHackGold;             // +0x554（0x8e944 isLoadingHackGold movzbl 直返 0x08697448 /
                                        //   setLoadHackGold ORIG 0x08697458 bool 字段写形：this 先载 +
                                        //   movzbl 回读 + mov %dl,disp(%eax)，P33 判据）
    char m_pad555[3];                   // +0x555..0x557
    ::cSecuUserAction* m_secuUserAction; // +0x558（0x8e948；InitOnAccept 0x086493c8 new 后挂接并 Init()）
    unsigned int m_capchaReqTick;       // +0x55c（reqHumanCertify 写 getCurTickCount）
    bool m_hackIpLogged;                // +0x560（setHackUser ORIG 0x0867f748 type==3 IP 监视登记标志写 1）
    bool m_field561;   // +0x561（0x8e951 isARSUserKick）
    char m_pad562[0x564 - 0x562];

    unsigned int getPoint();                   // ORIG W 0x0822eea4 _ZN...8getPointEv
    void setPoint(unsigned short point);       // ORIG W 0x082870de _ZN...8setPointEt
    void incPoint(int value);                  // ORIG W 0x084eb53e _ZN...8incPointEi
    void decIncRate(int value);                // ORIG W 0x08694ff0 _ZN...10decIncRateEi
    void resetIncRate();                       // ORIG W 0x08694fe0 _ZN...12resetIncRateEv
    bool IsSaveCleanPadPoint();                // ORIG W 0x0822eede _ZN...19IsSaveCleanPadPointEv
    static int getPointPerHackType(WongWork::ENUM_HACKTYPE type);   // ORIG static（type@+0x8）
                                               // ORIG W 0x08695025 _ZN...19getPointPerHackTypeENS_13ENUM_HACKTYPEE
};
}

#endif  // DNF_GAME_CMCAP_MANAGER_H_