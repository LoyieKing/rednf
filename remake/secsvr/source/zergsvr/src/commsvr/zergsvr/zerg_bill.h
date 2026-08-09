// Auto-generated header stub from DWARF info
// Original path: src/commsvr/zergsvr/zerg_bill.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BILL_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BILL_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"

// sizeof = 1
struct Zerg_Bill { // line 13
public:
static Zerg_Bill *_instance;
static Zerg_Bill * instance(); // line 16
static void clean_instance(); // line 17
Zerg_Bill(); // line 28
~Zerg_Bill(); // line 29
void write_get_svrinfo_bill(const char *svrinfo, SERVICES_ID req_service, unsigned int idc_no, unsigned int server_guid_no); // line 34
};

#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_BILL_H_H_
