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
        else if (token == "[finish sound]")
        {
            ScanStr(&s->m_finishSound);
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
                // ORIG make_pair<aniLayer::T&, seriaRoomDecoAniInfo&>（左值实参）
                seriaRoomDecoAniEventScript::aniLayer::T layer =
                    static_cast<seriaRoomDecoAniEventScript::aniLayer::T>(
                        layerId);
                s->m_finishAniMap.insert(std::make_pair(layer, info));
            }
        }
    }
    return true;
