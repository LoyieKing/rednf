// Restored from gunnersvr oracle binary (DWARF + disassembly), GCC 4.1.0 SUSE.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_event_reactor_base.cpp

#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"

#include <utility>

// TU-local declaration; real class lives in zen_trace_log_msg.{h,cpp}
// (its stub header is still being restored by another task).
struct ZEN_Trace_LogMsg {
    static void debug_errorex(const char *str_format, ...);
};

ZEN_Reactor *ZEN_Reactor::instance_ = 0;

ZEN_Reactor::ZEN_Reactor() : handler_map_() {
    max_event_number_ = 1024;
    initialize(max_event_number_);
}

ZEN_Reactor::ZEN_Reactor(size_t max_event_number) : handler_map_() {
    max_event_number_ = max_event_number;
    initialize(max_event_number_);
}

ZEN_Reactor::~ZEN_Reactor() {
}

size_t ZEN_Reactor::size() {
    return handler_map_.size();
}

size_t ZEN_Reactor::max_size() {
    return max_event_number_;
}

int ZEN_Reactor::initialize(size_t max_event_number) {
    max_event_number_ = max_event_number;
    handler_map_.resize(max_event_number);
    return 0;
}

// Oracle walks the hash buckets: repeatedly takes the first non-empty bucket
// head and calls handle_close() on it (restarting from bucket 0), then clears.
// `handler_map_.begin()` has exactly the same observable semantics.
int ZEN_Reactor::close() {
    int iter_temp = 0;
    ZEN_Event_Handler *event_handler = 0;

    while (handler_map_.size() != 0) {
        event_handler = handler_map_.begin()->second;
        event_handler->handle_close();
    }
    handler_map_.clear();
    return 0;
}

int ZEN_Reactor::register_handler(ZEN_Event_Handler *event_handler, int event_mask) {
    int ret = 0;
    ZEN_SOCKET socket_hd = 0;
    ZEN_Event_Handler *tmp_handler = 0;

    if (handler_map_.size() >= max_event_number_) {
        return -1;
    }
    socket_hd = event_handler->get_handle();
    if (find_event_handler(socket_hd, tmp_handler) == 0) {
        return -1;
    }
    handler_map_.insert(std::make_pair(socket_hd, event_handler));
    if (event_mask != 0) {
        schedule_wakeup(event_handler, event_mask);
    }
    return 0;
}

int ZEN_Reactor::remove_handler(ZEN_Event_Handler *event_handler, bool call_handle_close) {
    int ret = 0;
    ZEN_SOCKET socket_hd = 0;
    ZEN_Event_Handler *tmp_handler = 0;
    int event_mask = 0;

    socket_hd = event_handler->get_handle();
    if (find_event_handler(socket_hd, tmp_handler) != 0) {
        return -1;
    }
    event_mask = event_handler->get_mask();
    if (event_mask != 0) {
        cancel_wakeup(event_handler, event_mask);
    }
    socket_hd = event_handler->get_handle();
    handler_map_.erase(socket_hd);
    if (call_handle_close) {
        event_handler->handle_close();
    }
    return 0;
}

int ZEN_Reactor::cancel_wakeup(ZEN_Event_Handler *event_handler, int event_mask) {
    event_handler->disable_mask(event_mask);
    return 0;
}

int ZEN_Reactor::schedule_wakeup(ZEN_Event_Handler *event_handler, int event_mask) {
    event_handler->enable_mask(event_mask);
    return 0;
}

ZEN_Reactor *ZEN_Reactor::instance() {
    return instance_;
}

void ZEN_Reactor::clean_instance() {
    if (instance_ != 0) {
        delete instance_;
    }
    instance_ = 0;
}

void ZEN_Reactor::instance(ZEN_Reactor *pinstatnce) {
    clean_instance();
    instance_ = pinstatnce;
}
