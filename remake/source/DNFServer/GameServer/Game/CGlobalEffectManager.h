#ifndef GAME_CGLOBALEFFECTMANAGER_H_
#define GAME_CGLOBALEFFECTMANAGER_H_

// ============================================================================
// df_game_r 还原 —— CGlobalEffectManager (0x18 布局) + CGlobalEffect (0x2c)。
// 布局/函数逐条取自 ORIG 反汇编与 docs/class_func_reports/CGlobalEffectManager/：
//   CGlobalEffect ctor  0x084b800a  memcpy 布局：+0 type, +4 value(float),
//                                              +8 name[0x20], +0x28 endTime
//   CGlobalEffectManager ctor 0x082a721a：两个 vector<CGlobalEffect>（+0x00/+0x0c）
//   函数：registNewEffect 0x084b7932 / getEffectNum 0x08234fca /
//         isAffectedEffect 0x084b7d2c / getEffectValue 0x084b7daa /
//         makeGlobalEffectInfo 0x084b7ec4 / notifyGlobalEffectInfo 0x084b7e48 /
//         beginReservedEffect 0x084b7c50 / checkTimeout 0x084b7a94
// 依赖：PacketGuard/InterfacePacketBuf/G_GameWorld()->send_all/G_TimerQueue()
//        CSystemTime::getCurSec / TimerGlobalEffectItemBegin|Timeout::registTimer
// ============================================================================

#include <vector>

class PacketGuard;

// ORIG 0x084b800a / 0x082a7214（D1/D2 同名弱符号）：0x2c 布局。
//   +0x00 unsigned int m_type
//   +0x04 float        m_value
//   +0x08 char         m_name[0x20]   （strncpy 0x1e 于 ctor）
//   +0x28 long         m_endTime
class CGlobalEffect
{
public:
    CGlobalEffect(unsigned int type, float value, const char* name, long endTime);
    ~CGlobalEffect();

    unsigned int getType() const;      // ORIG 0x084b7fda
    float        getValue() const;     // ORIG 0x084b7fe4
    bool         isTimeout(long curSec) const;  // ORIG 0x084b7ff8：m_endTime <= curSec
    void         makeNotifyInfo(PacketGuard& packet) const;  // ORIG 0x084b7880

    unsigned int m_type;       // +0x00
    float        m_value;      // +0x04
    char         m_name[0x20]; // +0x08
    long         m_endTime;    // +0x28
};

// ORIG 0x082a721a ctor：两个 vector<CGlobalEffect>（+0x00 活动 / +0x0c 保留）。
// sizeof = 0x18（两个 vector<CGlobalEffect> 各 12 字节；无虚表，无 dtor 符号）。
class CGlobalEffectManager
{
public:
    CGlobalEffectManager();

    void registNewEffect(unsigned int type, float value, const char* name,
                         long beginSec, long reserveSec);   // ORIG 0x084b7932
    unsigned int getEffectNum() const;                      // ORIG 0x08234fca
    bool isAffectedEffect(unsigned int type) const;         // ORIG 0x084b7d2c
    float getEffectValue(unsigned int type) const;          // ORIG 0x084b7daa
    void makeGlobalEffectInfo(PacketGuard& packet) const;   // ORIG 0x084b7ec4
    void notifyGlobalEffectInfo() const;                    // ORIG 0x084b7e48
    void beginReservedEffect(unsigned int type);            // ORIG 0x084b7c50
    void checkTimeout(unsigned int type, unsigned int curSec);  // ORIG 0x084b7a94

private:
    std::vector<CGlobalEffect> m_activeEffects;   // +0x00
    std::vector<CGlobalEffect> m_reservedEffects; // +0x0c
};

// TimerGlobalEffectItemBegin / TimerGlobalEffectItemTimeout 权威声明唯一在
// TimerDispatcher.h（ORIG :public TimerSig 含 dispatch_sig；本头此前的无基类
// 视图类与其构成 ODR 重复，已删除）。registTimer(unsigned,unsigned) 定义由
// 原实现 TU 提供（ORIG T 0x08145caa / 0x08145cf4）。
#include "TimerDispatcher.h"

#endif  // GAME_CGLOBALEFFECTMANAGER_H_
