// HeroMissionEvent::processMission @ 0x081672be
// 独立事件 TU：承载 HeroMissionEvent 的本地类与实现。

class CUser;
namespace HeroMissionCondition { namespace MissionNo { enum T { T_0 = 0 }; } }

class HeroMissionEvent
{
public:
    void processMission(CUser* user, HeroMissionCondition::MissionNo::T mission,
                        unsigned int param);
};

void HeroMissionEvent::processMission(
    CUser* user, HeroMissionCondition::MissionNo::T mission, unsigned int param)
{
    void* self = this;
    if (!self || !user)
        return;

    // 事件状态位位于 CEventBase +0x08；避免调用未还原的虚表布局。
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(self);
    if (bytes[8] == 0)
        return;

    // 任务条件对象及角色值容器尚未在本批次还原；保留 ABI 入口，
    // 不伪造布局写内存。有效条件 TU 接入后在此继续分派。
    (void)mission;
    (void)param;
}
