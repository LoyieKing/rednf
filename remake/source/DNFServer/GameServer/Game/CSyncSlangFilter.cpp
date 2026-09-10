// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSyncSlangFilter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

// [b28d-solo-CSyncSlangFilter] AE：P50 先声明后赋值（ORIG 有独立 iterator::C1Ev，初始化式会被 4.4 折叠）+ P141 无条件 delete 复现 ORIG 空指针守卫；ORIG 0x0862e92a。
#include "CSyncSlangFilter/dtor_SlangNode.inc"
}

#include "CSyncSlangFilter/FindChild.inc"
}

// [b28d-solo-CSyncSlangFilter] AE：键类型改 pair<const unsigned int, SlangNode*>（消 pair<uint,..> 转换构造）+ new 内联进 insert 实参（消具名 node 槽，P226/P278 匿名临时，槽集 it/end/insert/pair/new 与 ORIG 逐槽全等）；ORIG 0x0862ea5a。
#include "CSyncSlangFilter/AddChild.inc"
}

#include "CSyncSlangFilter/IsEndNode.inc"
}

#include "CSyncSlangFilter/SetEndNode.inc"
}

#include "CSyncSlangFilter/IMPL.inc"
}

#include "CSyncSlangFilter/dtor_IMPL.inc"
}

#include "CSyncSlangFilter/CSyncSlangFilter.inc"
}

#include "CSyncSlangFilter/dtor_CSyncSlangFilter.inc"
}

#include "CSyncSlangFilter/GetInstance.inc"
}

#include "CSyncSlangFilter/FreeInstance.inc"
}

#include "CSyncSlangFilter/match.inc"
}

#include "CSyncSlangFilter/MatchName.inc"
}

#include "CSyncSlangFilter/HasSlang.inc"
}

#include "CSyncSlangFilter/HasSlangName.inc"
}

// [b28d-solo-CSyncSlangFilter] AE：sub 去具名块改全表达式实参 + stars/allocator 匿名临时（P221/P226；同时把两析构置于 i+=n 之前，复现 ORIG 块序与槽位）；ORIG 0x0862e46c。
#include "CSyncSlangFilter/Filter.inc"
}

// [b28d-solo-CSyncSlangFilter] N：P75/N-G —— snprintf 首参 buf 装载时刻跨版本差（4.1 在 c_str 后立即 lea→%edx 并跨 varargs 保活，4.4 末位 lea→%eax）；(char*)buf 与逗号形实测同（&buf[0] 历史结论亦同）；ORIG 0x0862e72a。
#include "CSyncSlangFilter/AddSlang.inc"
}

// [b28d-solo-CSyncSlangFilter] N：P75/N-G —— 同 AddSlang（snprintf 首参装载时刻跨版本差）；ORIG 0x0862e276。
#include "CSyncSlangFilter/AddSlangName.inc"
}

#include "CSyncSlangFilter/AddSlangList.inc"
}

#include "CSyncSlangFilter/AddSlangListName.inc"
}
