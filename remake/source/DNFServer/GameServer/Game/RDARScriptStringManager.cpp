// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RDARScriptStringManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// [M2S5StringRaw 2026-09-11] 三处「我方多」冗余修正（探针 /tmp/probe/Game/RDARScriptStringManager.cpp
// 4.4.6 路由复刻，新形逐条对齐 ORIG 仅余 1 条对齐 nop；旧形 -12 条）：
//   ① ORIG 0x8aa65af `je` 与 0x8aa65c6 `jne` 同指 return-0 岛 0x8aa65c8
//      ⇒ 空行/注释两条件合并为一个短路或；
//   ② ORIG 0x8aa6614 `je` 与 0x8aa6625 `jne` 同指 -1 岛 0x8aa6627
//      ⇒ valueStart==0 与 keyLen==1 合并（旧形发两份 `mov $-1; jmp`）；
//   ③ ORIG tail 左移为单指针 q（-0x4 槽，`*q = q[1]` 含 NUL 一并搬迁）
//      ⇒ 无 dst 局部、无显式补 NUL；且 ORIG 无 `p = valueStart` 重载
//      （0x8aa6643 jle 直跳循环条件 0x8aa66b8，p 槽沿用 break 时值）。
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

    // 2) 空行或注释（两条件共享 ORIG return-0 岛 0x8aa65c8，合并短路或）
    if (*p == '\0' || (p[0] == '/' && p[1] == '/'))
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

    // 4) 无分隔符或 key 无效（共享 ORIG -1 岛 0x8aa6627）
    if (valueStart == 0 || (valueStart - keyStart) == 1)
        return -1;
    if ((valueStart - keyStart - 1) > 0x28)
        return -2;

    // 5) 解析 value：换行结束 / "\\n" 转义。
    // 不写 `p = valueStart`：key 扫描 break 时 p 恰为 valueStart（ORIG
    // 0x8aa6643 `jle 0x8aa66b8` 直跳循环条件，p 槽未重载）。
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
            // 单指针就地前移（ORIG 0x8aa6688..0x8aa66b2：q(-0x4)=p+1，
            // `*q = q[1]` 循环至 *q==0 ⇒ NUL 一并搬迁结束，无显式补 NUL）。
            for (char* q = p + 1; *q != '\0'; ++q)
                *q = *(q + 1);
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
{
    // ORIG：va_list ap 局部（槽往返）+ 返回值具名局部（P0）
    va_list ap;
    va_start(ap, format);
    int ret = ss_vsprintf(buffer, N, format, reinterpret_cast<char*>(ap));
    va_end(ap);
    return ret;
}

#include "RDARScriptStringManager/RDARScriptStringManager.inc"
{
    // ORIG C1 0x8aa4e98：STScriptFileList(+0) / vector<StringDictionary*> (+0x18)
    // / wstring (+0x24) 依次默认构造。头成员 fileList_/dictionaries_/tag_ 由
    // 成员初始化列表默认构造（map/vector/wstring 空构造），体为空即达 ORIG 语义。
}

// b28d-21: IDENTICAL（91=91）— 迭代器直构（P50 反例复活）+ end 循环外缓存（P13）
// + 后置 `it++`（P28 增量侧 ppEi 判据）+ `delete *it` 无具名 d/无显式判空。
// ORIG D1 0x8aa4f04。
#include "RDARScriptStringManager/dtor_RDARScriptStringManager.inc"
{
    // ORIG D1 0x8aa4f04：begin/end 迭代器循环逐元素 delete（与 release 同形），
    // 随后成员析构。end 循环外求值一次（P13），迭代器先声明后赋值（P50）。
    std::vector<StringDictionary*>::iterator it = dictionaries_.begin();
    std::vector<StringDictionary*>::iterator e = dictionaries_.end();
    for (; it != e; it++)   // ORIG 增量段 ppEi（后置，P28 推论）
    {
        delete *it;         // ORIG 无具名 d/无显式判空（delete 自带守卫）
    }
}

