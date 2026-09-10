bool importArtifactInfo(STEquipmentScript::artifact_info& info,
                        std::string str)
{
    // [SweepC 2026-09-09 P333 首次建表状态机还原] 0x089bd079：ORIG 450 / 我方 354
    // （gap 96；p167 双向 0/0 ⇒ 纯控制流形态差）。ORIG 真形 = 静态表在**循环体内**
    // 声明（guard 检查随 continue 回跳重放，0x089bd085）+ 11 个 case 链式 fallthrough
    // 注册：`if (__inited) { 读值; break; } insert(...); // fallthrough`；
    // 尾块 0x089bd80d：`if (!__inited) { leave(0x089bd822); __inited=true; continue; }`。
    // 表：map<const char*,int,less_TCHAR> @0x94fdf28 + SpinLock @0x94fdf40 +
    // 就绪字节 @0x94fdf44；11 个 [creature *] 关键字 → id（0x2ee9..0x2f07），
    // 未命中返回 0，命中按 id 分派 ScanInt/ScanFloat 写回。
    // 扫描参数：3 个 int 字段 ScanInt(0)；7 个 rate 字段 ScanFloat(scale,0)
    // ——mp/charge/overcharge/exp 用 scale=2，crit×2/stuck 用 scale=1。
    do
    {
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            // [P236 2026-09-10] ORIG 0x089bd0? 的 c_str 结果槽（-0x104）在 find 的
            // sret 槽（-0x108）**之上**：原码是内联实参 `find(str.c_str())`
            // （临时槽序 = key 浅 / sret 深），具名 key 局部反装槽序。
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(str.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                              // 回循环头（重放 guard）
            }
            id = 0;
        }

        switch (id)
        {
        case 0:
        case 0x2ee9:                                   // [creature minimum level]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature minimum level]", 0x2ee9));
            }
            else
            {
                info.m_minLevel = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        case 0x2eec:                                   // [creature physical attack]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature physical attack]", 0x2eec));
            }
            else
            {
                info.m_physAttack = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        case 0x2eef:                                   // [creature magical attack]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature magical attack]", 0x2eef));
            }
            else
            {
                info.m_magAttack = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        case 0x2ef2:                                   // [creature skill consume mp rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature skill consume mp rate]", 0x2ef2));
            }
            else
            {
                info.m_mpRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2ef5:                                   // [creature skill charge time rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature skill charge time rate]", 0x2ef5));
            }
            else
            {
                info.m_chargeRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2ef8:                                   // [creature skill over charge time rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature skill over charge time rate]", 0x2ef8));
            }
            else
            {
                info.m_overChargeRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2efb:                                   // [creature experience amount rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature experience amount rate]", 0x2efb));
            }
            else
            {
                info.m_expRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2efe:                                   // [creature physical critical hit]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature physical critical hit]", 0x2efe));
            }
            else
            {
                info.m_physCrit = ScanFloat(1, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2f01:                                   // [creature magical critical hit]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature magical critical hit]", 0x2f01));
            }
            else
            {
                info.m_magCrit = ScanFloat(1, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2f04:                                   // [creature stuck]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature stuck]", 0x2f04));
            }
            else
            {
                info.m_stuck = ScanFloat(1, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2f07:                                   // [creature piercing]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature piercing]", 0x2f07));
            }
            else
            {
                info.m_piercing = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        default:
            if (__inited)
            {
                return false;                          // ORIG 未命中 → 0
            }
            // fallthrough 到尾块（ORIG 0x089bd7fb）
        }

        if (!__inited)
        {
            // ORIG 0x089bd822/0x089bd827：leave → __inited=true → 回循环头
            __init_lock.leave();
            __inited = true;
            continue;
        }
        break;                                         // 已建表 → 退出并返回 true
    } while (true);
    return true;
