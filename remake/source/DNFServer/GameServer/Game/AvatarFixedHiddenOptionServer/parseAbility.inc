bool ARAD::SCRIPT::AvatarFixedHiddenOption::parseAbility(
    std::vector<STAvatarHiddenAbilityVariation>& variations, std::string& tag)
{
    // ORIG 0x088aef5b 序言后立即清零 -0x28/-0x24/-0x20 三槽：跨 case 复用的具名局部
    // rate/value/fvalue（float 以 `mov $0x0,%eax; mov %eax,-0x20` 位模式清零）。
    // ORIG：__lookup 为 map<const char*,int,less_TCHAR>，__init_lock 为 SpinLock，
    // __inited 在锁保护下置位；使用真实公共类型以保留跨 TU 依赖和初始化语义。
    int rate = 0;
    int value = 0;
    float fvalue = 0.0f;
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    static LookupMap lookup;
    static SpinLock initLock;
    static bool inited = false;

    for (;;)
    {
        int key;
        if (inited)
        {
            LookupMap::const_iterator it = lookup.find(tag.c_str());
            if (it != lookup.end())
                key = it->second;
            else
                key = -1;
        }
        else
        {
            initLock.enter();
            if (inited)
            {
                initLock.leave();
                continue;   // ORIG 0x088af157 jmp 0x088aef74：重查
            }
            key = 0;        // 哨兵：进入注册链（case 0 与 case 0x46 共享入口）
        }

        switch (key)
        {
        // 每个 case 的 !inited 分支就地构造 value_type（pair<const char* const,int>，
        // 模板实参含 char const(&)[N]，故每个字符串长度一个实例）再 map::insert
        // （value_type const&）——与 ORIG 初始化链逐条同形（0x46..0xac）。用
        // make_pair 会多出 pair<const char*,int> → value_type 的转换临时（第三次
        // call），与 ORIG 不符，故直接构造 value_type。
        case 0:
        case 0x46:   // [physical attack] → type 1
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[physical attack]", 0x46));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_ATTACK, value, rate));
                break;
            }
            // fallthrough：ORIG 0x088af1d9 jmp 0x088af237（下一 case 入口）
        case 0x4b:   // [magical attack] → type 2
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[magical attack]", 0x4b));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_ATTACK, value, rate));
                break;
            }
        case 0x50:   // [physical defense] → type 3
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[physical defense]", 0x50));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_DEFENSE, value, rate));
                break;
            }
        case 0x55:   // [magical defense] → type 4
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[magical defense]", 0x55));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_DEFENSE, value, rate));
                break;
            }
        case 0x5a:   // [HP MAX] → type 5
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[HP MAX]", 0x5a));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::HP_MAX, value, rate));
                break;
            }
        case 0x5f:   // [MP MAX] → type 6
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[MP MAX]", 0x5f));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MP_MAX, value, rate));
                break;
            }
        case 100:    // [HP regen speed] → type 7
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[HP regen speed]", 100));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::HP_REGEN_SPEED, value, rate));
                break;
            }
        case 0x69:   // [MP Regen speed] → type 8
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[MP Regen speed]", 0x69));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MP_REGEN_SPEED, value, rate));
                break;
            }
        case 0x6e:   // [attack speed] → type 9
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[attack speed]", 0x6e));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::ATTACK_SPEED, value, rate));
                break;
            }
        case 0x73:   // [move speed] → type 10
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[move speed]", 0x73));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MOVE_SPEED, value, rate));
                break;
            }
        case 0x78:   // [cast speed] → type 11
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[cast speed]", 0x78));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::CAST_SPEED, value, rate));
                break;
            }
        case 0x7d:   // [inventory limit] → type 12
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[inventory limit]", 0x7d));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::INVENTORY_LIMIT, value, rate));
                break;
            }
        case 0x82:   // [stuck] → type 13（第一个数值为浮点）
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[stuck]", 0x82));
            }
            else
            {
                fvalue = ScanFloat((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::STUCK, fvalue, rate));
                break;
            }
        case 0x87:   // [stuck resistance] → type 14
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[stuck resistance]", 0x87));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::STUCK_RESISTANCE, value, rate));
                break;
            }
        case 0x8c:   // [all activestatus resistance] → type 15
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[all activestatus resistance]", 0x8c));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::ALL_ACTIVESTATUS_RESISTANCE, value, rate));
                break;
            }
        case 0x91:   // [hit recovery] → type 16
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[hit recovery]", 0x91));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::HIT_RECOVERY, value, rate));
                break;
            }
        case 0x96:   // [equipment magical defence] → type 17（首个 ScanInt 弃读）
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[equipment magical defence]", 0x96));
            }
            else
            {
                ScanInt((bool*)NULL);
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_MAGICAL_DEFENCE, value, rate));
                break;
            }
        case 0x9c:   // [equipment physical defence] → type 18（首个 ScanInt 弃读）
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[equipment physical defence]", 0x9c));
            }
            else
            {
                ScanInt((bool*)NULL);
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_PHYSICAL_DEFENCE, value, rate));
                break;
            }
        case 0xa2:   // [jump power] → type 19
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[jump power]", 0xa2));
            }
            else
            {
                value = ScanInt((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::JUMP_POWER, value, rate));
                break;
            }
        case 0xa7:   // [physical critical hit] → type 20（浮点）
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[physical critical hit]", 0xa7));
            }
            else
            {
                fvalue = ScanFloat((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_CRITICAL_HIT, fvalue, rate));
                break;
            }
        case 0xac:   // [magical critical hit] → type 21（浮点）
            if (!inited)
            {
                lookup.insert(LookupMap::value_type("[magical critical hit]", 0xac));
            }
            else
            {
                fvalue = ScanFloat((bool*)NULL);
                rate = ScanInt((bool*)NULL);
                variations.push_back(STAvatarHiddenAbilityVariation(
                    AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_CRITICAL_HIT, fvalue, rate));
                break;
            }
            // fallthrough：ORIG 0x088b0041 jmp 0x088b0096（default 入口）
        default:
            if (inited)
                return false;
            break;
        }

        // 公共尾（ORIG 0x088b00a8）：注册链走完（inited 仍 false）时 leave 并置位，
        // 回到循环头重查；正常动作路径（inited true）直接返回 true。
        if (!inited)
        {
            initLock.leave();
            inited = true;
            continue;   // ORIG 0x088b00c9 jmp 0x088aef74
        }
        return true;
    }
