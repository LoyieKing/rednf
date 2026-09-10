// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RDARScriptStringManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r RDARScriptStringManager（G2 字符串表批次B）
// ----------------------------------------------------------------------------
// 全局 g_scriptStringManager_ 由 CTradeSpace.cpp 引用（findString(key) 邮件文案）。
// ORIG（docs/class_func_reports/RDARScriptStringManager/）：
//   C1    _ZN23RDARScriptStringManagerC1Ev                                 0x8aa4e98
//   D1    _ZN23RDARScriptStringManagerD1Ev                                 0x8aa4f04
//   release _ZN23RDARScriptStringManager7releaseEv                         0x8aa5014
//   loadStringFile _ZN23RDARScriptStringManager14loadStringFileEPSiPKciPSt6vectorI8ErrorLogSaIS4_EE  0x8aa529e
//   等（见各函数注释）。
// 类型布局（ORIG +0x00 STScriptFileList / +0x18 vector<StringDictionary*> /
// +0x24 wstring tag_）由 RDARScriptStringManager.h 成员近似，ABI 一致。
// StringDictionary 类型与实现见 StringDictionary.h / StringDictionary.cpp
// （批次A，本 TU 仅依赖其公开接口，不修改其实现）。
// 依赖 toTString(const char*) / CodePage::script2Database / LogManager，均已存在。
// toUnicode/toTString(wchar) 属 G4 域未落地，此处以文件局部 static 等价实现，
// 不导出全局符号、不与 G 域 worker 冲突。
// ============================================================================

#include "RDARScriptStringManager.h"

#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cwchar>

#include "DNFFunctionLib.h"   // CodePage::script2Database
#include "LogManager.h"       // LogManager::logFormat
#include "SyncScriptDeps.h"   // toTString(const char*) / toTString(const wchar_t*)
#include "StringTypes.h"      // toUnicode（G4 域；返回 TLS 环形缓冲槽指针）

// ----------------------------------------------------------------------------
// ORIG 静态 _ZL15parseStringLinePcPS_S0_ 0x8aa6551（本 TU 文件局部）。
// 语义（objdump 0x8aa6551）：
//   1) 跳过行首空白（0x0d/0x0a/0x20/0x09）；跳空过程中遇 CR/LF → 返回 0（空行）。
//   2) 若行首为 NUL 或 "//"（注释）→ 返回 0。
//   3) 扫描查找分隔符 '>'（0x3e）：找到则置 NUL、后移、记录 value 起始；未找到 → 返回 -1。
//   4) keyLen==1（无有效 key）→ 返回 -1；keyLen-1 > 0x28（41）→ 返回 -2。
//   5) 解析 value：遇 CR/LF → 置 NUL 结束；遇 "\\n" → 替换为真实 '\n' 并前移；
//   6) *out_key = keyStart; *out_value = valueStart; 返回 0。
// 返回：0=成功（或空/注释行，此时出参不变）、-1=无分隔符或 key 过短、-2=key 过长。
// ----------------------------------------------------------------------------
static int parseStringLine(char* line, char** outKey, char** outValue)
{
    char* p = line;

    // 1) 跳过行首空白；遇换行 → 空行
    // ORIG 0x8aa6551：循环条件在底部（`jmp cond` 入口 + `test;setne;jne`）
    // ⇒ 源码为 `while (*p)`（非 for(;;) + 显式 NUL break）。
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
            return 0;
        if (*p == ' ' || *p == '\t')
        {
            ++p;
            continue;
        }
        break;
    }

    // 2) 空行或注释
    if (*p == '\0')
        return 0;
    if (*p == '/' && *(p + 1) == '/')
        return 0;

    // 3) 找 '>' 分隔符
    char* keyStart = p;
    char* valueStart = 0;
    while (*p != '\0')
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            valueStart = p;
            break;
        }
        ++p;
    }

    // 4) 无分隔符或 key 无效
    if (valueStart == 0)
        return -1;
    if ((valueStart - keyStart) == 1)
        return -1;
    if ((valueStart - keyStart - 1) > 0x28)
        return -2;

    // 5) 解析 value：换行结束 / "\\n" 转义
    p = valueStart;
    while (*p != '\0')
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        if (*p == '\\' && *(p + 1) == 'n')
        {
            *p = '\n';
            // 整体前移，移除 'n'
            char* tail = p + 1;
            char* dst = p;
            ++tail;
            while (*tail != '\0')
                *dst++ = *tail++;
            *dst = '\0';
        }
        ++p;
    }

    // 6) 出参
    *outKey = keyStart;
    *outValue = valueStart;
    return 0;
}

// ORIG 字符串表函数用 ss_sprintf<256>（安全限长）而非裸 sprintf：
// 模板定义与 OnlyOrigBatch12.cpp 一致（extern ss_vsprintf，直接传变参起始地址）。
extern int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList);
template <size_t N>
#include "RDARScriptStringManager/ss_sprintf.inc"
}

#include "RDARScriptStringManager/RDARScriptStringManager.inc"
}

