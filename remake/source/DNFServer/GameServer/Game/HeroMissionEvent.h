#ifndef GAME_HEROMISSIONEVENT_H_
#define GAME_HEROMISSIONEVENT_H_

// ============================================================================
// df_game_r 还原 —— HeroMissionEvent（台湾 Hero Mission 事件）。
// 布局依据 ORIG ctor 0x08166522 / findMissionValue 0x08168010 反汇编推导：
//   基类 CEventBase（+0x00 vptr / +0x04 stADDINFO / +0x08 m_isEventing）
//   +0x0c map<MissionNo::T, BaseHeroMissionCondition*> m_conditions
//   +0x24 map<unsigned int, vector<HeroMissionValue>> m_values（按 charac_no）
// 本头不继承 CEventBase（避免引入 CGuildServerProxy.h 大依赖链），以布局
// 镜像字段保持 ORIG ABI；判活读 +0x08 标志，等价 ORIG IsEventing(NULL)
// （ORIG vptr+0x34 = CEventBase::IsEventing，实现即 movzbl +0x08）。
// ============================================================================

#include <map>
#include <string>
#include <vector>

#include "Arad_DataManager.h"   // ARAD::Singleton 模板唯一声明点（定义/实例化见 ARAD.cpp）
#include "HeroMissionCondition.h" // HeroMissionCondition / BaseHeroMissionCondition
                                  // 权威定义（W9_W7HS 批次；本头不再重复声明）
#include "CGuildServerProxy.h"   // struct Word_Param（StartEvent 参数类型；LT07 批次引入）


// ---- HeroMissionValue（ORIG clear 0x08164b80：+0x00 int，+0x04..+0x0a ushort）----
class HeroMissionValue
{
public:
    int m_mission;            // +0x00（clear 置 0；findMissionValue 比对目标）
    unsigned short m_field4;  // +0x04
    unsigned short m_field6;  // +0x06
    unsigned short m_field8;  // +0x08
    unsigned short m_fieldA;  // +0x0a（完成进度；processMission 读取）

    // ORIG _ZN16HeroMissionValue5clearEv T 0x08164b80（全字段清零）
    void clear();
    // ORIG _ZN16HeroMissionValue14setStringTokenESs T 0x08164d68（按值收 token 串，
    // 解析 "mission,cur,max,flag" 形式；定义归 HeroMission 域批次）
    void setStringToken(std::string token);
    // ORIG _ZN16HeroMissionValue5resetEv T 0x08164bb2（resetMissionForGm 依赖）。
    void reset();
};

// ============================================================================
// HeroMissionEvent
// ============================================================================
class HeroMissionEvent
{
public:
    HeroMissionEvent();
    void sendMissionData(CUser* user);                    // ORIG 0x0816770e
    void setEventData(CUser* user, char const* data);     // ORIG 0x08166752
    // （另有 std::string const& 重载 ORIG 0x08166d0e，不在本批）

    void processMission(CUser* user, HeroMissionCondition::MissionNo::T mission,
                        unsigned int param);   // ORIG 0x081672be
    bool findMissionValue(CUser* user, HeroMissionCondition::MissionNo::T mission,
                          HeroMissionValue*& out);   // ORIG 0x08168010
    // ---- W8_LongTail Batch07_VendingHero 批次（Game/W8LT_VendingHero.cpp）----
    // Word_Param 定义见 CGuildServerProxy.h（ORIG StartEvent(Word_Param) 参数类型）。
    bool loadScript();                                            // ORIG 0x08167ac8
    void StartEvent();                                            // ORIG 0x0816664a
    // ORIG 0x081666e6：仅经 vptr+0x08 调用自身 StartEvent()（thunk 形态）。
    void StartEvent(Word_Param param);
    void EndEvent();                                              // ORIG 0x08166700
    ~HeroMissionEvent();                                          // ORIG D0 0x0816662c
    void saveEventData(CUser* user, bool removeData);             // ORIG 0x08166d38
    void loadEventData(CUser* user);                              // ORIG 0x0816709e
    bool rewardMission(CUser* user,
                       HeroMissionCondition::MissionNo::T mission); // ORIG 0x0816748e
    // ORIG 0x08166d0e：std::string::c_str 后转 char const* 重载。
    void setEventData(CUser* user, const std::string& data);

    void resetMissionForGm(CUser* user,
                           HeroMissionCondition::MissionNo::T mission);          // ORIG 0x08167a3a
    void setMissionForGm(CUser* user, HeroMissionCondition::MissionNo::T mission,
                         unsigned int value);                                    // ORIG 0x081679ac

private:
    // ORIG 布局镜像（CEventBase 基类）
    char m_vptr[4];            // +0x00
    unsigned short m_addInfo0; // +0x04（stADDINFO）
    unsigned short m_addInfo1; // +0x06（stADDINFO）
    char m_isEventing;         // +0x08
    char m_pad09[3];           // +0x09..+0x0b
    std::map<HeroMissionCondition::MissionNo::T, BaseHeroMissionCondition*>
        m_conditions;          // +0x0c
    char m_pad14[0x10];        // +0x14..+0x23
    std::map<unsigned int, std::vector<HeroMissionValue> >
        m_values;              // +0x24
};

#endif  // GAME_HEROMISSIONEVENT_H_
