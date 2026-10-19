// Reconstructed from libtss_sdk.so.1.0.394: DPInterface::write_bill
// original at 0x000bef40. The `uin` argument is not forwarded; the stack
// DPBillAdapter receives (msg, 1, 0) and is destroyed immediately.
//
// The ORIG CU dpinterface.cpp static-initialization block lives in the
// dpinterface.cpp slice (this TU shares that ORIG CU).
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface_deps.h"

int DPInterface::write_bill(unsigned int uin, char *msg) {
    (void)uin;
    tss_dpsdk::DPBillAdapter bill(msg, 1, 0);
    return 0;
}
