// 还原实现：comm_time_provider.cpp（语义对照 gunnersvr oracle 0x080887b0-0x080889b0）
#include "src/commlib/framework/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "src/commlib/framework/comm_time_provider.h"

Comm_Time_Provider *Comm_Time_Provider::instance_ = 0;

// line 30
Comm_Time_Provider::Comm_Time_Provider() {
    cur_time_.gettimeofday();
}

// line 36
Comm_Time_Provider::~Comm_Time_Provider() {
}

// line 23
void Comm_Time_Provider::clean_instance() {
    if (instance_ != 0) {
        delete instance_;
        instance_ = 0;
    }
}

// line 13
Comm_Time_Provider *Comm_Time_Provider::instance() {
    if (instance_ == 0) {
        instance_ = new Comm_Time_Provider;
    }
    return instance_;
}
