// Reconstructed from gunnersvr disassembly.
#include "src/commsvr/gunnersvr/gunner_timer_handler.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_time.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "src/commsvr/gunnersvr/gunner_reporter.h"

#include <sys/time.h>

GunnerTimerHandler::GunnerTimerHandler(ZEN_Timer_Queue *timer_queue)
    : ZEN_Timer_Handler(timer_queue) {
}

GunnerTimerHandler::~GunnerTimerHandler() {
}

int GunnerTimerHandler::handle_timeout(const ZEN_Time_Value &now, const void *act) {
    timeval tv;
    char time_str[64];

    tv.tv_sec = now.sec();
    tv.tv_usec = 0;
    ZEN_OS::timestamp(&tv, time_str, 0x3f);
    ZEN_Trace_LogMsg::debug_infoex("the reporting time = %s", time_str);
    GunnerInfoReporter::instance()->report(now.sec());
    return 0;
}
