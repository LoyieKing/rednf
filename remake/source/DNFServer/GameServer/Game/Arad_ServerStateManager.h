// df_game_r Game/ ARAD::Arad_ServerStateManager —— 权威头（13 方法全域还原批）。
// ORIG 类布局（ctor 0x81a922c / dtor 0x81a927e 反汇编反推）：
//   +0x00 bool m_flag（ctor 写 0；其余方法不读）
//   +0x04 Mutex m_lock（0x18，insertState/deleteState/sendState/checkContinus 加锁）
//   +0x1c std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage> m_stateMap
//   sizeof = 0x34（+0x1c + map 0x18；typedef 断言）。
#ifndef DNF_GAME_ARAD_SERVER_STATE_MANAGER_H_
#define DNF_GAME_ARAD_SERVER_STATE_MANAGER_H_

#include <map>

#include "GameTypes.h"               // Mutex / Guard / cMyTrace
#include "AradServerStateMessage.h"  // ORIG 12B packed 状态消息

// ENUM_GLOBAL_EFFECT_ARAD / ENUM_NOTIFY_EVENT_CATEGORY：
// ORIG 枚举全集未从二进制单独导出（仅作形参/键类型参与 mangled 与传值）；
// 此处落地占位枚举保证 ABI（mangled 23ENUM_GLOBAL_EFFECT_ARAD /
// 26ENUM_NOTIFY_EVENT_CATEGORY，sizeof=int）。运行期取值均经形参传入，
// 枚举项名为推断命名，数值不得随意引用为业务语义。
enum ENUM_GLOBAL_EFFECT_ARAD
{
    K_GLOBAL_EFFECT_ARAD_NONE = 0
};

enum ENUM_NOTIFY_EVENT_CATEGORY
{
    K_NOTIFY_EVENT_CATEGORY_SERVER_STAT = 1   // insertState/deleteState/checkContinus 恒传 1
};

class CUser;  // 前置：sendEffectPacket/sendState 形参仅用指针

namespace arad {
struct SigServerState;
}

namespace ARAD {

class SSendEffect;  // sendState 内部 for_each 函子（ORIG W 弱符号族，实现随 .cpp）

class Arad_ServerStateManager
{
public:
    Arad_ServerStateManager();   // ORIG 0x081a922c _ZN4ARAD23Arad_ServerStateManagerC1Ev
    ~Arad_ServerStateManager();  // ORIG 0x081a927e _ZN4ARAD23Arad_ServerStateManagerD1Ev

    void attachTimer();          // ORIG 0x081a98d4 _ZN4ARAD23Arad_ServerStateManager11attachTimerEv
    void checkContinus();        // ORIG 0x081a95dc _ZN4ARAD23Arad_ServerStateManager13checkContinusEv
    void deleteState(ENUM_GLOBAL_EFFECT_ARAD code);
                                 // ORIG 0x081a93d4 _ZN4ARAD23Arad_ServerStateManager11deleteStateE23ENUM_GLOBAL_EFFECT_ARAD

    // 重载一：Sig 内多条目批量导入（Guard 内逐条 insertStateNoSync）
    void insertState(arad::SigServerState* sig);
                                 // ORIG 0x081a92ca _ZN4ARAD23Arad_ServerStateManager11insertStateEPN4arad14SigServerStateE
    // 重载二：单条目导入
    void insertState(ENUM_GLOBAL_EFFECT_ARAD code, const AradServerStateMessage& msg);
                                 // ORIG 0x081a936e _ZN4ARAD23Arad_ServerStateManager11insertStateE23ENUM_GLOBAL_EFFECT_ARADRK22AradServerStateMessage

    void insertStateNoSync(ENUM_GLOBAL_EFFECT_ARAD code, const AradServerStateMessage& msg);
    bool findState(ENUM_GLOBAL_EFFECT_ARAD code, AradServerStateMessage& out);
                                 // ORIG 0x081a9c2a _ZN4ARAD23Arad_ServerStateManager9findStateE23ENUM_GLOBAL_EFFECT_ARADR22AradServerStateMessage（ORIG 非 const）
                                 // ORIG 0x081a991a _ZN4ARAD23Arad_ServerStateManager17insertStateNoSyncE23ENUM_GLOBAL_EFFECT_ARADRK22AradServerStateMessage
    void loadServerState();      // ORIG 0x081a97f8 _ZN4ARAD23Arad_ServerStateManager15loadServerStateEv
    void saveServerState(ENUM_NOTIFY_EVENT_CATEGORY category, ENUM_GLOBAL_EFFECT_ARAD code,
                         const AradServerStateMessage& msg, bool endFlag);
                                 // ORIG 0x081a9afa _ZN4ARAD23Arad_ServerStateManager15saveServerStateE26ENUM_NOTIFY_EVENT_CATEGORY23ENUM_GLOBAL_EFFECT_ARADRK22AradServerStateMessageb
    void sendEffectPacket(ENUM_GLOBAL_EFFECT_ARAD code, const AradServerStateMessage& msg, CUser* user);
                                 // ORIG 0x081a9a1c _ZN4ARAD23Arad_ServerStateManager16sendEffectPacketE23ENUM_GLOBAL_EFFECT_ARADRK22AradServerStateMessageP5CUser
    void sendState(CUser* user); // ORIG 0x081a9500 _ZN4ARAD23Arad_ServerStateManager9sendStateEP5CUser

private:
    bool              m_flag;      // +0x00
    Mutex             m_lock;      // +0x04（0x18 字节）
    std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage> m_stateMap;  // +0x1c
};
typedef char Arad_ServerStateManager_size_check[(sizeof(Arad_ServerStateManager) == 0x34) ? 1 : -1];

}  // namespace ARAD

#endif  // DNF_GAME_ARAD_SERVER_STATE_MANAGER_H_
