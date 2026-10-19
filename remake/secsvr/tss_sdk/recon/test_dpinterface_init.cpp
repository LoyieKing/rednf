#include <cstdio>
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface_deps.h"

static int calls = 0;
static unsigned int bill_uin = 0;
int DPMsgHandler::init() { ++calls; return 0; }
int DPHackMgr::init() { ++calls; return 0; }
void tss_dpsdk::DPBillAdapter::init(unsigned int uin) { bill_uin = uin; }
int tss_dpsdk::DPMonitorReporter::init() { ++calls; return 0; }
namespace ZEN_Trace_LogMsg {
void debug_debug(const char *, ...) {}
void debug_error(const char *, ...) {}
}

DPInterface::DPInterface() { msg_handler_ = new DPMsgHandler; hack_manager_ = new DPHackMgr; }

int main() {
    DPInterface *p = DPInterface::instance();
    int a = p->init("7");
    int b = p->init("9");
    std::printf("first=%d second=%d calls=%d uin=%u\n", a, b, calls, bill_uin);
    return (a == 0 && b == -1 && calls == 3 && bill_uin == 7) ? 0 : 1;
}
