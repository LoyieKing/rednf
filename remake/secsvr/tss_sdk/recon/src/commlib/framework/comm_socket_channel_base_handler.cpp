// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_base_handler.cpp
// Address range 0x000e6e60-0x000e7120.
#include "src/commlib/framework/comm_socket_channel_base_handler.h"

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

SocketChannelInitData::SocketChannelInitData() {
}

SocketChannelBaseHandler::SocketChannelBaseHandler() {
}

SocketChannelBaseHandler::~SocketChannelBaseHandler() {
}

void SocketChannelBaseHandler::set_sndrcv_bus(SocketChannelBus *bus) {
    sndrcv_bus_ = bus;
}
