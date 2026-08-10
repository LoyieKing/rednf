// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE.
// Original path: src/commlib/zenlib/zen_event_reactor_epoll.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_REACTOR_EPOLL_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_REACTOR_EPOLL_H_H_

#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include <sys/epoll.h>

// sizeof = 44
struct ZEN_Epoll_Reactor : public ZEN_Reactor {
public:
    ZEN_Epoll_Reactor();
    ZEN_Epoll_Reactor(size_t max_event_number, bool edge_triggered, int once_max_events);
    virtual ~ZEN_Epoll_Reactor();
    virtual int initialize(size_t max_event_number, bool edge_triggered, int once_max_events);
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

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_REACTOR_EPOLL_H_H_
