// Canonical SERVICES_ID / SERVICES_IP_INFO declarations for recon TUs.
//
// The real reconstruction lives in the selected reuse tree
// source/secagent/src/commlib/framework/comm_service_info.h (host builds add
// `-I .../source/secagent`).  This shadow only forwards to that file; it does
// not redeclare anything, so there is exactly one definition in the image.
//
// SERVICES_ID itself is wrapped in TSS_RECON_SERVICES_ID_DECLARED in the
// canonical header so layout/dp_bill_recorder.h (which repeats the same ORIG
// struct for its self-contained billing model) can use the same guard and the
// two headers become co-includable in either order.
#ifndef TSS_RECON_COMM_SERVICE_INFO_SHADOW_H_
#define TSS_RECON_COMM_SERVICE_INFO_SHADOW_H_

#include_next "src/commlib/framework/comm_service_info.h"

#endif  // TSS_RECON_COMM_SERVICE_INFO_SHADOW_H_
