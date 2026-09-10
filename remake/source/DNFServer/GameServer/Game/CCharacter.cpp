// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCharacter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CCharacter（依据 docs/class_func_reports/CCharacter/ 报告）。
// ctor ORIG _ZN10CCharacterC1Ev T 0x0834828c / dtor D1 T 0x0834842a。
//
// batch-15 近战池（ledger class=A 共 2 行）亲验归因：
//   1) _ZN9__gnu_cxx13new_allocatorISt4pairIiiEE9constructIIS2_EEEvPS2_DpOT_
//      (ORIG 0x080dda40 22 / 我 38)——libstdc++ 头生成体，差 16 = 4.4 placement
//      new 的 EH cleanup（_ZdlPvS_+_Unwind_Resume 10 条，N 类『new-expression
//      landing-pad 抑制』）+ N83 外联 op new + 寄存器物化；源码侧无合法解，豁免。
//   2) set_data：见其函数头 §6.2 标注；逐块亲验——调用多重集全等、目标常量写址
//      多重集仅 ORIG 以 +0x7d5 为基 + (i-1) 缩放 vs 我方 +0x7d4 基 + i（同一地址
//      集，常量基表达差 1）与 1 处 +0x10 标量写形态差 ⇒ 语义完成维持。
// ============================================================================
#include "CCharacter.h"

// ORIG STSecondGrowTypeInfo::STSecondGrowTypeInfo W 0x8373934：
// 2 个 _Additioal_info 构造 + 2 个 vector 默认构造 + clear()。
#include "CCharacter/STSecondGrowTypeInfo.inc"
}

// ORIG ~STSecondGrowTypeInfo W 0x8376324：逆序析构 +0xa4/+0xb0 两个 vector。
#include "CCharacter/dtor_STSecondGrowTypeInfo.inc"
}

// ORIG CCharacter::CCharacter @ 0x834828c：
//   _Additioal_info[7]（this+0 起，步长 0x52）
//   vector[7]（+0x240，步长 0xc）
//   STSecondGrowTypeInfo[7]（+0x294，步长 0xc0）→ clear()。
#include "CCharacter/CCharacter.inc"
}

// ORIG ~CCharacter @ 0x834842a：destroy → 逆序析构 STSecondGrowTypeInfo[7]、
// vector[7]（成员析构由编译器按声明逆序生成，语义一致）。
#include "CCharacter/dtor_CCharacter.inc"
}

// ORIG get_give_skill_firstGrowtype @ 0x83485d8。
// ORIG 地址 = this + 0x240 + 12*(grow+1)（set_data 填满 [0..6]，本查询跳过 0 号基础槽）。
// [2026-09-10 D06] bool（同 get_give_skill 判据；尾 `mov $0x1,%eax` 与 bool 兼容）。
#include "CCharacter/get_give_skill_firstGrowtype.inc"
}

// ORIG get_give_skill_secondGrowtype @ 0x8348694。
// 注意：secondGrow 越界（<1 或 >2）时返回 1 且不拷贝（ORIG uVar2=1 分支）。
// ORIG 地址 = this + 0x330 + 0xc0*(grow+1) + 12*(secondGrow-1) + 8
//          = &m_secondGrowTypes[grow+1].m_skills[secondGrow-1]（同样跳过 0 号基础槽）。
// [2026-09-10 D06] bool（同族）。
#include "CCharacter/get_give_skill_secondGrowtype.inc"
}

// ORIG get_growtype_stat @ 0x834883e：按 (firstGrow, secondGrow) 返回成长属性块
// 指针；越界（firstGrow>5 或 secondGrow>2）返回 NULL。secondGrow==0 时取
// firstGrow*0x52+0x52（= m_baseInfo[firstGrow+1]，块宽 sizeof(_Additioal_info)=0x52），
// 否则取 (secondGrow-1)*0x52+(firstGrow+1)*0xc0+0x294
// （= &m_secondGrowTypes[firstGrow+1].m_addInfo[secondGrow-1]，
//   m_secondGrowTypes @+0x294、步长 sizeof(STSecondGrowTypeInfo)=0xc0、
//   m_addInfo 元素步长 0x52；布局断言见 CCharacter.h）。
#include "CCharacter/get_growtype_stat.inc"
}

