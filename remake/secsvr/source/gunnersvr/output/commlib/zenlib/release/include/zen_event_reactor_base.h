// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE.
// Original path: src/commlib/zenlib/zen_event_reactor_base.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_REACTOR_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_REACTOR_BASE_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <ext/hash_map>
#include <stddef.h>

struct ZEN_Time_Value;

// sizeof = 28
struct ZEN_Reactor : public ZEN_NON_Copyable {
public:
    ZEN_Reactor(const ZEN_Reactor &arg0);
protected:
    ZEN_Reactor();
    ZEN_Reactor(size_t max_event_number);
public:
    virtual ~ZEN_Reactor();
    size_t size();
    size_t max_size();
    int initialize(size_t max_event_number);
    virtual int close();
    virtual int register_handler(ZEN_Event_Handler *event_handler, int event_mask);
    virtual int remove_handler(ZEN_Event_Handler *event_handler, bool call_handle_close);
    // Pure virtual in the oracle vtable (slots point to __cxa_pure_virtual);
    // cancel_wakeup/schedule_wakeup also have out-of-line definitions used by
    // ZEN_Epoll_Reactor via qualified calls.
    virtual int cancel_wakeup(ZEN_Event_Handler *event_handler, int event_mask) = 0;
    virtual int schedule_wakeup(ZEN_Event_Handler *event_handler, int event_mask) = 0;
    virtual int handle_events(ZEN_Time_Value *arg0, size_t *arg1) = 0;
protected:
    int exist_event_handler(ZEN_Event_Handler *event_handler) {
        ZEN_Event_Handler *tmp_handler = 0;
        return find_event_handler(event_handler->get_handle(), tmp_handler);
    }
    int find_event_handler(ZEN_SOCKET socket_hd, ZEN_Event_Handler *&event_handler) {
        __gnu_cxx::hash_map<int, ZEN_Event_Handler *>::iterator iter = handler_map_.find(socket_hd);
        if (iter != handler_map_.end()) {
            event_handler = iter->second;
            return 0;
        }
        return -1;
    }
public:
    static ZEN_Reactor *instance();
    static void clean_instance();
    static void instance(ZEN_Reactor *pinstatnce);
protected:
    __gnu_cxx::hash_map<int, ZEN_Event_Handler *> handler_map_;
    size_t max_event_number_;
    static ZEN_Reactor *instance_;
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_REACTOR_BASE_H_H_