#include "RDARScriptStringManager/findString.inc"
                                                bool* flag) const
{
    // ORIG 0x8aa57fe
    // b28d-21: DIFF/N 封顶 — ORIG 0x8aa57fe 中一处实参为 `movl $<A>,(%esp)` 直压
    // 立即数（rodata/全局），重建为 `mov this; add $0x18` 取成员地址（对象/调用
    // 形态差）+ best 指针槽位差 4（N82）。语义/调用序全等。
    if (key == 0)
    {
        return "RDARScriptStringManager::ERROR::input NULL string";
    }

    static char errStr[256];
    if (table >= static_cast<int>(dictionaries_.size()))
    {
        return "RDARScriptStringManager::ERROR::requested fileIndex isn't in string table";
    }

    // ORIG 0x8aa583d：第二处范围检查取**全局对象** g_scriptStringManager_ 的
    // vector（`movl $&g_scriptStringManager_+0x18,(%esp)`），非 this 成员。
    if (table < 0 || table >= static_cast<int>(g_scriptStringManager_.dictionaries_.size()))
    {
        LogManager::logFormat(
            0, "../RDARScriptStringManager.cpp", __FUNCTION__, 0x11c,
            "\xbf\xc0\xb7\xf9 : \xbd\xba\xc6\xae\xb8\xb5 \xc5\xd7\xc0\xcc\xba\xed %d \xb4\xc2 \xb9\xfc\xc0\xa7\xb8\xa6 \xb9\xfe\xbe\xee\xb3\xb3\xb4\xcf\xb4\xd9. "
            "(index out of range reading string table %d)",
            table);
        ss_sprintf(errStr, "String table index out of range %d", table);
        return errStr;
    }

    const char* result = dictionaries_[table]->get(toTString(key));
    if (flag != 0)
        *flag = (result != 0);
    if (result != 0)
        return result;

    // ORIG 0x8aa5918：GetFileName 结果存 callee-saved %ebx 跨 toTString 调用
    // （无槽往返）⇒ 实参内联（P0/P23）。
    ss_sprintf(errStr, "%s == NULL, %s : ",
               toTString(key), fileList_.GetFileName(table));
    return errStr;
}

#include "RDARScriptStringManager/findString_1.inc"
                                                bool* flag) const
{
    // ORIG 0x8aa594c
    // b28d-21: DIFF/N 封顶 — ORIG 0x8aa594c 共 49 条 vs 重建 94 条：ORIG 走
    // 窄串直接路径，重建多出临时 std::string 构造/析构与转换（N86/N89 模板族）。
    // 语义等价（同 key 查表），byte 层 N。
    if (key == 0)
    {
        return "RDARScriptStringManager::ERROR::input NULL string";
    }

    static char errStr[256];
    // ORIG 0x8aa5969：toTString(const wchar_t*) 返回 TLS 缓冲 const char*，
    // 直接作 get 实参（无 std::string 临时；N86/N89 源侧根因）。ORIG 无表范围检查。
    const char* result = dictionaries_[table]->get(toTString(key));
    if (flag != 0)
        *flag = (result != 0);
    if (result != 0)
        return result;

    // ORIG 0x8aa59c9：第 3 变参是原始 wchar_t* key（"%s" 按窄串打印，原码遗留 P171）。
    ss_sprintf(errStr, "%s == NULL, %s : ",
               key, fileList_.GetFileName(table));
    return errStr;
}

// b28d-21: 已修 P157/P183（operator[] 引用局部 `StringDictionary* const& d` +
// 去 getCount 具名局部，56→24）；余 ORIG setb/sete/setg 物化 vs 4.4
// jae/mov$1 直跳（P197④ setcc-fold）+ 槽号（N82）。ORIG 0x8aa5cba。
#include "RDARScriptStringManager/getKeyIndex.inc"
{
    // ORIG 0x8aa5cba：key==0 或 table 越界 → -1
    if (key == 0)
        return -1;
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG 0x8aa5cba：-0x18 槽存 operator[] 返回的**引用地址**，每次使用
        // 再解引用（`mov -0x18(%ebp),%eax; mov (%eax),%eax`）⇒ 用引用局部
        // 而非指针值缓存（P183/P157）；ORIG 无 getCount 具名局部（循环尾
        // `cmp -0x14(%ebp),%eax; setg` 每轮重调）⇒ 条件内联。
        StringDictionary* const& d = dictionaries_[table];
        for (int i = 0; i < d->getCount(); ++i)
        {
            const char* v = d->getValue(i);
            if (v != 0)
            {
                // ORIG：toUnicode 结果无槽往返 ⇒ 实参内联（P0）
                if (wcscmp(toUnicode(v), key) == 0)
                {
                    // ORIG：getKey 结果落 -0xc 槽后槽比零 ⇒ 具名局部（P0 槽往返）
                    const char* k = d->getKey(i);
                    if (k != 0)
                        return i;
                }
            }
        }
    }
    return -1;
}

