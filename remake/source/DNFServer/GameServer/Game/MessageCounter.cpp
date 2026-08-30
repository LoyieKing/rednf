// MessageCounter.cpp —— 按报告 docs/class_func_reports/message_counter__*/*.md 忠实转写。
#include "MessageCounter.h"

#include <new>
#include <string.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "SigTypes.h"
#include "CSystemTime.h"

namespace message_counter
{

// ---- ORIG 域常量 _ZN15message_counterL13UPDATE_PERIODE（r 0x08c5a78c = 600）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int UPDATE_PERIOD = 600;

// SIG_INSERT_RESPONSE_TIME（0x7d8）访问视图：
//   +0x00 int  baseSec；+0x04 int count；
//   entries[100] 每 0x14 @+8 + j*0x14：
//     {+0x00 uint id; +0x04 int count; +0x08 int bytesLow;
//      +0x0c int bytesHigh; +0x10 int avgRounded}

namespace
{
inline void sig_init_head(char* sig, int baseSec)
{
    *reinterpret_cast<int*>(sig) = baseSec;
    *reinterpret_cast<int*>(sig + 4) = 0;
}
inline int sig_entry_count(const char* sig)
{
    return *reinterpret_cast<const int*>(sig + 4);
}
inline void sig_bump_count(char* sig)
{
    ++*reinterpret_cast<int*>(sig + 4);
}
void sig_write_entry(char* sig, int slot, unsigned int id, int count,
                     unsigned int bytesLow, unsigned int bytesHigh,
                     int avgRounded)
{
    char* e = sig + 8 + slot * 0x14;
    *reinterpret_cast<unsigned int*>(e + 0x00) = id;
    *reinterpret_cast<int*>(e + 0x04) = count;
    *reinterpret_cast<unsigned int*>(e + 0x08) = bytesLow;
    *reinterpret_cast<unsigned int*>(e + 0x0c) = bytesHigh;
    *reinterpret_cast<int*>(e + 0x10) = avgRounded;
}
}  // namespace

// ===================== MessageKey =====================

// ORIG @0x08570204。
MessageKey::MessageKey(int id, unsigned char type, unsigned short size)
    : m_id(id), m_type(type), m_size(size)
{
}

// ORIG @0x08570192：先比 id（signed，< 返 1）；相等再比 type（unsigned char，
// jae→0 / < 返 1）；相等再比 size（unsigned short，同上），否则返 0。
bool MessageKey::operator<(const MessageKey& rhs) const
{
    if (m_id < rhs.m_id)
        return true;
    if (m_id != rhs.m_id)
        return false;
    unsigned int t = (unsigned char)m_type;
    unsigned int r = (unsigned char)rhs.m_type;
    if (t < r)
        return true;
    return m_size < rhs.m_size;
}
// ===================== CTimeInterval（基类） =====================

// ORIG @0x085702c6：仅设 vptr。
CTimeInterval::CTimeInterval()
{
}

CTimeInterval* CTimeInterval::Create()
{
    // ORIG @0x0856f974：operator new(0xc) → 三槽 u32 清零 →
    // 构造派生 CUnixTimeInterval（ORIG call _ZN15message_counter17CUnixTimeIntervalC1Ev，
    // 置派生 vtable；基类构造为空实现）→ 返回。
    CTimeInterval* raw = static_cast<CTimeInterval*>(::operator new(0xc));
    *reinterpret_cast<unsigned int*>(raw) = 0;
    raw->m_last.tv_sec = 0;
    raw->m_last.tv_usec = 0;
    new (raw) CUnixTimeInterval();
    return raw;
}

void CTimeInterval::Destory(CTimeInterval* p)
{
    // ORIG @0x0856f9b0：operator delete(param)，无析构链。
    ::operator delete(p);
}

void CTimeInterval::SetLastTime() {}            // W 空覆盖 @0x0857017c

long long CTimeInterval::GetTimeInterval()      // W 空覆盖 @0x08570182
{
    return 0;
}

// ===================== CUnixTimeInterval（派生） =====================

CUnixTimeInterval::CUnixTimeInterval() : CTimeInterval()
{
}

// ORIG @0x08570236：gettimeofday(&this->m_last, 0)。
void CUnixTimeInterval::SetLastTime()
{
    gettimeofday(&m_last, 0);
}
// ORIG @0x08570254：(now-last)_sec*1000 + (now-last)_usec/1000.0，
// FP 取整后转 long long 返回。
long long CUnixTimeInterval::GetTimeInterval()
{
    struct timeval now;
    gettimeofday(&now, 0);

    double millis =
        (double)((int)(now.tv_sec - m_last.tv_sec) * 1000) +
        ((double)((int)(now.tv_usec - m_last.tv_usec)) / 1000.0);
    long long result = (long long)millis;
    if (millis - (double)result >= 0.5)
        ++result;
    return result;
}
// ===================== CMessageCounter =====================

// ORIG ctor @0x0844de36：map 默认构造 + this+0x18=param。
CMessageCounter::CMessageCounter(int idBias)
    : m_map(),
      m_idBias(idBias)
{
}

CMessageCounter::~CMessageCounter() {}

// ORIG IncreaseMessage @0x0856f9c4：命中则 ++count → 64 位累加耗时字节 →
// 滑动平均 avg = newBytes*(1/n') + oldAvg*((n'-1)/n')；未命中插入全零记录。
void CMessageCounter::IncreaseMessage(unsigned int id, unsigned char type,
                                      unsigned short size, long long bytes)
{
    MessageKey key((int)id, type, size);
    std::map<MessageKey, MessageValue>::iterator it = m_map.find(key);
    if (it == m_map.end())
    {
        m_map.insert(std::make_pair(key, MessageValue()));
        return;
    }

    MessageValue& value = it->second;
    ++value.m_count;

    unsigned int addLow = (unsigned int)(unsigned long long)bytes;
    unsigned int addHigh = (unsigned int)((unsigned long long)bytes >> 32);
    unsigned int beforeLow = value.m_bytesLow;
    value.m_bytesLow += addLow;
    value.m_bytesHigh += addHigh;
    if (value.m_bytesLow < beforeLow)
        ++value.m_bytesHigh;                    // CARRY4 汇入高位

    double wNew = 1.0 / (double)value.m_count;                  // 1/n'
    double wOld = (double)(value.m_count - 1) / (double)value.m_count;
    value.m_avg = (double)bytes * wNew + value.m_avg * wOld;
}

// ORIG WriteLog @0x0856fbb0：LOGDB_Q(#4) 投递 SIG_INSERT_RESPONSE_TIME；
// 满 100 条续包；SIG: int baseSec@0 / int count@4 /
// entries[100]@+8 每 0x14。收尾 map.clear()。
void CMessageCounter::WriteLog()
{
    if (m_map.empty())
        return;

    StreamPool& pool = *GlobalData::s_stream_pool;
    MsgQueueMgr& queueMgr = *GlobalData::s_msgq_mgr;

    Stream* stream = pool.Acquire("MessageCounter.cpp", 99);
    CStreamGuard guard(stream, false);

    guard << 0x26c;
    guard << -1;

    char* sig = reinterpret_cast<char*>(
        guard.GetInBuffer<SIG_INSERT_RESPONSE_TIME>());

    int nowSec = GlobalData::s_systemTime_.getCurSec();
    int baseSec = nowSec - nowSec % UPDATE_PERIOD;
    sig_init_head(sig, baseSec);

    for (std::map<MessageKey, MessageValue>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        const MessageKey& key = it->first;
        const MessageValue& value = it->second;
        int slot = sig_entry_count(sig);

        unsigned int idField =
            (key.m_id == 0)
                ? 0u
                : (unsigned int)(key.m_id + m_idBias);

        sig_write_entry(sig, slot,
                        idField + (unsigned int)key.m_size,
                        value.m_count,
                        value.m_bytesLow, value.m_bytesHigh,
                        (int)(value.m_avg + (value.m_avg >= 0 ? 0.5 : -0.5)));
        sig_bump_count(sig);

        if (sig_entry_count(sig) == 100)
        {
            queueMgr.put(MsgQueueMgr::LOGDB_Q, guard);

            stream = pool.Acquire("MessageCounter.cpp", 0x7b);
            guard.Set(stream);
            guard << 0x26c;
            guard << -1;
            sig = reinterpret_cast<char*>(
                guard.GetInBuffer<SIG_INSERT_RESPONSE_TIME>());
            sig_init_head(sig, baseSec);
        }
    }

    if (sig_entry_count(sig) == 0)
    {
        pool.Free(guard);
    }
    else
    {
        queueMgr.put(MsgQueueMgr::LOGDB_Q, guard);
    }

    m_map.clear();
}

}  // namespace message_counter