// ORIG get_give_skill @ 0x8348798。
#include "CCharacter/get_give_skill.inc"
}

// ============================================================================
// 以下为 CharacterAIA2 批次追加（报告 docs/class_func_reports/CCharacter/
// {CCharacter.md,~CCharacter.md} 完整转写；destroy/set_data/clear 报告缺失
// → objdump 转写，ORIG nm：destroy T 0x083485c4 / clear T 0x0834852e /
//   set_data T 0x08348894）。
// ============================================================================
#include "STCharacterScript.h"

// ORIG STSecondGrowTypeInfo::clear W 0x08373a1e（objdump 转写）：
// i=0..1 { m_addInfo[i].clear(); m_skills[i].clear(); } 后 memset(&m_fieldbc,0,2)
// —— 仅清 m_fieldbc 低 2 字节，高 2 字节保留（ORIG 固有）。
#include "CCharacter/clear.inc"
}

// ORIG CCharacter::destroy @ 0x083485c4：sub $0x18 帧 + call clear()。
#include "CCharacter/destroy.inc"
}

#include "CCharacter/clear_1.inc"
}

// ORIG CCharacter::set_data @ 0x08348894（objdump 全量转写，2901B / 932 条）。
// §6.2 超大函数（932 insns）验收：调用点差集 ZERO；gap 43→16（1.7%），残余为
// 4.1 常量拆分二段 `add $0x50/add $0x2` 寻址 vs 4.4 `lea disp(,%e)` 折叠
// （ORIG-only add 58 / OURS-only lea 46 净额，N82/P183 同族机器码折叠差）
// ⇒ 判语义完成、byte 层 N 封顶。
// ORIG 结构为 if(i==0)/else **双展开体**，两支各写一整套 24 条赋值，不共享缓存：
//   读侧两支统一为 script->m_status[i]（ORIG 每次读都重算 script+i*0x57+0x24；
//   i=0 落 m_initialStatus，i=1..6 落 m_growStatus[i-1]，布局见 STCharacterScript.h）；
//   写侧 i==0 支为常量偏移 this+0x00 一整套（m_baseInfo[0] / m_growTypeFlag[0]），
//   else 支为 m_baseInfo[i] / m_growTypeFlag[i]（ORIG 写侧算式为
//   (i-1)*0x52+0x52 与 (i-1)+0x7d5，地址等价；本工程 g++4.4.7 会把
//   (i-1)*0x52+0x52 常量折叠成 i*0x52，两种写法生成码逐字节相同，
//   故取无 UB 的下标形式）。
//   两支之后共用 m_giveSkillList[i] = script->m_skillList[i]（vector::operator=）。
// 第二成长域（a=0..6 × b=0..1）同样逐字段重算地址、不缓存引用：
//   dst = m_secondGrowTypes[a].m_addInfo[b]（this + 0x294 + a*0xc0 + b*0x52），
//   src = script->m_secondGrow[a].m_statusA[b]（script + 0x3d0 + a*0x100 + b*0x57），
//   resist 两段循环按具名字段寻址即与 ORIG 逐字节一致（+0x2a4 / +0x2ac），
//   尾字节写 ((char*)&m_secondGrowTypes[a].m_fieldbc)[b]（ORIG +0x350+a*0xc0+b），
//   末尾 m_secondGrowTypes[a].m_skills[b] = script->m_secondGrow[a].m_skills[b]。
// [b28d-7 N82] ORIG 0x08348894：帧 $0x5c + `imul $0x57` 后 add 两步寻址（-0x3c 先载），4.4 帧 $0x4c + lea 折叠；P167 CLEAR ⇒ 封顶。
#include "CCharacter/set_data.inc"
}
