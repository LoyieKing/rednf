// Reconstructed from secagent binary DWARF + disassembly.
// Original path: src/formmog/secagent/secagent_timer_handler.h
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_TIMER_HANDLER_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_TIMER_HANDLER_H_H_

#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include <stdint.h>

// sizeof = 36
struct SecAgentTimerHandler : public ZEN_Timer_Handler { // line 23
private:
    uint32_t cur_index_;              // line 33 @8
    uint32_t user_size_;              // line 36 @12
    SecAgentDPsdkInfoMgr::LPCONFIG conf_; // line 38 @16
    ZEN_Time_Value last_heartbeat_time_;  // line 40 @20
    ZEN_Time_Value last_check_user_time_; // line 41 @28
    static const uint64_t NO_EXPIRE_TIME_;         // line 44 = 300
    static const unsigned int HEARTBEAT_TIME_INTERVAL; // line 47 = 60
public:
    SecAgentTimerHandler(ZEN_Timer_Queue *timer_queue); // line 9
    virtual ~SecAgentTimerHandler(); // line 19
    virtual int handle_timeout(const ZEN_Time_Value &now, const void *act); // line 23
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_TIMER_HANDLER_H_H_
