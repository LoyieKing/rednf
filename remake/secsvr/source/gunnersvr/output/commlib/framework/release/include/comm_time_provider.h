// 还原头文件：Comm_Time_Provider（单例，缓存当前时间；三个取值/更新方法为 inline）
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TIME_PROVIDER_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TIME_PROVIDER_H_H_

#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include <time.h>
#include <sys/time.h>

// sizeof = 8
struct Comm_Time_Provider { // line 14
private:
    ZEN_Time_Value cur_time_;              // line 42 (0x00)
    static Comm_Time_Provider *instance_;  // line 45
public:
    static Comm_Time_Provider *instance(); // line 17
    static void clean_instance(); // line 18
    time_t time() const { // line 21
        return cur_time_.sec();
    }
    timeval gettimeofday() const { // line 27
        return cur_time_;
    }
    void update(const ZEN_Time_Value &arg0) { // line 33
        cur_time_ = arg0;
    }
private:
    Comm_Time_Provider(); // line 39
    ~Comm_Time_Provider(); // line 40
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TIME_PROVIDER_H_H_
