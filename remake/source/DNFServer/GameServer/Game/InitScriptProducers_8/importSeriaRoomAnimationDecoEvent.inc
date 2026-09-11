// ============================================================================
// importSeriaRoomAnimationDecoEvent —— ORIG 0x08960861
// [using item] pair 序列清+填 m_itemMap；[reward item] 第三列弃读的二元组
// push m_rewardList；[ani list] 行 “key,string,string(layer 名),int×3” 经
// getDrawLayer 转层号入 m_aniList；[finish sound] 直写 m_finishSound；
// [finish ani] 循环 “[front]/[back]” 选层（静态表 0x318/0x31b），随后行
// string(name)+string(layer)+int×3 入 m_finishAniMap；未知层名返回失败。
// 该段仅能经 EOF（ScanStr 失败）退出。打开/空指针返回 0，EOF 返回 1。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap0] ORIG 08960861：p167 CLEAR（90/90）；fresh .o
// 628/628（gap=0）。本轮修真 bug：①[reward item] 第三列由 `(void)ScanInt(&value)`
// 改为 `if(!ScanInt(&reward.m_field8)) return false;`（ORIG 0x8960a36..a53 写对象
// +0x08 并检查返回值——旧码写错目标且缺失败门禁）；②key/val 提升为函数级局部
// （ORIG 0x89608bd/c7 入口初始化 -0xec/-0xf0，[using item]/[ani list] 复用）。
// 并按 P333 重写 [finish ani] 惰性注册链：首轮 v=0 沿 case 0→0x318→0x31b
// fallthrough 建表（ORIG 0x8960efb/0x8960f55），case 体负极性 if(!__inited)，
// default 在 __inited 时 return false（未知层名），三件套声明移入子循环（P177，
// ORIG 重扫 0x8960fc9 jmp 0x8960cf2）。
// [P1T2 2026-09-11 复修] ①删多余具名局部 `layer`（P0）：ORIG 无该槽，make_pair
// 左值实参直接取 layerId 的槽（ORIG 0x89610d6 `lea -0x128(%ebp),%edx`）；旧码
// `T layer = static_cast<T>(layerId)` 多占 4 字节 ⇒ 全函数栈槽整体下移 4
// （ORIG -0xe4 ↔ 我方 -0xe8，33 槽全体），删后槽集合逐一对齐（slotcmp 实证）。
// ②[finish sound] else-if 块移到 [finish ani] 之后：ORIG 块序实证 0x89611bb
// （偏移 0x95a，在 [finish ani] 段 0x8960e..–0x89611.. 之后），4.4 按源码序摆放；
// token 互斥，语义不变。效果：86 差异块 → 3（clean 620/621，gap -1→+1）。
// 残余 N 类：①0x8960938 `jne 89609c1` 尾块 vs 4.4 `je`+`jmp` 就地物化；
// ②pair 构造实参寄存器 ORIG %edx vs 4.4 %ecx（求值序）；A/B 三档
// （-B cc1plus446bin、-B …/4.4.4/、-fno-thread-jumps）读数逐项相同。
bool importSeriaRoomAnimationDecoEvent(class seriaRoomDecoAniEventScript* s,
                                       const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string layerName;
    int key = 0;                                        // ORIG -0xec（[using item]/[ani list] 复用）
    int val = 0;                                        // ORIG -0xf0（[using item]）
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                                    // EOF → 成功

        if (token == "[using item]")
        {
            s->m_itemMap.clear();
            for (;;)
            {
                // ORIG：ScanInt(int*)（p167 MISSING ScanInt(int*)×3）；key/val 为
                // 函数级局部（ORIG 0x89608bd/0x89608c7 入口初始化 -0xec/-0xf0）。
                if (!ScanInt(&key))
                    break;
                if (!ScanInt(&val))
                    return false;
                // ORIG pair<int,int> 转发构造（非 make_pair）
                s->m_itemMap.insert(std::pair<int, int>(key, val));
            }
        }
        else if (token == "[reward item]")
        {
            s->m_rewardList.clear();
            for (;;)
            {
                seriaRoomDecoRewardInfo reward;
                if (!ScanInt(&reward.m_itemIdx))
                    break;
                if (!ScanInt(&reward.m_reward))
                    return false;
                // ORIG 0x8960a36..a53：第三列写 reward.m_field8（-0xfc+8）且
                // 检查 ScanInt 返回值（失败 → return false）；旧 `(void)ScanInt
                // (&value)` 既写错目标又缺失败门禁。
                if (!ScanInt(&reward.m_field8))
                    return false;
                s->m_rewardList.push_back(reward);
            }
        }
        else if (token == "[ani list]")
        {
            s->m_aniList.clear();
            seriaRoomDecoAniInfo info;
            for (;;)
            {
                info.clear();
                if (!ScanInt(&key))
                    break;
                if (!ScanStr(&info.m_name))
                    return false;
                if (!ScanStr(&layerName))
                    return false;
                info.m_layer = getDrawLayer(layerName.c_str());
                if (!ScanInt(&info.m_a))
                    return false;
                if (!ScanInt(&info.m_b))
                    return false;
                if (!ScanInt(&info.m_c))
                    return false;
                s->m_aniList.insert(std::make_pair(key, info));
            }
        }
        else if (token == "[finish ani]")
        {
            seriaRoomDecoAniInfo info;
            for (;;)
            {
                info.clear();
                if (!ScanStr(&layerName))              // 层选择 token
                    break;                             // EOF → 回主循环（成功）

                // ORIG 0x8960cf2：三件套声明落子循环体内（每轮穿 guard，P177）。
                static std::map<const char*, int, less_TCHAR> __lookup;   // ORIG __lookup 0x94f75b8
                static SpinLock __init_lock;                              // ORIG __init_lock 0x94f75d0
                static bool __inited = false;                             // ORIG __inited 0x94f75d4

            retry:
                // P333 状态机：首轮 v=0 沿 case 0→0x31b fallthrough 建表
                // （ORIG 0x8960efb/0x8960f55），链尾置 __inited 后 goto retry
                // 重查（ORIG 0x8960fc9 jmp 0x8960cf2）；此后 v=find 结果。
                int v;
                if (__inited)
                {
                    // ORIG 8960e8c..8960ee5：ne 判定后 found->second 只读一次
                    // （ptEv×1，结果落 -0x1c 槽）再与 0x318/0x31b 比较。
                    std::map<const char*, int, less_TCHAR>::const_iterator found =
                        __lookup.find(layerName.c_str());
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
                    v = 0;                             // 建表哨兵
                }

                int layerId;
                typedef std::map<const char*, int, less_TCHAR>::value_type V;
                switch (v)
                {
                case 0:
                case 0x318:                            // [front]
                    // ORIG 0x8960f02：负极性 `xor $1; test; je`（P168a②）。
                    if (!__inited)
                    {
                        __lookup.insert(V("[front]", 0x318));
                    }
                    else
                    {
                        layerId = 0;
                        break;
                    }
                    // fallthrough
                case 0x31b:                            // [back]
                    if (!__inited)
                    {
                        __lookup.insert(V("[back]", 0x31b));
                    }
                    else
                    {
                        layerId = 1;
                        break;
                    }
                    // fallthrough
                default:
                    if (__inited)
                        return false;                  // 未命中/未知层名 → 失败
                    break;
                }

                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    goto retry;
                }

                if (!ScanStr(&info.m_name))
                    return false;
                if (!ScanStr(&layerName))
                    return false;
                info.m_layer = getDrawLayer(layerName.c_str());
                if (!ScanInt(&info.m_a))
                    return false;
                if (!ScanInt(&info.m_b))
                    return false;
                if (!ScanInt(&info.m_c))
                    return false;
                // ORIG make_pair<aniLayer::T&, seriaRoomDecoAniInfo&>（左值实参）：
                // 实参就是 layerId 本身的槽（ORIG 0x89610d6 `lea -0x128(%ebp),%edx`
                // 直取 layerId 地址），无 layer 具名槽（P0）。
                s->m_finishAniMap.insert(std::make_pair(
                    reinterpret_cast<seriaRoomDecoAniEventScript::aniLayer::T&>(
                        layerId),
                    info));
            }
        }
        // ORIG 块序：0x89611bb（函数尾，偏移 0x95a）才是 [finish sound] 段，
        // 位于 [finish ani] 段（0x8960e..–0x89611..）之后；4.4 按源码序摆放，
        // 故此处链序须与 ORIG 一致（token 互斥，语义不变）。
        else if (token == "[finish sound]")
        {
            ScanStr(&s->m_finishSound);
        }
    }
    return true;

}