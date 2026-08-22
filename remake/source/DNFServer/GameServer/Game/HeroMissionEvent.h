#ifndef GAME_HEROMISSIONEVENT_H_
#define GAME_HEROMISSIONEVENT_H_

// ============================================================================
// df_game_r 还原 —— HeroMissionEvent（台湾 Hero Mission 事件，typo 批次）。
// 布局依据 ORIG ctor 0x08166522 / findMissionValue 0x08168010 反汇编推导：
//   基类 CEventBase（+0x00 vptr / +0x04 stADDINFO / +0x08 m_isEventing）
//   +0x0c map<MissionNo::T, BaseHeroMissionCondition*> m_conditions
//   +0x24 map<unsigned int, vector<HeroMissionValue>> m_values（按 charac_no）
// 本头不继承 CEventBase（避免引入 CGuildServerProxy.h 大依赖链），以布局
// 镜像字段保持 ORIG ABI；判活读 +0x08 标志，等价 ORIG IsEventing(NULL)
// （ORIG vptr+0x34 = CEventBase::IsEventing，实现即 movzbl +0x08）。
// ============================================================================

#include <map>
#include <vector>

class CUser;

namespace HeroMissionCondition
{
namespace MissionNo
{
// ORIG 参数类型：HeroMissionCondition::MissionNo::T（按值，enum）。
// mangled: N20HeroMissionCondition9MissionNo1TE
enum T
{
    T_0 = 0,
    T_4 = 4,
    T_6 = 6
};
}
}

// ---- HeroMissionValue（ORIG clear 0x08164b80：+0x00 int，+0x04..+0x0a ushort）----
class HeroMissionValue
{
public:
    int m_mission;            // +0x00（clear 置 0；findMissionValue 比对目标）
    unsigned short m_field4;  // +0x04
    unsigned short m_field6;  // +0x06
    unsigned short m_field8;  // +0x08
    unsigned short m_fieldA;  // +0x0a（完成进度；processMission 读取）
};

// ---- BaseHeroMissionCondition（ORIG vtable 0x08b7f0d8：虚析构 + 2 纯虚槽）----
// vptr+0x08 = 条件处理（派生类实现），vptr+0x0c = 第二个纯虚（派生类实现）。
class BaseHeroMissionCondition
{
public:
    virtual ~BaseHeroMissionCondition() {}
    virtual void process(CUser* user, HeroMissionValue* value,
                         unsigned int param) = 0;   // vptr+0x08
    virtual void process2(CUser* user, HeroMissionValue* value,
                          unsigned int param) = 0;  // vptr+0x0c
};

// ============================================================================
// HeroMissionEvent
// ============================================================================
class HeroMissionEvent
{
public:
    HeroMissionEvent();

    void processMission(CUser* user, HeroMissionCondition::MissionNo::T mission,
                        unsigned int param);   // ORIG 0x081672be
    bool findMissionValue(CUser* user, HeroMissionCondition::MissionNo::T mission,
                          HeroMissionValue*& out);   // ORIG 0x08168010

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

// ---- ARAD::Singleton 模板（与 GlobalData.cpp 定义逐字一致，ODR 合规）----
namespace ARAD
{
template <class T> class Singleton
{
public:
    static T* Get();
};

template <class T> T* Singleton<T>::Get()
{
    static T inst;
    return &inst;
}
}

#endif  // GAME_HEROMISSIONEVENT_H_
