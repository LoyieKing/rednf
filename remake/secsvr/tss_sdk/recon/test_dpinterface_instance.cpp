// Link/guard check for the reconstructed DPInterface::instance().
// The real handlers are not needed to verify the local static singleton.
#include <cstdio>
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"

DPInterface::DPInterface() {}

int main() {
    DPInterface *a = DPInterface::instance();
    DPInterface *b = DPInterface::instance();
    std::printf("a=%p b=%p same=%s\n", (void*)a, (void*)b, a == b ? "yes" : "no");
    return a == b ? 0 : 1;
}
