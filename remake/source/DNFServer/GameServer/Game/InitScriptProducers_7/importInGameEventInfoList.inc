// ============================================================================
// importInGameEventInfoList — ORIG 0x0897eb59（报告 _lt_global_gt_/
// importInGameEventInfoList.md 汇编+Ghidra C 双段转写；init.md +0x674d）。
// 双相派发真形：函数级静态 __lookup map<char const*,int,less_TCHAR>
// （BSS 0x094f7ba8）+ __init_lock SpinLock（0x094f7bc0）+ __inited（0x094f7bc4），
// guard 每轮穿（P177：static 位于重派发循环头）；注册轮持锁 id=0 → switch
// 全 miss → 公共尾 0x0897f0b3 leave+置位+jmp 0x0897ebca（guard 重查同 token）。
// 分派 = `cmpl $0x37; ja; shl $2; jmp *table` 跳表（P163：case 0 并入
// [/event_list] leaf 撑 range）。token→id 逐例 insert（非链式，每块独立）：
// [/event_list]=0x24（命中→result=true 经共享退出岛 0x0897f0fe 返回）、
// [idx]=0x27（ScanInt(int*)→info+0x00 + ScanStr→info+0x08，返回值不判）、
// [record type]=0x2b(+0x04)、[start]=0x2e(+0x0c)、[end]=0x31(+0x10)、
// [state]=0x34(+0x14)、[/event]=0x37（insertEventInfo(info.m_field4, info)
// 0x0897f0ae）。info/token 为函数级临时（-0xd8/-0xc0，循环外构造一次）；
// result(-0x1d) 仅 ScanType EOF / [/event_list] 两退出路径读取（P164 scan_exit
// 岛共享尾：`movzbl -0x1d,%ebx` + dtor 统一块）。
//
// 本波已修（P 范式命中）：
//  P309 key 提函数头声明（`const char* key;` 无 init）⇒ token 槽 -0xc0 对齐；
//  P168a② 公共尾负极性 `if (!__inited) { leave; 置位; goto redo; }` ⇒ 复现
//   ORIG `movzbl __inited; xor $1; test; je <nop 岛>`；
//  [/event_list] 路径改裸 `return true;` ⇒ 4.4 直 `mov $0x1,%ebx; jmp 共享退出岛`
//   （与 ORIG 0x0897ee2b 同形，旧 `result=true; goto` 的槽写已消）。
//
// 残余 DIFF 归因（N 类封顶，语义完成：p167 50/50 CLEAR、orig/ours=364/363）：
//  ① 4.4 循环入口对齐填充 `jmp +2; nop`（info C1 之后、ScanType 之前），ORIG
//     4.1 直接 fall-through（N112 变体，jmp 非 nop 不被 strip_nop_padding 剥离）；
//  ② find 的 sret 临时槽与 key 槽互换：ORIG key -0xb8 / temp -0xbc，4.4 编译器
//     临时恒优先占最浅槽 ⇒ key -0xbc / temp -0xb8（N82/N86 槽分配族；穷举
//     具名 iterator/声明序互换/内联实参三形均不闭合）。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N112/N82] orig/ours=364/363
//   jcc 20:20 逐位同构（含 0x37 跳表前置 `cmpl $0x37; ja`），仅 3 处 jmp/nop
//   岛位置差（我方循环入口多 `jmp +2; nop`、ORIG 尾多 `jmp; nop`；N112/N121）；
//   p167 别名口径 CLEAR（MISSING C2/D2 ↔ EXTRA C1/D1 配对，P93）；
//   无 call *（无虚调用）；无语义缺口。
bool importInGameEventInfoList(InGameEventInfoList* list, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    const char* key;                  // ORIG -0xb8（函数头声明，P309 先声明浅；无 init）
    std::string token;                // ORIG -0xc0
    bool result = false;              // ORIG -0x1d（0x0897eb97 movb $0）
    InGameEventInfo info;             // ORIG -0xd8（循环外 C1 0x0897eba4）

    while (true)
    {
        if (!ScanType(token, true))
        {
            break;                    // EOF → return result(false)（0x0897f0df）
        }

    redo_lookup:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            key = token.c_str();                     // 0x0897ecd6
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(key);
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                     // 0x0897ed8d
            if (__inited)
            {
                __init_lock.leave();                 // 0x0897eda4
                goto redo_lookup;                    // 0x0897edaa → guard
            }
            id = 0;                                  // 0x0897edaf
        }

        switch (id)
        {
        case 0:
        case 0x24:                       // [/event_list]（0x0897edce leaf）
            // ORIG 各 leaf `movzbl __inited; xor$1; test; je parse` —— 取反
            // 短路物化形（P32：!bool 判产生 xor$1），注册块落 if 体。
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[/event_list]", 0x24));
                break;                               // → 公共尾
            }
            return true;                             // 0x0897ee2b mov$1,%ebx
        case 0x27:                       // [idx]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[idx]", 0x27));
                break;
            }
            ScanInt(&info.m_eventId);                // 0x0897ee9b（返回值弃）
            ScanStr(&info.m_name);                   // 0x0897eeac
            break;
        case 0x2b:                       // [record type]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[record type]", 0x2b));
                break;
            }
            ScanInt(&info.m_field4);
            break;
        case 0x2e:                       // [start]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[start]", 0x2e));
                break;
            }
            ScanInt(&info.m_startTime);
            break;
        case 0x31:                       // [end]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[end]", 0x31));
                break;
            }
            ScanInt(&info.m_endTime);
            break;
        case 0x34:                       // [state]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[state]", 0x34));
                break;
            }
            ScanInt(&info.m_isEventing);             // +0x14 int（头注实证）
            break;
        case 0x37:                       // [/event]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[/event]", 0x37));
                break;
            }
            list->insertEventInfo(info.m_field4, info);   // 0x0897f0ae
            break;
        default:
            break;                                   // 未命中静默（含 "[event_list]" 开标签）
        }

        // ORIG 公共尾取反物化形：movzbl __inited; xor $1; test; je <nop 岛>
        // ⇒ 源码写负极性 `if (!__inited) { leave; 置位; 重派发; }`（P168a②）。
        if (!__inited)
        {
            __init_lock.leave();                     // 0x0897f0c8
            __inited = true;                         // 0x0897f0cd
            goto redo_lookup;                        // 0x0897f0d4 → guard 重派发
        }

    scan_next:;
    }
    return result;                                   // 共享退出岛 0x0897f0fe
}