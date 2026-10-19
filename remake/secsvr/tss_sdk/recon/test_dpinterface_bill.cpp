#include <cstdio>
#include <cstring>
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface.h"
#include "src/formmog/tss_sdk/plugins/proto_proc/dpinterface_deps.h"

static int ctor_calls = 0;
static int dtor_calls = 0;
static char seen_msg[32];
static unsigned int seen_type = 0;
static void *seen_user = (void*)1;

namespace tss_dpsdk {
DPBillAdapter::DPBillAdapter(const char *msg, unsigned int type, tss_sdk_impl::UserInfo *user)
    {
    ++ctor_calls; std::strncpy(seen_msg, msg, sizeof(seen_msg)-1); seen_type = type; seen_user = user;
}
DPBillAdapter::~DPBillAdapter() { ++dtor_calls; }
void DPBillAdapter::init(unsigned int) {}
int DPMonitorReporter::init() { return 0; }
}
int DPMsgHandler::init() { return 0; }
int DPHackMgr::init() { return 0; }
namespace ZEN_Trace_LogMsg { void debug_debug(const char *, ...) {} void debug_error(const char *, ...) {} }
DPInterface::DPInterface() {}
int main() {
    DPInterface *p = DPInterface::instance();
    int r = p->write_bill(42, (char*)"hello");
    std::printf("ret=%d ctors=%d dtors=%d msg=%s type=%u user=%p\n", r, ctor_calls, dtor_calls, seen_msg, seen_type, seen_user);
    return (r==0 && ctor_calls==1 && dtor_calls==1 && std::strcmp(seen_msg,"hello")==0 && seen_type==1 && seen_user==0) ? 0 : 1;
}
