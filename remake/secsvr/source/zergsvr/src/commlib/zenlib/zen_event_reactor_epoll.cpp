// Restored from gunnersvr oracle binary (DWARF + disassembly), GCC 4.1.0 SUSE.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_event_reactor_epoll.cpp

#include "src/commlib/zenlib/zen_event_reactor_epoll.h"
#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_os_adapt_error.h"

#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

// TU-local declaration; real class lives in zen_trace_log_msg.{h,cpp}
// (its stub header is still being restored by another task).
struct ZEN_Trace_LogMsg {
    static void debug_errorex(const char *str_format, ...);
    static void debug_infoex(const char *str_format, ...);
};

ZEN_Epoll_Reactor::ZEN_Epoll_Reactor()
    : ZEN_Reactor(1024) {
    epoll_fd_ = -1;
    edge_triggered_ = false;
    once_max_events_ = DEFAULT_ONCE_TRIGGER_MAX_EVENT;
    once_events_ary_ = 0;
}

ZEN_Epoll_Reactor::ZEN_Epoll_Reactor(size_t max_event_number, bool edge_triggered, int once_max_events)
    : ZEN_Reactor(max_event_number) {
    edge_triggered_ = edge_triggered;
    once_max_events_ = once_max_events;
    once_events_ary_ = 0;
    initialize(max_event_number, edge_triggered, once_max_events);
}

ZEN_Epoll_Reactor::~ZEN_Epoll_Reactor() {
    ::close(epoll_fd_);
    if (once_events_ary_ != 0) {
        delete[] once_events_ary_;
    }
}

int ZEN_Epoll_Reactor::initialize(size_t max_event_number, bool edge_triggered, int once_max_events) {
    int ret = 0;

    ret = ZEN_Reactor::initialize(max_event_number);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] Epoll reactor ZEN_Reactor::initialize fail.please check code. ret = %u.", ret);
        return ret;
    }
    edge_triggered_ = edge_triggered;
    epoll_fd_ = epoll_create(max_event_number_ + 64);
    if (epoll_fd_ < 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] Epoll reactor ::epoll_create fail.please check code. error = [%u|%u]",
            errno, strerror(errno));
        return -1;
    }
    once_max_events_ = once_max_events;
    once_events_ary_ = new epoll_event[once_max_events];
    return 0;
}

int ZEN_Epoll_Reactor::register_handler(ZEN_Event_Handler *event_handler, int event_mask) {
    int ret = 0;
    epoll_event ep_event;

    ret = ZEN_Reactor::register_handler(event_handler, 0);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] ZEN_Reactor::register_handler fail. please check you code .ret =%d", ret);
        return -1;
    }
    event_handler->set_mask(event_mask);
    make_epoll_event(&ep_event, event_handler);
    ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event_handler->get_handle(), &ep_event);
    if (ret == 0) {
        return 0;
    }
    ret = ZEN_Reactor::remove_handler(event_handler, false);
    ZEN_Trace_LogMsg::debug_errorex(
        "[zenlib] [%s] Epoll reactor ::epoll_ctl fail.please check code. ret =%d error = [%u|%s]",
        __PRETTY_FUNCTION__, ret, errno, strerror(errno));
    return -1;
}

int ZEN_Epoll_Reactor::remove_handler(ZEN_Event_Handler *event_handler, bool call_handle_close) {
    int ret = 0;
    epoll_event event;

    event.events = 0;
    ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event_handler->get_handle(), &event);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] [%s] epoll reactor ::epoll_ctl fail.please check code. ret =%d error = [%u|%s]",
            __PRETTY_FUNCTION__, ret, errno, strerror(errno));
    }
    event_handler->set_mask(0);
    ret = ZEN_Reactor::remove_handler(event_handler, call_handle_close);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[zenlib] ZEN_Reactor::remove_handler fail. please check you code .ret =%u", ret);
        return -1;
    }
    return 0;
}

