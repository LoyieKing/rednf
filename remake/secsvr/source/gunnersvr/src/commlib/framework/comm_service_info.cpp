// 还原实现：comm_service_info.cpp（语义对照 gunnersvr oracle 0x0807c170-0x0807c350）
#include "src/commlib/framework/comm_predefine.h"
#include "src/commlib/framework/comm_service_info.h"

// line 10
SERVICES_ID::SERVICES_ID(unsigned short svrtype, unsigned int svrid) {
    services_type_ = svrtype;
    services_id_ = svrid;
}

// line 17
SERVICES_ID::~SERVICES_ID() {
}

// line 23
void SERVICES_ID::set_serviceid(unsigned short svrtype, unsigned int svrid) {
    services_type_ = svrtype;
    services_id_ = svrid;
}

// line 31
bool SERVICES_ID::operator==(const SERVICES_ID &others) const {
    return services_type_ == others.services_type_
           && services_id_ == others.services_id_;
}

// line 42
bool SERVICES_ID::operator!=(const SERVICES_ID &others) const {
    return services_type_ != others.services_type_
           || services_id_ != others.services_id_;
}

// line 53（oracle：u32 加法后比较，注意为 services_type_ + services_id_）
bool SERVICES_ID::operator<(const SERVICES_ID &others) const {
    return (unsigned int)services_type_ + services_id_
           < (unsigned int)others.services_type_ + others.services_id_;
}

// line 68
SERVICES_IP_INFO::SERVICES_IP_INFO()
    : services_info_(0, 0) {
    ip_addr_ = 0;
    port_ = 0;
    idc_no_ = 0;
    server_guid_no_ = 0;
    cfg_info_[0] = 0;
}

// line 77
SERVICES_IP_INFO::~SERVICES_IP_INFO() {
}
