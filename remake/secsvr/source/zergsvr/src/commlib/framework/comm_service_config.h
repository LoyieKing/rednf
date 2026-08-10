// 该头文件在 zergsvr 原始编译中只提供 ServicesConfig / SetOfServicesIPInfo /
// HashofSvrIPInfo / EqualSvrIPInfo；还原后这些类型位于 comm_service_info.h
// （DWARF decl_file 指向 comm_service_info.h），这里仅作兼容包含。
// Original path: src/commlib/framework/comm_service_config.h
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_CONFIG_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_CONFIG_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SERVICE_CONFIG_H_H_