int ZEN_Epoll_Reactor::cancel_wakeup(ZEN_Event_Handler *event_handler, int event_mask) {
    int ret = 0;
    epoll_event ep_event;

    ret = ZEN_Reactor::cancel_wakeup(event_handler, event_mask);
    if (ret != 0) {
        return -1;
    }
    make_epoll_event(&ep_event, event_handler);
    ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, event_handler->get_handle(), &ep_event);
    if (ret == 0) {
        return 0;
    }
    ZEN_Reactor::schedule_wakeup(event_handler, event_mask);
    ZEN_Trace_LogMsg::debug_errorex(
        "[zenlib] [%s] epoll reactor ::epoll_ctl fail.please check code. ret =%d error = [%u|%s]",
        __PRETTY_FUNCTION__, ret, errno, strerror(errno));
    return -1;
}

int ZEN_Epoll_Reactor::schedule_wakeup(ZEN_Event_Handler *event_handler, int event_mask) {
    int ret = 0;
    epoll_event ep_event;

    ret = ZEN_Reactor::schedule_wakeup(event_handler, event_mask);
    if (ret != 0) {
        return -1;
    }
    make_epoll_event(&ep_event, event_handler);
    ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, event_handler->get_handle(), &ep_event);
    if (ret == 0) {
        return 0;
    }
    ZEN_Reactor::cancel_wakeup(event_handler, event_mask);
    ZEN_Trace_LogMsg::debug_errorex(
        "[zenlib] [%s] epoll reactor ::epoll_ctl fail.please check code. ret =%d error = [%u|%s].",
        __PRETTY_FUNCTION__, ret, errno, strerror(errno));
    return -1;
}

int ZEN_Epoll_Reactor::handle_events(ZEN_Time_Value *time_out, size_t *size_io_event) {
    int msec_timeout = -1;
    int event_happen = 0;
    time_t useconds = 0;
    int i = 0;

    if (time_out != 0) {
        useconds = time_out->usec();
        if (useconds != 0 && useconds < 1000) {
            msec_timeout = 1;
        } else {
            msec_timeout = time_out->total_msec();
        }
    }
    event_happen = epoll_wait(epoll_fd_, once_events_ary_, once_max_events_, msec_timeout);
    if (event_happen > 0) {
        *size_io_event = event_happen;
        for (i = 0; i < event_happen; ++i) {
            process_ready_event(&once_events_ary_[i]);
        }
        event_happen = 0;
    }
    return event_happen;
}

void ZEN_Epoll_Reactor::process_ready_event(epoll_event *ep_event) {
    int ret = 0;
    ZEN_Event_Handler *event_hdl = 0;
    bool event_in_happen = false;
    bool event_out_happen = false;
    int hdl_ret = 0;

    if (find_event_handler(ep_event->data.fd, event_hdl) != 0) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[zenlib] process_ready_event find_event_handler fail,maybe one handle is close previous.");
        return;
    }
    if ((ep_event->events & EPOLLIN) != 0) {
        hdl_ret = event_hdl->handle_input();
        event_in_happen = true;
        if (hdl_ret == -1) {
            event_hdl->handle_close();
            event_in_happen = true;
        }
    }
    if ((ep_event->events & EPOLLOUT) != 0) {
        if (!event_in_happen) {
            hdl_ret = event_hdl->handle_output();
            event_out_happen = true;
            if (hdl_ret == -1) {
                event_hdl->handle_close();
            }
        } else {
            if (find_event_handler(ep_event->data.fd, event_hdl) != 0) {
                ZEN_Trace_LogMsg::debug_infoex(
                    "[zenlib] process_ready_event find_event_handler fail,maybe one handle is close previous.");
                return;
            }
            hdl_ret = event_hdl->handle_output();
            event_out_happen = true;
            if (hdl_ret == -1) {
                event_hdl->handle_close();
            }
        }
    }
    if ((ep_event->events & EPOLLERR) != 0) {
        if (!event_in_happen && !event_out_happen) {
            hdl_ret = event_hdl->handle_exception();
            if (hdl_ret == -1) {
                event_hdl->handle_close();
            }
        } else {
            if (find_event_handler(ep_event->data.fd, event_hdl) != 0) {
                ZEN_Trace_LogMsg::debug_infoex(
                    "[zenlib] process_ready_event find_event_handler fail,maybe one handle is close previous.");
                return;
            }
            hdl_ret = event_hdl->handle_exception();
            if (hdl_ret == -1) {
                event_hdl->handle_close();
            }
        }
    }
}
