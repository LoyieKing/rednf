// df_game_r 还原 —— GameServerDemonManager（LinkClosureB-2 声明头）
// ORIG 符号（nm -S --defined-only 核对）：
//   T _Z30GameserverDemonManagerInstancev            0x080f61b8
//   T _ZN22GameServerDemonManagerD1Ev/D2Ev           0x080f621c
//   W _ZN22GameServerDemonManagerC1Ev/C2Ev           0x080f6330
//   W _ZN22GameServerDemonManager16GetNoticeMessageEv 0x0863bff6
//   T _ZN22GameServerDemonManager16SetNoticeMessageEhPc 0x080f6280
//   T _ZN22GameServerDemonManager19SetUserProhibitFlagEb 0x080f6222
//   W _ZN22GameServerDemonManager22IsProhibitConnectStateEv 0x084ed404
//   T _ZN22GameServerDemonManager26Notice10MinShutDownMessageEv 0x080f623a
// LinkClosureD：补具名字段/ctor/dtor（§8.3 权限，偏移实证，sizeof 0x101 不变）。
// Timer_NoticeEmergencyPatchMessage::dispatch_sig（报告
// docs/class_func_reports/Timer_NoticeEmergencyPatchMessage/dispatch_sig.md，
// ORIG 0x0863b6c4）与 Notice10MinShutDownMessage 等。
#ifndef DNF_GAME_GAME_SERVER_DEMON_MANAGER_H_
#define DNF_GAME_GAME_SERVER_DEMON_MANAGER_H_
#include <string.h>



class GameServerDemonManager
{
public:
    GameServerDemonManager();                      // ORIG C1/C2 0x080f6330 (W)
    ~GameServerDemonManager();                     // ORIG D1/D2 0x080f621c (T)

    // LinkClosureD 具名字段（ORIG C1/SetNoticeMessage 偏移实证，sizeof 0x101）。
    bool m_prohibitFlag;            // +0x00（IsProhibitConnectState 直返无 setne
                                    // 且调用点 test %al ⇒ bool getter 直返 bool 字段，P32）
    unsigned char m_noticeLen;      // +0x01
    char m_notice[0xff];            // +0x02..0x100
    char const* GetNoticeMessage();        // ORIG W 0x0863bff6
    void SetNoticeMessage(unsigned char, char*);   // ORIG T 0x080f6280
    void SetUserProhibitFlag(bool);        // ORIG T 0x080f6222
    bool IsProhibitConnectState();        // ORIG W 0x084ed404（movzbl 直返无 setne +
                                          // 调用点 test %al 无 setne = bool ABI，P32）
    void Notice10MinShutDownMessage();     // ORIG T 0x080f623a
};

GameServerDemonManager* GameserverDemonManagerInstance();  // ORIG T 0x080f61b8

#endif  // DNF_GAME_GAME_SERVER_DEMON_MANAGER_H_
