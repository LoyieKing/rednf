// ============================================================================
// convertTabTypeStringToEnum —— ORIG T 0x08aa7d98
// 报告 _lt_global_gt_/convertTabTypeStringToEnum.md 全文转写。
// 函数级静态 __lookup(map<char const*,int,less_TCHAR>)/__init_lock(SpinLock)/
// __inited 三件套：首调登记 general/specific/despair/event → 0x11/0x14/0x17/0x1a；
// 之后查表命中返回 0..3，未命中返回 4。
// ============================================================================
int convertTabTypeStringToEnum(const char* text)
{
    // [b28d-solo-ScriptLeafA2] 结构按 ORIG 0x08aa7d98 还原（报告 Ghidra C 全文）：
    //   ret 局部 + 合并 switch——FIND 命中值（0x11/0x14/0x17/0x1a）与初始化哨兵
    //   0 共用同一 switch（case 0 与 0x11 共块，P163 的"补未列 case 0"同族）；
    //   各 case 的 `!__inited` 分支插入、else 返回 N；未命中 ret=-1 落 switch 外。
    //   已修：①insert 实参改 map::value_type 直构（P315）→ p167 CLEAR（29=29）；
    //   ②迭代器改 `!= end`（ORIG operator!=）；③__inited 改 bool + `!` 极性复现
    //   ORIG `movzbl;xor $1;test;je`（P168a/N122）。帧 sub $0x8c 与 ORIG 全等。
    //   本波（fresh）再修三处极性/布局（P292/P222/P59 族，ORIG 0x08aa7d98）：
    //   ① 头段 ORIG 为 **正极性直测** `movzbl __inited; test; je <lock 段>`
    //      ⇒ 源码写 `if (__inited) {find} else {lock}`（then=find 落 fall-through，
    //      lock 段在后），消去 4.4 对 `!` 的 `xor $1` 并翻正块序；
    //   ② 尾段 ORIG 为 `movzbl; xor $1; test; je <nop;mov $4>` ⇒ 源码写
    //      `if (!__inited) {leave; __inited=1; continue;} return 4;`（leave 段
    //      fall-through、return 4 在 else 后）；
    //   ③ `int ret = 0;` → `int ret;`（ORIG 函数头无 `movl $0,-0x1c` 初始化，
    //      仅 else 段赋值 ret=0，P197③ 两段式）。
    int ret;
    for (;;)
    {
        // [R22a13 2026-09-13 P333 形] ORIG 0x08aa7da4 为循环回跳锚（= guard1 检查
        //   `movzbl _ZGVZ26convertTabTypeStringToEnumPKcE8__lookup`，`continue`
        //   直跳 +0xc：nop; jmp 08aa7da4）；我方 static 声明置于 for 外 ⇒ 4.4
        //   回跳 +0x106（`__inited` 检查）且经 +0x362 跳板（jmp; nop; jmp）。
        //   把三 static 声明移入循环体：首轮仍只构造一次（guard 置位后跳过），
        //   后续每轮重查 guard1/guard2/__inited —— 与 ORIG 逐点同形。
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        // ① 正极性：`if (__inited)` 直测（4.4 对 bool 局部 `!` 恒 xor，N122）
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(text);
            // ORIG 0x08aa7ea7 段调 operator!=（非 ==，p167 EXTRA ==/MISSING !=）。
            ret = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            ret = 0;
        }

        switch (ret)
        {
        case 0:
        case 0x11:
            // p167：ORIG 直接调 value_type::C1<char const(&)[N], int>（无 make_pair
            // 中转），按 P315 用 map::value_type 直接构造。极性写 `!__inited`
            // 复现 ORIG 0x08aa7fc0 段 `movzbl;xor $1;test;je`（P168a/N122）。
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "general", 0x11));
            }
            else
            {
                return 0;   // general
            }
        case 0x14:
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "specific", 0x14));
            }
            else
            {
                return 1;   // specific
            }
        case 0x17:
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "despair", 0x17));
            }
            else
            {
                return 2;   // despair
            }
        case 0x1a:
            if (!__inited)
            {
                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                    "event", 0x1a));
            }
            else
            {
                return 3;   // event
            }
        }

        // ② 尾段固定形：`!__inited` ⇒ leave+置位+回跳落 fall-through，
        //    `return 4` 走 else 后的尾岛（ORIG 0x08aa80d9 段 `xor $1; test;
        //    je <nop;mov $4>`）。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = 1;
            continue;
        }
        break;      // 未命中：落到函数尾的单一 return（4.4 尾块 fall-through）
    }
    return 4;
}