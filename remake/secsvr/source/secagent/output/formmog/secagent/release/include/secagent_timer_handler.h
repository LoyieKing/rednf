// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_timer_handler.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_TIMER_HANDLER_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_TIMER_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include <stdint.h>

// sizeof = 36
struct SecAgentTimerHandler : public ZEN_Timer_Handler { // line 23
private:
uint32_t cur_index_;
uint32_t user_size_;
SecAgentDPsdkInfoMgr::LPCONFIG conf_;
ZEN_Time_Value last_heartbeat_time_;
ZEN_Time_Value last_check_user_time_;
static const uint64_t NO_EXPIRE_TIME_;
static const unsigned int HEARTBEAT_TIME_INTERVAL;
public:
SecAgentTimerHandler(const SecAgentTimerHandler &arg0);
SecAgentTimerHandler(ZEN_Timer_Queue *arg0); // line 9
virtual ~SecAgentTimerHandler(); // line 19
virtual int handle_timeout(const ZEN_Time_Value &now, const void *act); // line 23
};
extern const uint64_t NO_EXPIRE_TIME_; // line 44
extern const unsigned int HEARTBEAT_TIME_INTERVAL; // line 47


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_TIMER_HANDLER_H_H_