// b28d-solo: ORIG 0x8aa59f0 尾部 `mov $0x1,%eax` + 两处 `mov $0x0,%eax`
// ⇒ 真实返回 bool（P159）；三个守卫全部正极性包裹（无 continue），
// `wcsstr(toUnicode(sv), name)` 实参内联（ORIG 无 std::wstring wv 临时）。
#include "RDARScriptStringManager/getTagByName.inc"
                                           std::vector<std::wstring>* b,
                                           int table, const wchar_t* name) const
{
    // ORIG 0x8aa59f0：遍历表；每条 value（去空格）若 wcsstr 包含 name 且 key 非空，
    // 则把 key/value 分别 push 进 a/b。
    if (a == 0 || b == 0)
        return false;
    // ORIG 0x8aa5a21：`setae %al; test %al,%al; je <body>; mov $0,%eax; jmp <end>`
    // ⇒ 早返形（范围失败块内联在函数中段）。
    if (static_cast<unsigned>(table) >= dictionaries_.size())
        return false;
    {
        // ORIG -0x18 槽存 operator[] 引用地址（P157/P183），循环尾每轮重调 getCount。
        StringDictionary* const& d = dictionaries_[table];
        for (int i = 0; i < d->getCount(); ++i)
        {
            const char* v = d->getValue(i);
            if (v != 0)
            {
                std::string sv = v;
                // ORIG 0x8aa59f0：仅 std::remove（未 erase，尾部空格仍保留在串中）。
                std::remove(sv.begin(), sv.end(), ' ');
                // ORIG：toUnicode(sv) 直接作 wcsstr 实参（无 std::wstring wv 临时）。
                if (wcsstr(toUnicode(sv), name) != 0)
                {
                    const char* k = d->getKey(i);
                    if (k != 0)
                    {
                        a->push_back(std::wstring(toUnicode(k)));
                        b->push_back(std::wstring(toUnicode(v)));
                    }
                }
            }
        }
    }
    return true;
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
// [M2S5StringRaw 2026-09-11 复核] 唯一残差（末尾 ss_sprintf 第 3 参装载时刻：
// ORIG 先存 callee 返回值再载 a vs 4.4 先缓存 a 入 %edx）本波新钥匙复核：
// P329 内联回实参位 / P226 语句表达式 / 类型旋钮均无源形可翻（上列 14 形探针
// /tmp/p75.cpp+m9..m14 已覆盖）⇒ P75 封顶维持，169=169 助记符序列全同。
#include "RDARScriptStringManager/getTagByName_1.inc"
                                                     const wchar_t* b, bool flag) const
{
    // ORIG 0x8aa5e40（169 指令逐段转写）：全程窄 char[256] TLS 缓冲
    //（%gs 相对 -0x3c248 / -0x3c148）+ ss_sprintf<256> + strcmp，
    // 没有任何 std::wstring / std::allocator<wchar_t> 临时对象
    //（ORIG 调用点里 _ZNSbIwSt11char_traitsIwESaIwEEC1* / _ZNSaIwEC1Ev 全为 0）。
    // 原来的宽串拼接实现属臆造，每轮循环都多出 basic_string/allocator 构造
    // 与其 EH 清理（eager-ctor 冗余），故按 ORIG 形态改回窄串缓冲。
    // 【b28c-36 复验】fresh ndiff=4 / NEAR（169=169，助记符序列全同）：
    // 唯一残差 = 末尾 ss_sprintf 第 3 参 `(const char*)a` 的装载时刻——
    // ORIG `mov %eax,0xc(%esp); mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)`
    // （先存 callee 返回值再载 a）；4.4 先 `mov 0x10(%ebp),%edx` 缓存再双 store
    // （P75 跨版本实参装载序）。探针 /tmp/b28c_l36_p5.cpp 五形实测：A(reinterpret_cast)
    // 与 B(C cast) 同产 edx 提前缓存；C(文件名局部)/D(指针局部)/E(双局部) 均新增
    // 栈槽且装载序更远 ⇒ 源形不可翻，判 NEAR 封顶。
    // TLS `lea -0x3c248(%eax)` vs `lea 0x0(%eax)` 与 .rodata addend 差属链接产物
    // （.o 未解析重定位；P155 TLS 豁免 + P196 .o 伪差），链接后随 <A> 归一化。
    // [Q7S2 2026-09-11 符号名轴] ORIG 局部 static 的 mangled 名 = `buffer` / `errStr`：
    //   nm 直证 `_ZZNK23RDARScriptStringManager12getTagByNameEiPKwS1_bE6buffer` 与
    //   `…E6errStr`（原 `buf` / `errBuf` 为我方发明名，ORIG 无 `…E3buf`/`…E6errBuf`）。
    //   纯改名，TLS 语义与布局不变，零运行期影响。
    static __thread char buffer[256];      // ORIG TLS -0x3c248
    static __thread char errStr[256];      // ORIG TLS -0x3c148

    (void)flag;                         // ORIG -0x2c：bool 形参归一化存栈后未再使用

    int idx = -1;                       // ORIG -0x24
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG -0x20 存的是 const operator[] 返回的引用地址，每次使用再解引用
        StringDictionary* const& d = dictionaries_[table];
        idx = getKeyIndex(table, a);
        if (idx == -1)
        {
            if (b != 0 && *b != L'\0')
                idx = getKeyIndex(table, b);

            const char* base = 0;       // ORIG -0x18
            if (idx != -1)
                base = d->getKey(idx);
            else
                base = reinterpret_cast<const char*>(tag_.c_str());  // ORIG 按窄串直用

            for (int i = 0; i < 10000; ++i)
            {
                ss_sprintf(buffer, "%s_%d", base, i);
                bool exists = false;                       // ORIG -0xd
                for (int j = 0; j < d->getCount(); ++j)    // ORIG 每轮重调 getCount()
                {
                    if (strcmp(d->getKey(j), buffer) == 0)    // ORIG 无 NULL 检查
                    {
                        exists = true;
                        break;
                    }
                }
                if (!exists)
                {
                    // ORIG：add(key=buf, value=a[0x10(%ebp)]) 直接把 wchar_t* 当 char* 传。
                    d->add(buffer, reinterpret_cast<const char*>(a));
                    idx = d->getCount() - 1;
                    break;
                }
            }
        }

        int unusedSlot = 0;             // ORIG -0x1c：清零后未再引用
        (void)unusedSlot;
        if (idx != -1)
        {
            ss_sprintf(buffer, "<%d::%s>", table, d->getKey(idx));
            return toUnicode(buffer);
        }
    }

    // [M2S5E 2026-09-11] P75 装载序旋钮（变量轴：显式 cast → 裸形参）：
    //   ORIG 0x8aa5e9a `mov %eax,0xc(%esp); mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)`
    //   （先存 callee 返回值，再直接载形参 a）里**没有**任何转换指令；旧形
    //   `reinterpret_cast<const char*>(a)` 让 4.4 视其为需提前求值的表达式，
    //   产 `mov 0x10(%ebp),%edx; mov %eax,0xc(%esp); mov %edx,0x8(%esp)`（+1 条 edx
    //   缓存）。ss_sprintf 为 `template<int N> int ss_sprintf(char (&buffer)[N],
    //   const char* format, ...)`：N 仅由 errBuf[256] 推导，实参位无类型约束
    //   ⇒ 传裸 a（同一指针值，位模式相同）不新实例化符号、语义等价。
    ss_sprintf(errStr, "`%s is not found, %s`",
               a, fileList_.GetFileName(table));
    return reinterpret_cast<const wchar_t*>(errStr);
}

