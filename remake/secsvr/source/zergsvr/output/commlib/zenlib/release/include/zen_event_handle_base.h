// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE.
// Original path: src/commlib/zenlib/zen_event_handle_base.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_HANDLE_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_HANDLE_BASE_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

struct ZEN_Reactor;

// Event mask bits as observed in the gunnersvr oracle (mask -> epoll events):
//   0x02 -> EPOLLIN, 0x04 -> EPOLLOUT, 0x08 -> EPOLLERR,
//   0x10 -> EPOLLIN|EPOLLOUT, 0x20 -> EPOLLIN
enum ZEN_EVENT_MASK {
    ZEN_EVENT_READ = 0x02,
    ZEN_EVENT_WRITE = 0x04,
    ZEN_EVENT_EXCEPT = 0x08,
    ZEN_EVENT_ACCEPT = 0x10,
    ZEN_EVENT_PERSIST = 0x20,
};

// sizeof = 12 (vptr + zen_reactor_ + event_mask_)
// Virtual members are declared only (never defined inline): the gunnersvr
// binary has no ZEN_Event_Handler out-of-line symbols and no derived handler
// vtable, so the original header declared them the same way.
struct ZEN_Event_Handler {
public:
    ZEN_Event_Handler();
    ZEN_Event_Handler(ZEN_Reactor *reactor);
    virtual ~ZEN_Event_Handler();
    virtual ZEN_SOCKET get_handle() const = 0;
    virtual int handle_input();
    virtual int handle_output();
    virtual int handle_exception();
    virtual int handle_close();
    int get_mask() { return event_mask_; }
    void set_mask(int event_mask) { event_mask_ = event_mask; }
    void enable_mask(int event_mask) { event_mask_ |= event_mask; }
    void disable_mask(int event_mask) { event_mask_ &= ~event_mask; }
    virtual void reactor(ZEN_Reactor *reactor);
    virtual ZEN_Reactor *reactor() const;
protected:
    ZEN_Reactor *zen_reactor_;
    int event_mask_;
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_HANDLE_BASE_H_H_
