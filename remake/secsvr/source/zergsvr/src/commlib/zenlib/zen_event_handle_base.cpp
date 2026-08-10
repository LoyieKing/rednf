// Restored from zergsvr binary (DWARF CU zen_event_handle_base.cpp).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_event_handle_base.cpp

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_event_handle_base.h"
#include "src/commlib/zenlib/zen_event_reactor_base.h"
#include <iostream>

ZEN_Event_Handler::ZEN_Event_Handler() : zen_reactor_(0), event_mask_(0) {
}

ZEN_Event_Handler::ZEN_Event_Handler(ZEN_Reactor *reactor) : zen_reactor_(reactor), event_mask_(0) {
}

ZEN_Event_Handler::~ZEN_Event_Handler() {
}

int ZEN_Event_Handler::handle_input() {
    return 0;
}

int ZEN_Event_Handler::handle_output() {
    return 0;
}

int ZEN_Event_Handler::handle_exception() {
    return 0;
}

int ZEN_Event_Handler::handle_close() {
    this->reactor()->remove_handler(this, false);
    return 0;
}

void ZEN_Event_Handler::reactor(ZEN_Reactor *reactor) {
    zen_reactor_ = reactor;
}

ZEN_Reactor *ZEN_Event_Handler::reactor() const {
    return zen_reactor_;
}
