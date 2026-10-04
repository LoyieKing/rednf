#ifndef DNF_COMMON_TIME_H_
#define DNF_COMMON_TIME_H_

#include <time.h>

// 全服务唯一的 CommonTime。
// ORIG SetCurTime（df_game_r 0x082a4224）写入：
//   +0x00..+0x05 年/月/日/时/分/星期
//   +0x08 m_curTime、+0x0c m_createTime（各 4 字节 long）
// 对象到 +0x10 结束。+0x49 的 int 是下一个字段，不是本对象的尾巴。
class CommonTime
{
public:
    void SetCreateTime(long t);
    void SetCreateTime(char* src);
    void SetCurTime();
    operator long() const;
    void clear();
    void GetCreateTime(char* buf) const;
    void MakeTime(tm& out) const;

    union
    {
        char m_time[6];
        struct
        {
            char m_year;    // +0x00
            char m_month;   // +0x01
            char m_day;     // +0x02
            char m_hour;    // +0x03
            char m_min;     // +0x04
            char m_wday;    // +0x05
        };
    };
    char m_pad6[2];         // +0x06
    long m_curTime;         // +0x08
    long m_createTime;      // +0x0c
};

#endif