#include "RDARScriptStringManager/setTableValue.inc"
{
    // ORIG 0x8aa5d98
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG：operator[] 结果落具名槽 -0xc 后解引用（P0 有槽往返）
        StringDictionary*& dict = dictionaries_[table];
        dict->setValue(key, value);
    }
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
// [M2S5StringRaw 2026-09-11] 本轮修复 gap 主项：两处 bool 条件原写成取反形
// （`isJpn == false` / `!foundDuplicate`），令 4.4 发 `movzbl; xor $1; test` 4 条，
// 而 ORIG 为 `cmpb $0,mem; je` 2 条（0x5535 `je 0x55b7` 落点是「无检查 add」块、
// 0x5794 `je 0x57a1` 落点是 push_back 块 ⇒ ORIG 源码本就是正向 if）。已改为
// `if (isJpn)`（真块 fall-through 带 NULL 检查、else 为无检查 add）与
// `if (foundDuplicate) return false;`。预期 390 → 386（ORIG 387；余 N18b 的
// r 寄存器比较 1 条我方无）。待父泳道 fresh 编译核账。
#include "RDARScriptStringManager/loadStringFile.inc"
                                             int /*table*/,
                                             std::vector<ErrorLog>* err)
{
    // ORIG 0x8aa529e：① skip 是表达式短路物化（`je L;je L;call bad;test;je L2`
    // + `mov $1/$0,%eax`，无具名 bool 槽，P0）；② auto_ptr 默认构造后 reset
    // （ORIG 0x52ec `auto_ptr(0)` → new → C1 → reset）；③ isJpn/isKor 为
    // 「预清 + 条件置位」（P194）；④ ErrorLog 第三字段 = lineNo（e.m_flag）；
    // ⑤ 重复检测先 getKey(i+1) 后 getKey(i)、无判空、第三处为
    // `e.m_detail = getKey(i)`（ORIG `add $0x8,%eax` + operator=，非 +=）。
    if (in == 0 || path == 0 || in->bad())
        return false;

    std::auto_ptr<StringDictionary> dict;
    dict.reset(new StringDictionary());

    bool isJpn = false;                                          // local_32
    if (std::strstr(path, "Creature.jpn.str") != 0)
        isJpn = true;
    bool isKor = false;                                          // local_31
    if (std::strcmp(path, "Etc/Etc.kor.str") == 0)
        isKor = true;

    int lineNo = 0;
    char lineBuf[0x2928];
    // [M2S5E 2026-09-11] P339 声明序：ORIG 槽序 isJpn(-0x2e)/isKor(-0x2d)/
    //   lineNo(-0x2c)/foundDuplicate(-0x25)/r(-0x24)/i(-0x20)/count(-0x1c)
    //   ⇒ foundDuplicate 的**声明**在 r 之前（4.4 按声明序自深至浅分配）；
    //   旧形在 freeze() 之后才声明，令 4.4 把它放 -0x21、r 放 -0x28（两槽各偏 4）。
    //   赋值点保持在 freeze() 之后（ORIG 0x56xx `movb $0x0,-0x25` 同点）。
    bool foundDuplicate;
    // [M2S5E 2026-09-11 负结论] `volatile int r` 试验：读数 393 vs ORIG 387
    //   （gap 0→−6）且全函数槽整体平移（-0x38→-0x34、-0x2e→-0x2a、-0x2c→-0x28）
    //   ⇒ 回滚。ORIG 0x8aa529e 段两处 r 比较为 4.1 的恒 load 形
    //   `mov -0x24(%ebp),%eax; cmp $A,%eax; je`；4.4 -O0 对普通局部恒发 mem-compare
    //   `cmpl $A,-0x24(%ebp); jne`（该处我方少 1 条）——4.4 无源形可翻（P75 封顶）。
    int r;

    while (!in->eof())
    {
        ++lineNo;
        in->getline(lineBuf, 0x2928);

        char* key = 0;
        char* value = 0;
        r = parseStringLine(lineBuf, &key, &value);

        // [M2S5E 2026-09-11] 决策树 → 真 switch（P 族：手写 if/else-if → switch）：
        //   ORIG 0x8aa529e+0x152 段为 `mov -0x24(%ebp),%eax; cmp $0xfffffffe,%eax;
        //   je <err1>; cmp $0xffffffff,%eax; jne <skip>`（比较链 + 单次 load），
        //   正是 4.1 -O0 对 switch 的发射形；旧 if/else-if 让 4.4 发
        //   `cmpl $A,-0x24(%ebp); jne`（mem-compare，少 1 条 load）。
        switch (r)
        {
        case -1:
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000000);
                e.m_message = path;
                e.m_flag = lineNo;
                err->push_back(e);
            }
            return false;
        case -2:
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000001);
                e.m_message = path;
                e.m_flag = lineNo;
                err->push_back(e);
            }
            return false;
        }

        if (key != 0 && value != 0)
        {
            if (isKor)
            {
                // [b29-Lane21] N78/P214③：块内 `char cpBuf[4000] = { 0 };` 令 4.4 内联
                // `lea; mov $0,%eax; mov $0x3e8,%edx; rep stos`（ORIG 0x0835… 形）；
                // 原函数级声明 + memset 恒发 `call memset`（多 4 条）。
                char cpBuf[4000] = { 0 };
                std::strcpy(cpBuf, value);
                CodePage::script2Database(value, cpBuf);
                value = cpBuf;
            }

            // [M2S5StringRaw 2026-09-11] ORIG 0x5535 `cmpb $0,-0x2e; je 0x55b7`
            // + 0x55b7 块体 = 无检查 add ⇒ ORIG 源码是**正向 `if (isJpn)`**
            // （真块 fall-through 带 NULL 检查，else 块在其后）。旧形写
            // `isJpn == false` 令 4.4 发 `movzbl; xor $1; test; je`（4 条，
            // 与 4.1 的 cmpb+je 2 条不同，属 N122 真根因，非纯后端差）。
            if (isJpn)
            {
                // ORIG 0x5549 `test %al,%al; setne %al; test %al,%al; je <NULL 分支>`
                // ⇒ 正极性包裹（P214①）；toTString 结果无槽往返（P0 实参内联）
                if (*toTString(value) != '\0')
                    dict->add(toTString(key), toTString(value));
                else
                    dict->add(toTString(key), "NULL");
            }
            else
            {
                dict->add(toTString(key), toTString(value));
            }
        }
    }

    dict->freeze();

    // 重复连续 key 检测
    foundDuplicate = false;
    // [M2S5E 2026-09-11] P339 声明序（ORIG 槽深→浅）：ORIG `addl $0x1,-0x20(%ebp)`
    //   + `cmp -0x1c(%ebp),%eax` ⇒ 循环变量 i 在 -0x20（深）、上限 count 在 -0x1c
    //   （浅）；旧形 `int count = ...; for (int i = ...)` 让 4.4 把 count 放 -0x20、
    //   i 放 -0x1c（全函数 10 处 mem 引用互换）。把 i 提到 count 之前声明
    //   （`for (; i < count; ++i)` 保持初始化位置与 ORIG 的 `movl $0x0,-0x20` 同点）。
    int i = 0;
    int count = dict->getCount() - 1;
    for (; i < count; ++i)
    {
        // ORIG 0x564b：strcmp/printf 的 getKey 均无具名槽（P0 实参内联），
        // 第 2 实参（i+1）先求值。
        if (std::strcmp(dict->getKey(i), dict->getKey(i + 1)) == 0)
        {
            std::printf("%s, %s \n", dict->getKey(i), dict->getKey(i + 1));
            foundDuplicate = true;
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000002);
                e.m_message = path;
                e.m_detail = dict->getKey(i);
                err->push_back(e);
            }
        }
    }

    // [M2S5StringRaw 2026-09-11] ORIG 0x5794 `cmpb $0,-0x25; je 0x57a1`：je 跳
    // push_back 块 ⇒ 正向 `if (foundDuplicate) return false;`（旧形 `!foundDuplicate`
    // 令 4.4 发 movzbl+xor+test，4 条 vs ORIG 2 条）。
    if (foundDuplicate)
        return false;
    dictionaries_.push_back(dict.release());
    return true;
}

