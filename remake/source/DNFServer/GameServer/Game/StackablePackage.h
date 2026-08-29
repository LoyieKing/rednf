#ifndef GAME_STACKABLEPACKAGE_H_
#define GAME_STACKABLEPACKAGE_H_

#include <cstddef>
#include <map>
#include <memory>
#include <utility>

class CUser;

namespace StackablePackage
{
enum EVENT_TYPE { EVENT_TYPE_0 = 0 };

class EventProcessor
{
public:
    virtual ~EventProcessor() {}
    virtual bool MatchEvent(int) const = 0;   // 【推断】槽2；bool 返回（operator() 透传无 setne + find_if xor $1 形态）
    virtual int Process(CUser&, int) = 0;    // 槽3

    EVENT_TYPE GetEventType() const { return m_eventType; }
    int GetEventId() const { return m_eventId; }

protected:
    EVENT_TYPE m_eventType;  // this+0x4（子类构造设置）
    int m_eventId;           // this+0x8（子类构造设置）
};

// HdrCreate3: FindControllableEventProcessor。Evidence: nm ORIG
// 0x08281c3a (C1(int) W, 0x0d) / 0x08281c48 (operator() W, 0x3a)。
// operator() 返回类型 bool：ORIG 调用侧 __find_if@0x082822a2 对返回值做
// xor $0x1/test %al（P22 bool 形态）；函数体透传 MatchEvent 无 setne 规范化
// （MatchEvent 亦为 bool）。
class FindControllableEventProcessor
{
public:
    explicit FindControllableEventProcessor(int eventId) : m_eventId(eventId) {}
    bool operator()(std::pair<int const, EventProcessor*> const& p) const
    {
        if (p.second == 0)
            return 0;
        return p.second->MatchEvent(m_eventId);
    }
private:
    int m_eventId;  // ctor(int) 保存的匹配键
};

class EventProcessManager
{
public:
    EventProcessManager();
    ~EventProcessManager();
    int Process(EVENT_TYPE, CUser&, int);
    void destroy();
    bool isStackablePackageItem(int) const;
    void registerEventProcessor(EventProcessor*);
    void setupEventProcessors();

    struct EventProcessManagerImpl;
    std::auto_ptr<EventProcessManagerImpl> m_impl;
};

static_assert(offsetof(EventProcessManager, m_impl) == 0,
              "EventProcessManager impl must be the first named field");
static_assert(sizeof(EventProcessManager) == sizeof(std::auto_ptr<EventProcessManager::EventProcessManagerImpl>),
              "EventProcessManager ABI drift");

// ORIG 为文件内 static（internal linkage，_ZN16StackablePackageL16G_EventProcessorEv，
// 3 个 TU 各一份 local t @0x08281730/0x08320224/0x084bba67）。
static EventProcessManager& G_EventProcessor();
}

#endif
