// Auto-generated header stub from DWARF info
// Original path: src/commlib/zenlib/zen_event_handle_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_HANDLE_BASE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_HANDLE_BASE_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

// sizeof = 12
struct ZEN_Event_Handler { // line 8
public:
int (***_vptr.ZEN_Event_Handler)(...);
protected:
ZEN_Reactor *zen_reactor_;
int event_mask_;
public:
ZEN_Event_Handler(const ZEN_Event_Handler &arg0);
ZEN_Event_Handler(); // line 8
ZEN_Event_Handler(ZEN_Reactor *arg0); // line 14
virtual ~ZEN_Event_Handler(); // line 21
virtual ZEN_SOCKET get_handle() const; // line 29
virtual int handle_input(); // line 29
virtual int handle_output(); // line 35
virtual int handle_exception(); // line 40
virtual int handle_close(); // line 47
int get_mask(); // line 78
void set_mask(int arg0); // line 84
void enable_mask(int arg0); // line 91
void disable_mask(int arg0); // line 96
virtual void reactor(ZEN_Reactor *reactor); // line 55
virtual ZEN_Reactor * reactor() const; // line 61
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_EVENT_HANDLE_BASE_H_H_
