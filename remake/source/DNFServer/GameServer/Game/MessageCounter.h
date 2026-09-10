#ifndef GAME_MESSAGECOUNTER_H_
#define GAME_MESSAGECOUNTER_H_

// ============================================================================
// df_game_r 还原 —— message_counter 域（包量统计容器/计时器）。
// ORIG 权威证据（df_game_r，报告 docs/class_func_reports/message_counter__*/）：
//   MessageKey C1/C2 (int,uchar,ushort) W 0x08570204；lt W 0x08570192
//   CUnixTimeInterval ctor W 0x085702d4；SetLastTime 0x08570236；
//     GetTimeInterval 0x08570254
//   CTimeInterval ctor W 0x085702c6（置 vptr，虚覆盖两函数为空）；Create T
//     0x0856f974 / Destory T 0x0856f9b0 / SetLastTime W 0x0857017c（空）/
//     GetTimeInterval W 0x08570182（返 0）
//   CMessageCounter C1/C2(i) W 0x0844de36；D1/D2 W 0x0845000e；
//     IncreaseMessage T 0x0856f9c4；WriteLog T 0x0856fbb0
// 布局：
//   MessageKey { int m_id@0; unsigned char m_type@4; unsigned short m_size@6 }
//     sizeof = 8。
//   MessageValue { int m_count@0; unsigned long long m_bytes@4(低@4高@8,
//     对齐后 low u32@4 high u32@8); double m_avg@0xc } sizeof = 0x14
//     （ORIG pair-> +0x08 count / +0x0c,+0x10 bytes / +0x14 avg）。
//   CTimeInterval 基类（vptr@0x00，timeval m_last @+0x04，sizeof 0xc；
//     ORIG SetLastTime 用 gettimeofday 写 this+0x04 实证字段位置）。
//   CUnixTimeInterval 派生（ctor @0x085702d4 调基类 ctor 后换 vptr），
//     覆盖 SetLastTime/GetTimeInterval 为真实现。
// ============================================================================

#include <map>
#include <sys/time.h>
#include <stddef.h>

namespace message_counter
{

class MessageKey
{
public:
    MessageKey(int id, unsigned char type, unsigned short size);

    bool operator<(const MessageKey& rhs) const;

    int            m_id;    // +0x00
    unsigned char  m_type;  // +0x04
    unsigned short m_size;  // +0x06
};                          // sizeof = 8

class MessageValue
{
public:
    // [b28d-SymMsgCtr] ORIG 无 _ZN15message_counter12MessageValueC[12]Ev ⇒
    // 原码为 POD（无用户 ctor）。零初始化经 value-init `MessageValue()` 完成，
    // 不发 ctor 符号，语义与原手写清零一致。

    int           m_count;      // pair+0x08
    // [R29-1] ORIG IncreaseMessage 0x0856fa8a 段对 +0x0c/+0x10 发
    // `mov 0xc(%ecx),%eax; mov 0x10(%ecx),%edx; add -0x98(%ebp),%eax;
    //  adc -0x94(%ebp),%edx; mov %eax,0xc(%ecx); mov %edx,0x10(%ecx)`
    // = 一次 64 位 `+=`（add/adc 对），故 bytes 为 64 位整型视角；
    // 低/高 32 位名保留（WriteLog 消费）。sizeof/偏移零变化。
    union
    {
        unsigned long long m_bytes;     // pair+0x0c（低@+0x0c 高@+0x10）
        struct
        {
            unsigned int  m_bytesLow;   // pair+0x0c
            unsigned int  m_bytesHigh;  // pair+0x10
        };
    };
    double        m_avg;        // pair+0x14
};

class CTimeInterval
{
public:
    CTimeInterval();                  // 仅设 vptr（ORIG 0x085702c6）
    virtual void SetLastTime();       // W 空覆盖 @0x0857017c
    virtual long long GetTimeInterval(); // W @0x08570182 返 0
    static CTimeInterval* Create();   // T 0x0856f974：new(0xc)+清零+ctor
    static void Destory(CTimeInterval* p);   // T 0x0856f9b0

protected:
    struct timeval m_last;            // +0x04
};                                    // sizeof = 0xc

// [R29-1] 布局锁（union 视角零影响验证；ORIG pair+0x08/+0x0c/+0x10/+0x14 实证）
static_assert(sizeof(MessageValue) == 0x14, "MessageValue size drift");
static_assert(offsetof(MessageValue, m_count) == 0x0, "m_count @+0x0");
static_assert(offsetof(MessageValue, m_bytes) == 0x4, "m_bytes @+0x4");
static_assert(offsetof(MessageValue, m_bytesLow) == 0x4, "m_bytesLow @+0x4");
static_assert(offsetof(MessageValue, m_bytesHigh) == 0x8, "m_bytesHigh @+0x8");
static_assert(offsetof(MessageValue, m_avg) == 0xc, "m_avg @+0xc");

class CUnixTimeInterval : public CTimeInterval
{
public:
    // [R29-2] 用户 ctor 已删（P299/CWG434）：ORIG nm 中 C1Ev/C2Ev 为 **W** size 0x1c
    // ⇒ 原码无用户提供默认 ctor；删后 `new CUnixTimeInterval()` 的 value-init
    // 前置清零（ORIG Create 0x0856f974 三 movl $0）才能发射。
    virtual void SetLastTime();       // W 0x08570236 gettimeofday→成员
    virtual long long GetTimeInterval(); // W 0x08570254 毫秒差取整
};

class CMessageCounter
{
public:
    explicit CMessageCounter(int idBias);
    ~CMessageCounter();

    void IncreaseMessage(unsigned int id, unsigned char type, unsigned short size,
                         long long bytes);
    void WriteLog();

private:
    std::map<MessageKey, MessageValue> m_map;   // +0x00
    int  m_idBias;                              // +0x18（WriteLog entry id 偏置；ORIG 实证 +0x18，无填充）
};

}  // namespace message_counter

#endif  // GAME_MESSAGECOUNTER_H_