// b28d-solo: ORIG 0x8aa50bc 尾部 `mov $0x1,%eax` / `mov $0x0,%eax` 两条返回
// 路径 ⇒ 真实返回 bool（P159；返回类型不入 mangled，全仓 0 调用点）。
#include "RDARScriptStringManager/writeStringFile.inc"
{
    // ORIG 0x8aa50bc：写 "Script/<文件名>"，逐条 "key>value\n"，value 内 '\n' 转义
    // 为 "\\n"。ORIG 对 GetFileName 结果无判空（P171 照抄）。
    char path[512];
    ss_sprintf(path, "Script/%s", fileList_.GetFileName(table));
    std::FILE* fp = std::fopen(path, "w");
    // ORIG 0x8aa510b：fp 槽比零物化 `setne %al; test %al,%al; je <尾 mov $0>`
    // ⇒ while+return 形（P225/P194）。
    while (fp != 0)
    {
        if (static_cast<unsigned>(table) < dictionaries_.size())
        {
            // ORIG -0x28 槽存 operator[] 引用地址（P157/P183），循环尾每轮重调
            // getCount（无 n 具名局部）。
            StringDictionary* const& d = dictionaries_[table];
            for (int i = 0; i < d->getCount(); ++i)
            {
                const char* key = d->getKey(i);
                char buf[1024];
                // ORIG：getValue 结果直接作 strcpy 第 2 实参（无三目/无具名 value）。
                std::strcpy(buf, d->getValue(i));

                if (key != 0)
                {
                    // '\n' → "\\n"：从 '\n' 处右移剩余串，插入 '\' 和 'n'。
                    for (char* p = buf; *p != '\0'; ++p)
                    {
                        if (*p == '\n')
                        {
                            // ORIG 槽深（浅→深）q(-0x10) < r(-0x14) < len(-0x18)
                            // ⇒ 声明序取反序（后声明浅，P212 槽向）。
                            int len = 1;
                            char* r;
                            char* q = p + 1;
                            while (*q != '\0')
                            {
                                ++len;
                                ++q;
                            }
                            r = p + len;
                            // ORIG 0x5200：增量段先 `subl $1,cnt` 后 `subl $1,r`
                            // ⇒ 逗号增量形（--cnt, --r）。
                            for (int cnt = len; cnt >= 0; --cnt, --r)
                            {
                                r[1] = *r;
                            }
                            *p = '\\';
                            ++p;
                            *p = 'n';
                        }
                    }
                    std::fprintf(fp, "%s>%s\n", key, buf);
                }
            }
        }
        std::fclose(fp);
        return true;
    }
    return false;
}

