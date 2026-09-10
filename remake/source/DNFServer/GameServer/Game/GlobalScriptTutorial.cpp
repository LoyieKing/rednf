// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GlobalScriptTutorial/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GlobalScriptTutorial.h"
#include "DNFLexWrapper.h"

#include <string>
#include <vector>

#include "GlobalScriptTutorial/getCharacterJob.inc"

#include "GlobalScriptTutorial/GlobalScriptTutorial.inc"
}

#include "GlobalScriptTutorial/dtor_GlobalScriptTutorial.inc"
}

// b28d-20: ORIG 0x08ab8e4a。修法：外层/内层扫描循环改 while(true)+`if(!ScanType/ScanStr)break;`
// （P133/P225 负极性短路：ORIG 每条件 xor$1/test + mov$1/$0 汇聚岛 + ebx 循环标志，4.4 对
// `while(ScanType(token,true))` 折成正极性直跳）；token/s 移入循环体（ORIG 每轮构造/析构）；
// 尾部迭代器遍历改索引 size()/operator[] 形（P175）。结果 IDENTICAL_AE（132=133，尾 nop 剥离）。
#include "GlobalScriptTutorial/loadScript.inc"
}

// b28d-20: ORIG 0x08ab8ffe。修法：token 移入循环体 + while(true)/`if(!ScanType)break;`
// （P0 无槽往返=表达式右值；ORIG 每轮构造/析构 token 并复用 ebx 作循环/返回汇聚标志）。
// 结果 IDENTICAL_AE（155=156，尾 nop 剥离）。
#include "GlobalScriptTutorial/loadScript_1.inc"
}

// b28d-20: ORIG 0x08ab9226。修法：while(true)+`if(!ScanInt||!ScanInt||!ScanStr)break;`（P133）；
// 槽序 key,face,text —— 4.4 后声明占深槽，ORIG text@-0x14/key@-0xc/face@-0x10（P28 4.4 反向）。
// 结果 IDENTICAL_AE（76=77，尾 nop 剥离）。
#include "GlobalScriptTutorial/LoadDialogTag.inc"
}

// b28d-20: ORIG 0x08ab930a。修法：while(true)+`if(!ScanInt||!ScanInt)break;`（P133/P225，
// ORIG xor$1/test/jne + mov$1/$0 汇聚岛）。结果 IDENTICAL_AE（40=40）。
#include "GlobalScriptTutorial/LoadAvatarTag.inc"
}

// b28d-20: ORIG 0x08ab937a。修法：①while(true)+`if(!ScanInt×3)break;`（P133）；
// ②实参顺序修正为 addEquipmentIndex(type,index,slot) 且声明序 slot,type,index ——
// ORIG 压栈 arg1=第二扫描值/arg2=第三/arg3=第一（addEquipmentIndex 报告：p1=key,p2=value,
// p3=map 索引且 cmpl $3,p3 越界保护），原实现把第一扫描值当 key 属真语义错位。
// 结果 IDENTICAL_AE（51=51）。
#include "GlobalScriptTutorial/LoadEquipmentTag.inc"
}

#include "GlobalScriptTutorial/LoadDungeonIndex.inc"
}

// b28d-20: ORIG 0x08ab9408。修法：while(true)+`if(!ScanInt×5)break;`（P133/P225）。
// 结果 IDENTICAL_AE（71=72，尾 nop 剥离）。
#include "GlobalScriptTutorial/LoadSkillTag.inc"
}

// b28d-20: ORIG 0x08ab94d4。修法：while(true)+`if(!ScanInt×3)break;`（P133/P225）。
// 结果 IDENTICAL_AE（51=51）。
#include "GlobalScriptTutorial/LoadPassiveSkilltag.inc"
}

// b28d-20: ORIG 0x08ab95ac。修法：while(true)+`if(!ScanInt×3||!ScanStr)break;`（P133）；
// 槽序 key,field8,fieldc,path（4.4 后声明占深槽，ORIG path@-0x18/key@-0xc）。
// 结果 IDENTICAL_AE（84=84）。
#include "GlobalScriptTutorial/LoadTutorialNotice.inc"
}

// b28d-20: ORIG 0x08ab96a8。修法：while(true)+`if(!ScanInt||!ScanInt)break;`（P133/P225）。
// 结果 IDENTICAL_AE（40=40）。
#include "GlobalScriptTutorial/LoadItem.inc"
}

#include "GlobalScriptTutorial/clearScript.inc"
}


#include "GlobalScriptTutorial/IsGlobalTutorialDungeon.inc"
}
