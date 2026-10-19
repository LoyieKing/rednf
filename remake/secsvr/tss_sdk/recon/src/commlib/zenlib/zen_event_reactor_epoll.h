// ZEN_Epoll_Reactor, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_event_reactor_epoll.cpp
// Address range 0x0019f160-0x0019fcd0.
//
// This header supersedes the reuse candidate
// (source/gunnersvr/src/commlib/zenlib/zen_event_reactor_epoll.h and its
// secagent/output twins), which declares initialize() virtual.  The ORIG
// vtable says otherwise:
//
//   _ZTV17ZEN_Epoll_Reactor (0x00293b40) = 40 bytes = 2 + 8 words
//     +0x08 D1  +0x0c D0  +0x10 ZEN_Reactor::close
//     +0x14 register_handler  +0x18 remove_handler  +0x1c cancel_wakeup
//     +0x20 schedule_wakeup   +0x24 handle_events
//
// i.e. exactly the six ZEN_Reactor virtuals (close inherited from the base,
// cancel_wakeup/schedule_wakeup/handle_events overridden here).  A virtual
// initialize() would add a seventh slot and produce a 44-byte vtable, which is
// what the reconstructed build emitted before this header existed
// (audit/report/diff.json objects: _ZTV17ZEN_Epoll_Reactor orig=40 ours=44).
// ORIG DWARF marks the member non-virtual too; the ORIG symbol
// _ZN17ZEN_Epoll_Reactor10initializeEjbi is a plain (non-vtable) function at
// 0x0019f3c0.  sizeof stays 44 either way (vptr + epoll_fd_ + edge_triggered_
// + once_max_events_ + once_events_ary_), so no layout changes.
#ifndef TSS_RECON_ZEN_EVENT_REACTOR_EPOLL_H_
#define TSS_RECON_ZEN_EVENT_REACTOR_EPOLL_H_

#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include <sys/epoll.h>

// sizeof = 44
struct ZEN_Epoll_Reactor : public ZEN_Reactor {
public:
    ZEN_Epoll_Reactor();
    ZEN_Epoll_Reactor(size_t max_event_number, bool edge_triggered, int once_max_events);
    virtual ~ZEN_Epoll_Reactor();
    // Non-virtual in ORIG: called directly by the four-argument constructor
    // and never through the vtable.
    int initialize(size_t max_event_number, bool edge_triggered, int once_max_events);
    virtual int register_handler(ZEN_Event_Handler *event_handler, int event_mask);
    virtual int remove_handler(ZEN_Event_Handler *event_handler, bool call_handle_close);
    virtual int cancel_wakeup(ZEN_Event_Handler *event_handler, int event_mask);
    virtual int schedule_wakeup(ZEN_Event_Handler *event_handler, int event_mask);
    virtual int handle_events(ZEN_Time_Value *time_out, size_t *size_io_event);
protected:
    void make_epoll_event(epoll_event *ep_event, ZEN_Event_Handler *event_handler) {
        ep_event->events = 0;
        ep_event->data.fd = event_handler->get_handle();
        if ((event_handler->get_mask() & ZEN_EVENT_READ) != 0) {
            ep_event->events |= EPOLLIN;
        }
        if ((event_handler->get_mask() & ZEN_EVENT_WRITE) != 0) {
            ep_event->events |= EPOLLOUT;
        }
        if ((event_handler->get_mask() & ZEN_EVENT_EXCEPT) != 0) {
            ep_event->events |= EPOLLERR;
        }
        if ((event_handler->get_mask() & ZEN_EVENT_ACCEPT) != 0) {
            ep_event->events |= EPOLLIN | EPOLLOUT;
        }
        if ((event_handler->get_mask() & ZEN_EVENT_PERSIST) != 0) {
            ep_event->events |= EPOLLIN;
        }
        if (edge_triggered_) {
            ep_event->events |= EPOLLET;
        }
    }
    void process_ready_event(epoll_event *ep_event);
protected:
    static const int DEFAULT_ONCE_TRIGGER_MAX_EVENT = 1024;
    int epoll_fd_;
    bool edge_triggered_;
    int once_max_events_;
    epoll_event *once_events_ary_;
};

#endif // TSS_RECON_ZEN_EVENT_REACTOR_EPOLL_H_