#include "RDARScriptStringManager/getTagSizeInFile.inc"
{
    // ORIG 0x8aa5df2
    if (static_cast<unsigned>(table) < dictionaries_.size())
        return dictionaries_[table]->getCount();
    return -1;
}

#include "RDARScriptStringManager/findChangedString.inc"
                                                       bool* flag)
{
    // ORIG 0x8aa6448：函数级 static std::string ret（__cxa_guard 单次初始化），
    // 首次调用以 findString 结果填充，随后 changeSpacilTagStr 并返回 c_str。
    static std::string ret = findString(table, key, flag);
    changeSpacilTagStr(ret);
    return ret.c_str();
}

#include "RDARScriptStringManager/getStringFileList.inc"
{
    // ORIG 0x8aa50b4：STScriptFileList 子对象位于 +0x00，返回 this。
    return this;
}

// b28d-solo-N 封顶（EH 尾段）：前 185 条逐条全等（含两处死拷贝 std::string(s)、
// 局部 OPEN/CLOSE、find/rfind 截断 short、三处 substr 的重复 strlen）；残差全在
// 函数尾部 EH cleanup 决策链——ORIG 每析构点前发
// `test %esi,%esi; jne L; mov $0,%esi; jmp; L: mov $1,%esi`（4.1 landing-pad 决策），
// 4.4 直接顺序析构，属 N91/N104 族（跨版本 EH 结构，无源形解）。返回类型已按
// P159 由 void 改 bool（ORIG 尾 `mov %ebx,%eax`，ebx=1/0）。ORIG 0x8aa60b0。
// [M2S5StringRaw 2026-09-11 复核] gap +14 逐条归因完成：ORIG 独有 3 组 4.1
// landing-pad 决策链（`test %esi,%esi; jne L; mov $0,%esi; jmp L2; L: mov $1,%esi`，
// 6 条/组 ×3 = 18）对 4.4 的 3 条单 `jmp` ⇒ 18−3 = 15 ≈ 全部 gap（余 1 条对齐
// nop 口径差）。P216 方向为「我方多 _Unwind_Resume」，与本件反向不适用；无源形
// 可控项 ⇒ N91/N104 封顶维持。
// [M2S5E 2026-09-11 复验·新旋钮亦封顶] 本轮按「Ghidra bVar1 被复用两次」假说
//   在探针（/tmp/probeRDAR，4.4.6 后端 + 真 TU 旗标）复现 ORIG 结构：
//   `bool ret=false; bool ok = !part1.empty() && !partInner.empty(); if (ok) {
//    std::string out=…; ok = !out.empty(); if (ok) { s.clear(); s=out; ret=true; } }
//    return ret;` ⇒ 255 条（我方现形 259、ORIG 274），**零归一化组** ⇒ 具名 bool
//   在 4.4 -O0 恒落栈（`movzbl -0x9(%ebp),%ebx`），永不复现 ORIG 的 esi sink。
//   结论：ORIG 0x8aa631e..0x8aa63e7 的三组 6 条归一化
//   （`test %esi,%esi; jne L; mov $0x0,%esi; jmp L2; L: mov $0x1,%esi`）是 4.1 -O0
//   把跨 EH-cleanup 的临时保活到寄存器（esi）并在每个析构点重建的行为；4.4 -O0
//   只用栈槽/单次消费（我方 259 条中仅 ~out 后 1 处 `test %esi,%esi; je; mov $0,%ebx`）
//   ⇒ 源形不可翻，gap +14 封顶维持。
#include "RDARScriptStringManager/changeSpacilTagStr.inc"
{
    // ORIG 0x8aa60b0：把 "<URL LINK>内文</URL LINK>" 转为 0x1c 内文 0x1d，
    // 尾部残留追加，最后补 "\n"。任何初始条件不满足返回 false 且不修改 s。
    // 逐条对齐要点：① 两处死拷贝 std::string(s)（ORIG 0x60d2/0x60e4，只析构
    // 不读，P184 照抄）；② OPEN/CLOSE 为**局部** const char*（ORIG -0x24/-0x20
    // 槽写入，非 static 立即数折叠）；③ find/rfind 结果截断存 short（ORIG
    // `mov %ax,-0x1c/-0x1a` + `cmpw $0xffff`，npos→-1）；④ 三处 substr 的
    // strlen 各写两次（ORIG 重复 call）；⑤ 正极性包裹 `!empty() && !empty()`
    // （ORIG `xor $1;test;je` 链，P214①）。
    if (s.empty())
        return false;

    std::string deadCopy1 = s;
    std::string deadCopy2 = s;
    const char* OPEN = "<URL LINK>";
    const char* CLOSE = "</URL LINK>";

    short posOpen = s.find(OPEN, 0);
    if (posOpen == -1)
        return false;
    short posClose = s.rfind(CLOSE, std::string::npos);
    if (posClose == -1)
        return false;
    if (posOpen == posClose)
        return false;

    std::string part1 = s.substr(0, posOpen);
    std::string partInner = s.substr(
        posOpen + std::strlen(OPEN), posClose - posOpen - std::strlen(OPEN));
    std::string partTail = s.substr(
        posClose + std::strlen(CLOSE), s.length() - posClose - std::strlen(CLOSE));

    if (!part1.empty() && !partInner.empty())
    {
        std::string out = part1;
        out += static_cast<char>(0x1c);
        out += partInner;
        out += static_cast<char>(0x1d);
        if (!partTail.empty())
            out += partTail;
        out += "\n";

        if (!out.empty())
        {
            s.clear();
            s = out;
            return true;
        }
    }
    return false;
}

// b28d-21: IDENTICAL（50=50）— 同 D1：迭代器直构 + end 缓存 + `it++` + `delete *it`。
// ORIG 0x8aa5014。
#include "RDARScriptStringManager/release.inc"
{
    // ORIG 0x8aa5014：begin/end 迭代器循环逐元素 delete，随后 clear vector。
    std::vector<StringDictionary*>::iterator it = dictionaries_.begin();
    std::vector<StringDictionary*>::iterator e = dictionaries_.end();
    for (; it != e; it++)
    {
        delete *it;
    }
    dictionaries_.clear();
}

RDARScriptStringManager g_scriptStringManager_;