// ============================================================================
// importUseJobFatigueGiftScript —— ORIG 0x0895f817
// 函数级静态 tag→id 表（ORIG __lookup 0x94f75f8/guard+__init_lock
// 0x94f7610/__inited 0x94f7614）：[/job fatigue]=0x217、[job type]=0x21b、
// [fatigue title]=0x229、[fatigue text]=0x22d、[reward list]=0x231、
// [/job type]=0x252。[/job type] 时 insert(jobType,gift) 并复位累计态。
// [reward list] 行校验：第 2 列 >=0、第 3 列 >0，违规立即返回失败。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap1] ORIG 0895f817：p167 CLEAR（62/62）；fresh .o
// 472/473（gap=1）。本轮按 P333 状态机重写惰性注册链：首轮 v=0 沿 case 0→0x217→
// 0x21b→0x229→0x22d→0x231→0x252 fallthrough 逐条 insert（ORIG 0x895fae7..fe75），
// 每 case 负极性 `if(!__inited){insert}else{...break;}`（ORIG `xor $1; test; je`；
// 旧一次性建表形净缺 45 条）；case 0/0x217 命中即 `return true`（ORIG 0x895fb44；
// 旧"无操作"是语义错）；EOF 改 break+循环外 `return ok`（P164 配套，ORIG 尾共享块
// 0x895ffaf 返回 ok 槽）。已对齐：value_type 转发构造（P315）、三件套移入循环体
// （P177）、fatigueMatchGift ctor throw()（P216）。残余仅循环入口旋转 jmp/nop 岛
// 与尾部岛——N104/N121 封顶。
// [P1T2 复验 2026-09-11] 残余 2 块（clean 470/470；原始 472/473）：①我方在
// `movl $0xffffffff,-0x124(%ebp)`（jobType=-1）后多一条 `jmp` 到紧邻 fall-through
// 的循环头（我方 0x10853）；②ORIG 0x895ffa4 `jmp 0x895f8a5` 循环头中继（我方
// 直接 jmp 到目标）。A/B 三档（-B cc1plus446bin、-B …/4.4.4/、-fno-thread-jumps）
// 读数逐项相同 ⇒ 源码不可翻。
bool importUseJobFatigueGiftScript(class useCharacterJobFatigue* out,
                                   const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (out == 0)
        return false;

    std::string token;
    bool ok = false;
    useCharacterJobFatigue::fatigueMatchGift gift;          // ctor 全清零
    int jobType = -1;                                       // ORIG local_128

    for (;;)
    {
        // ORIG 0x895f89f jne 0x895ffaf：EOF 走共享尾块返回 ok 槽（-0x11d）
        // 当前值（P164 配套：break + 循环外统一 return 得 ORIG 尾岛形）。
        if (!ScanType(token, true))
            break;

        // ORIG 0x895f8a5：三件套声明落循环体内（每轮穿 guard，P177）。
        static std::map<const char*, int, less_TCHAR> __lookup;  // ORIG __lookup
        static SpinLock __init_lock;                              // ORIG __init_lock
        static bool __inited = false;                             // ORIG __inited

    retry:
        // P333 状态机：首轮（未建表）v=0 进 case 0，沿 fallthrough 链逐条
        // 建表（ORIG 0x895fae7/0x895fb4e/0x895fc09/0x895fc7c/0x895fcef/
        // 0x895fe75），链尾置 __inited 后 goto retry 重查；此后 v=find 结果。
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            v = (found != __lookup.end()) ? found->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            v = 0;                                           // 建表哨兵
        }

        // P315：ORIG 用 value_type 转发构造（pair<const char* const,int>
        // C1<char const(&)[N],int>），非 make_pair（后者多一层转换构造+EH）。
        typedef std::map<const char*, int, less_TCHAR>::value_type V;
        switch (v)
        {
        case 0:
        case 0x217:                                          // [/job fatigue]
            // ORIG 0x895faee：负极性 `xor $1; test; je`（P168a②：写 if(!__inited)）。
            if (!__inited)
                __lookup.insert(V("[/job fatigue]", 0x217));
            else
                return true;                                 // ORIG 0x895fb44
            // fallthrough
        case 0x21b:                                          // [job type]
            if (!__inited)
            {
                __lookup.insert(V("[job type]", 0x21b));
            }
            else
            {
                std::string jobName;
                ScanStr(&jobName);
                jobType = getCharacterJobToEnumValue(jobName);   // ORIG 0x895fbe0
                break;
            }
            // fallthrough
        case 0x229:                                          // [fatigue title]
            if (!__inited)
            {
                __lookup.insert(V("[fatigue title]", 0x229));
            }
            else
            {
                ScanStr(&gift.m_msg1);                       // +0x1c
                break;
            }
            // fallthrough
        case 0x22d:                                          // [fatigue text]
            if (!__inited)
            {
                __lookup.insert(V("[fatigue text]", 0x22d));
            }
            else
            {
                ScanStr(&gift.m_msg2);                       // +0x20
                break;
            }
            // fallthrough
        case 0x231:                                          // [reward list]
            if (!__inited)
            {
                __lookup.insert(V("[reward list]", 0x231));
            }
            else
            {
                for (;;)
                {
                    int first = 0;
                    int second = 0;
                    int third = 0;
                    first = ScanInt(&ok);
                    if (!ok)
                        break;
                    second = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (second < 0)
                        return false;                        // ORIG jns 校验
                    third = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (third <= 0)
                        return false;                        // ORIG jg 校验
                    gift.m_field18 = third;                  // mov %eax,-0x140
                    gift.m_rewardMap.insert(std::make_pair(first, second));
                }
                break;
            }
            // fallthrough
        case 0x252:                                          // [/job type]
            if (!__inited)
            {
                __lookup.insert(V("[/job type]", 0x252));
            }
            else
            {
                out->m_giftMap.insert(
                    std::make_pair(jobType, gift));          // ORIG make_pairIRiRN..E
                gift.m_field18 = 0;                          // movl $0,-0x140
                gift.m_rewardMap.clear();                    // map::clear(-0x158)
                jobType = -1;                                // movl $-1,-0x124
                break;
            }
            break;
        default:
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }
    }
    return ok;
}