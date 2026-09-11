// ============================================================================
// [SplitC batch10 · routeB_data 档2] CHumanCertifier::humanCertifyTable_
//   ORIG TU = CSHumanCertifyTable.cpp；目标键 _GLOBAL__I__ZN15CHumanCertifier18humanCertifyTable_E
//   ★ 类定义按 DataScan 配方壳内复制（host GlobalData.cpp 的 TU-local 定义）。
//   构造序敏感（档2）：relink 后需 .init_array 顺序核对 + 启动冒烟。
// ============================================================================
#include <map>
#include <string>
#include <vector>

class CHumanCertifier
{
public:
    static void init();
    static std::map<std::string, std::vector<int> > humanCertifyTable_;
};

std::map<std::string, std::vector<int> > CHumanCertifier::humanCertifyTable_;
