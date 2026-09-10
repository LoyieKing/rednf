// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ServiceError/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// nsl::ServiceError.cpp
#include <stdio.h>
#include <map>
#include <string>

#include "ServiceError.h"
#include "TraceLog.h"

#define REGISTER_SERVICE_ERROR(enum_type, err) \
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", err, #err); \
    nsl::___errstr_map___.insert(std::make_pair<enum_type, char(&)[2048]>(err, nsl::___errstr_tmp___))

namespace nsl {

std::map<int, std::string> ___errstr_map___;
char ___errstr_tmp___[2048];

#include "ServiceError/InitServiceErrorStr.inc"
}

const char* GetErrorStr(int err)
{
    std::map<int, std::string>::iterator it = ___errstr_map___.find(err);
    if (it == ___errstr_map___.end())
    {
        G_TraceLog()->sysLog(7, "GetErrorStr() failed. error_no : %d\n", err);
        return ___errstr_map___.find(ERROR_NOT_USE)->second.c_str();
    }
    return it->second.c_str();
}

} // namespace nsl

using namespace nsl;

#include "ServiceError/SetAuctionServiceErrorStr.inc"
}