// b28d-21: IDENTICAL（91=91）— 迭代器直构（P50 反例复活）+ end 循环外缓存（P13）
// + 后置 `it++`（P28 增量侧 ppEi 判据）+ `delete *it` 无具名 d/无显式判空。
// ORIG D1 0x8aa4f04。
#include "RDARScriptStringManager/dtor_RDARScriptStringManager.inc"
}

#include "RDARScriptStringManager/findString.inc"
}

#include "RDARScriptStringManager/findString_1.inc"
}

// b28d-21: 已修 P157/P183（operator[] 引用局部 `StringDictionary* const& d` +
// 去 getCount 具名局部，56→24）；余 ORIG setb/sete/setg 物化 vs 4.4
// jae/mov$1 直跳（P197④ setcc-fold）+ 槽号（N82）。ORIG 0x8aa5cba。
#include "RDARScriptStringManager/getKeyIndex.inc"
}

// b28d-solo: ORIG 0x8aa59f0 尾部 `mov $0x1,%eax` + 两处 `mov $0x0,%eax`
// ⇒ 真实返回 bool（P159）；三个守卫全部正极性包裹（无 continue），
// `wcsstr(toUnicode(sv), name)` 实参内联（ORIG 无 std::wstring wv 临时）。
#include "RDARScriptStringManager/getTagByName.inc"
}

// b28d-solo-NEAR 封顶（P75）：169=169 助记符序列全同，唯一残差 = 末尾
// ss_sprintf 第 3 变参的装载时刻——ORIG `mov %eax,0xc(%esp); mov 0x10(%ebp),%eax;
// mov %eax,0x8(%esp)`（先存 callee 返回值再载 a），4.4.6 恒
// `mov 0x10(%ebp),%edx; mov %eax,0xc(%esp); mov %edx,0x8(%esp)`。
// 本波 4.4.6 路由下复测 14 形探针（/tmp/p75.cpp+m9..m14）全部同形：
// reinterpret_cast / 双层 static_cast / C cast / `(unsigned)a` / `(long)a` /
// `+0` / `(0,expr)` / 逗号 / `&p[0]` / `&0xffffffff` / `^0` / `*1` / `/1` / `-0`
// —— 4.4 对「末实参为 call + 倒数第二为形参」恒该序，源形不可翻。
// ORIG 0x8aa5e40。
#include "RDARScriptStringManager/getTagByName_1.inc"
}

#include "RDARScriptStringManager/setTableValue.inc"
}

// b28d-solo-N 封顶：槽位集与 ORIG 全对齐（cpBuf -0x1010 / 三份 ErrorLog
// e -0x50/-0x60/-0x70 / auto_ptr -0x38 / isJpn -0x2e / isKor -0x2d / lineNo -0x2c）；
// 残差三类均为已登记 N 族：① `memset(cpBuf,0,0xfa0)` 4.1 内联 rep stos vs 4.4
// 恒 call（N78）；② `isJpn == false` 取反 4.4 恒 `movzbl;xor $1;test;je` vs
// ORIG `cmpb $0,mem;je`（N122）；③ r 槽 -0x28 vs ORIG -0x24 + 内存直比 vs
// 寄存器比较（N82/P286）。本波修正的真语义 bug：第三处 ErrorLog 原写
// `e.m_message += getKey(i)`，ORIG 0x5739 为 `add $0x8,%eax` + `operator=`
// ⇒ 应为 `e.m_detail = getKey(i)`；另补两处 `e.m_flag = lineNo`
// （ORIG 0x5415/0x5489 `mov lineNo,-0x44/-0x54`）。
// ORIG 0x8aa529e。
#include "RDARScriptStringManager/loadStringFile.inc"
}

// b28d-solo: ORIG 0x8aa50bc 尾部 `mov $0x1,%eax` / `mov $0x0,%eax` 两条返回
// 路径 ⇒ 真实返回 bool（P159；返回类型不入 mangled，全仓 0 调用点）。
#include "RDARScriptStringManager/writeStringFile.inc"
}

#include "RDARScriptStringManager/getTagSizeInFile.inc"
}

#include "RDARScriptStringManager/findChangedString.inc"
}

#include "RDARScriptStringManager/getStringFileList.inc"
}

// b28d-solo-N 封顶（EH 尾段）：前 185 条逐条全等（含两处死拷贝 std::string(s)、
// 局部 OPEN/CLOSE、find/rfind 截断 short、三处 substr 的重复 strlen）；残差全在
// 函数尾部 EH cleanup 决策链——ORIG 每析构点前发
// `test %esi,%esi; jne L; mov $0,%esi; jmp; L: mov $1,%esi`（4.1 landing-pad 决策），
// 4.4 直接顺序析构，属 N91/N104 族（跨版本 EH 结构，无源形解）。返回类型已按
// P159 由 void 改 bool（ORIG 尾 `mov %ebx,%eax`，ebx=1/0）。ORIG 0x8aa60b0。
#include "RDARScriptStringManager/changeSpacilTagStr.inc"
}

// b28d-21: IDENTICAL（50=50）— 同 D1：迭代器直构 + end 缓存 + `it++` + `delete *it`。
// ORIG 0x8aa5014。
#include "RDARScriptStringManager/release.inc"
}

RDARScriptStringManager g_scriptStringManager_;