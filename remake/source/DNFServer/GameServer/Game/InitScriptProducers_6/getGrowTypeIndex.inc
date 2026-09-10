int getGrowTypeIndex(const char* name)
{
    int idx = -1;                     // ORIG 首指令 movl $0xffff,-0x20
    for (;;)
    {
        static GrowTypeMap __lookup;  // ORIG ...E8__lookup
        static SpinLock __init_lock;                             // ORIG ...E11__init_lock
        static bool __inited = false;                            // ORIG ...E8__inited
        // [L16 本轮] 极性翻正（P222/P214①）：ORIG 为 `movzbl __inited; test %al,%al;
        // je else` 正极性（无 xor），我方原 `!__inited` 形多出发 `xor $0x1,%eax`。
        if (__inited)
        {
            GrowTypeMap::const_iterator it =
                __lookup.find(name);
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;               // ORIG 088be81f jmp 088be650（回 guard 重检）
            }
            idx = 0;
        }
        switch (idx)
        {
        // 建表模式（首调 !__inited，自 case 0 进入）逐 case 贯穿 insert 全表；
        // 派发模式（__inited）组内各码经 je 链汇聚共享结果槽后 return。
        case 0:
        case 0x8b5:
            if (__inited) return 0;
            __lookup.insert(GrowTypeMap::value_type("none", 0x8b5));
            /* fall through */
        case 0x8ba:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("weaponmaster", 0x8ba));
            /* fall through */
        case 0x8bb:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("nenmaster", 0x8bb));
            /* fall through */
        case 0x8bc:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("ranger", 0x8bc));
            /* fall through */
        case 0x8bd:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("elementalmaster", 0x8bd));
            /* fall through */
        case 0x8be:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("crusader", 0x8be));
            /* fall through */
        case 0x8bf:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("rogue", 0x8bf));
            /* fall through */
        case 0x8c1:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("elementalbomber", 0x8c1));
            /* fall through */
        case 0x8c7:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("soulbringer", 0x8c7));
            /* fall through */
        case 0x8c8:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("striker", 0x8c8));
            /* fall through */
        case 0x8c9:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("launcher", 0x8c9));
            /* fall through */
        case 0x8ca:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("summoner", 0x8ca));
            /* fall through */
        case 0x8cb:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("infighter", 0x8cb));
            /* fall through */
        case 0x8cc:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("necromancer", 0x8cc));
            /* fall through */
        case 0x8ce:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("glacialmaster", 0x8ce));
            /* fall through */
        case 0x8d4:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("berserker", 0x8d4));
            /* fall through */
        case 0x8d5:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("streetfighter", 0x8d5));
            /* fall through */
        case 0x8d6:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("mechanic", 0x8d6));
            /* fall through */
        case 0x8d7:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("battlemage", 0x8d7));
            /* fall through */
        case 0x8d8:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("exorcist", 0x8d8));
            /* fall through */
        case 0x8dd:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("asura", 0x8dd));
            /* fall through */
        case 0x8de:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("grappler", 0x8de));
            /* fall through */
        case 0x8df:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("spitfire", 0x8df));
            /* fall through */
        case 0x8e0:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("witch", 0x8e0));
            /* fall through */
        case 0x8e1:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("avenger", 0x8e1));
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;                   // ORIG 088bf2a4 jmp 088be650 重派发
        }
        return 0xffff;                  // ORIG 未命中/未知码：local_24 初值
    }
