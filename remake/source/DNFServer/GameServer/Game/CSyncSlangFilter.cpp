// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSyncSlangFilter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CSyncSlangFilter.h"

#include <new>
#include <string.h>
#include <ext/hash_map>

// [T2b-GI · P338/P327] 补 <iostream> 引入 file-scope 动态初始化 __ioinit，恢复本 TU
// `_GLOBAL__I_` 发射；键名目标 `_ZN16CSyncSlangFilter8instanceE`（ORIG 0x862e8df）。
// 零语义改动。
#include <iostream>

#include "DBMgr.h"
#include "GameTypes.h"
#include "GlobalData.h"
#include "MySQL.h"

namespace OS_API
{
#include "CSyncSlangFilter/snprintf.inc"
}

class SlangNode
{
public:
    SlangNode();
    ~SlangNode();
    SlangNode* AddChild(unsigned int ch);
    SlangNode* FindChild(unsigned int ch) const;
    bool IsEndNode() const;
    void SetEndNode(bool v);

private:
    __gnu_cxx::hash_map<unsigned int, SlangNode*> children_;
    bool end_;
};

// [T2b-GI · P327] 定义前移：ORIG 该 TU 首个强外部实体 = 类静态成员 instance
// （GI 键名 `_GLOBAL__I__ZN16CSyncSlangFilter8instanceE`，ORIG 0x862e8df）。
// 零初始化对象（非动态初始化）⇒ 移动不改变构造序。
CSyncSlangFilter* CSyncSlangFilter::instance = 0;

#include "CSyncSlangFilter/SlangNode.inc"


#include "CSyncSlangFilter/dtor_SlangNode.inc"


#include "CSyncSlangFilter/FindChild.inc"


#include "CSyncSlangFilter/AddChild.inc"


#include "CSyncSlangFilter/IsEndNode.inc"


#include "CSyncSlangFilter/SetEndNode.inc"


#include "CSyncSlangFilter/IMPL.inc"


#include "CSyncSlangFilter/dtor_IMPL.inc"


#include "CSyncSlangFilter/CSyncSlangFilter.inc"


#include "CSyncSlangFilter/dtor_CSyncSlangFilter.inc"


#include "CSyncSlangFilter/GetInstance.inc"


#include "CSyncSlangFilter/FreeInstance.inc"


#include "CSyncSlangFilter/match.inc"


#include "CSyncSlangFilter/MatchName.inc"


#include "CSyncSlangFilter/HasSlang.inc"


#include "CSyncSlangFilter/HasSlangName.inc"


#include "CSyncSlangFilter/Filter.inc"


#include "CSyncSlangFilter/AddSlang.inc"


#include "CSyncSlangFilter/AddSlangName.inc"


#include "CSyncSlangFilter/AddSlangList.inc"


#include "CSyncSlangFilter/AddSlangListName.inc"

