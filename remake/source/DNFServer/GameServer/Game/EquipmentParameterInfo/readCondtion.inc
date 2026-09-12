// ORIG 0x0899ce3b（0x4401 字节）。真实结构：static map<const char*,int> 惰性
// 注册各关键字 → 分支 id，另以 __init_lock/__inited 双检锁保护首次注册；
// 注册完成后按分支 id 逐条 if 分派。注册/加锁仅为编译期优化，行为等价于
// 按关键字字符串 if/else-if 链（与本文件 readAction/importSkillDataUp 注释
// 约定一致），此处省略 map/锁直接分派。每个命中分支按 ORIG 反编译顺序
// Scan* 消费词法流并向 values push「tag 常量 + 参数」；失败的分支按其
// ORIG 语义处理（多数分支不检查扫描结果；[is stat]/[equipment upgrade]
// 任一步失败即直接 return，不再 push）。
// [b28d-solo-EquipmentParameterInfo] P167 仅 EXTRA _Unwind_Resume×2 ⇒ N104 EH 布局封顶。
void readCondtion(EquipmentParameterInfo* info, std::string& line,
                  std::vector<int>& values)
{
    // [ORIG 结构还原] 0x899ce3b：与 readAction 同形 —— 函数级静态查表 +
    // Duff 式惰性注册链：__inited==0 时 id 强制 0 进 switch，每 case 头
    // `if (!__inited) { insert(own tag); goto next-case-guard }`，链尾（ORIG
    // 0x43a3）leave+置位后跳回重查；建表完成后 body 走共享尾。
    std::string token;
    int v = 0;
    int k = 0;
    bool isType = false;
    bool isInt = false;
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    int id;
 relaunch:
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(line.c_str());
        id = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();
        if (__inited)
        {
            __init_lock.leave();
            goto relaunch;
        }
        id = 0;
    }
    switch (id)
    {
    case 0xf22:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[cooltime]", 0xf22));
            goto L_f28;
        }
    {
        values.push_back(0);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0xf28:
    L_f28:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[start cooltime]", 0xf28));
            goto L_f2e;
        }
    {
        values.push_back(1);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0xf2e:
    L_f2e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[character]", 0xf2e));
            goto L_f34;
        }
    {
        values.push_back(2);
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf34:
    L_f34:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[casting]", 0xf34));
            goto L_f39;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(3);
        }
        break;
    }
    case 0xf39:
    L_f39:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[distance]", 0xf39));
            goto L_f45;
        }
    {
        values.push_back(4);
        ScanStr(&token);
        k = 2;
        if (token == "near")
        {
            k = 0;
        }
        else if (token == "far")
        {
            k = 1;
        }
        values.push_back(k);
        break;
    }
    case 0xf45:
    L_f45:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[my active status]", 0xf45));
            goto L_f4b;
        }
    {
        values.push_back(5);
        ScanStr(&token);
        v = getActiveStatusToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf4b:
    L_f4b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[my state]", 0xf4b));
            goto L_f52;
        }
    {
        values.push_back(6);
        ScanStr(&token);
        v = getStateToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf52:
    L_f52:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[my prev state]", 0xf52));
            goto L_f58;
        }
    {
        values.push_back(7);
        ScanStr(&token);
        v = getStateToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf58:
    L_f58:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target prev state]", 0xf58));
            goto L_f5f;
        }
    {
        values.push_back(8);
        ScanStr(&token);
        v = getStateToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf5f:
    L_f5f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[my appendage]", 0xf5f));
            goto L_f64;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x28);
        values.push_back(v);
        break;
    }
    case 0xf64:
    L_f64:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target active status]", 0xf64));
            goto L_f6a;
        }
    {
        values.push_back(9);
        ScanStr(&token);
        v = getActiveStatusToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf6a:
    L_f6a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target state]", 0xf6a));
            goto L_f70;
        }
    {
        values.push_back(0xa);
        ScanStr(&token);
        v = getStateToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf70:
    L_f70:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[my active status on]", 0xf70));
            goto L_f76;
        }
    {
        values.push_back(0xb);
        ScanStr(&token);
        v = getActiveStatusToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf76:
    L_f76:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target active status on]", 0xf76));
            goto L_f7c;
        }
    {
        values.push_back(0xc);
        ScanStr(&token);
        v = getActiveStatusToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf7c:
    L_f7c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[element]", 0xf7c));
            goto L_f82;
        }
    {
        values.push_back(0xd);
        ScanStr(&token);
        v = getElementToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf82:
    L_f82:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attack condition]", 0xf82));
            goto L_f88;
        }
    {
        values.push_back(0xe);
        ScanStr(&token);
        v = getAttackConditionToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf88:
    L_f88:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attack type]", 0xf88));
            goto L_f92;
        }
    {
        // ORIG 0x899df17：push tag 0xf 后仅 ScanStr 消费一个 token（结果丢弃）。
        values.push_back(0xf);
        ScanStr(&token);
        break;
    }
    case 0xf92:
    L_f92:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target type]", 0xf92));
            goto L_f98;
        }
    {
        values.push_back(0x10);
        ScanStr(&token);
        v = getCategoryToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf98:
    L_f98:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target grade]", 0xf98));
            goto L_f9f;
        }
    {
        values.push_back(0x11);
        ScanStr(&token);
        v = getGradeToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        break;
    }
    case 0xf9f:
    L_f9f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target check]", 0xf9f));
            goto L_fb6;
        }
    {
        // ORIG 0x899e116：push 0x12 后 while 循环 ScanTypeOrStr：每次读到
        // 非 type token（monster→2/apc→3/passiveobject→1/其它→4）push 该值
        // 并 push 一个 ScanInt；读到 type（下一标签）即退出循环。
        values.push_back(0x12);
        isType = false;
        while (!isType)
        {
            ScanTypeOrStr(isType, token, true);
            if (!isType)
            {
                k = 4;
                if (token == "monster")
                {
                    k = 2;
                }
                else if (token == "apc")
                {
                    k = 3;
                }
                else if (token == "passiveobject")
                {
                    k = 1;
                }
                values.push_back(k);
                v = ScanInt((bool*)0);
                values.push_back(v);
            }
        }
        break;
    }
    case 0xfb6:
    L_fb6:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[hit]", 0xfb6));
            goto L_fbb;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x13);
        values.push_back(v);
        break;
    }
    case 0xfbb:
    L_fbb:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attack success]", 0xfbb));
            goto L_fc0;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x14);
        values.push_back(v);
        break;
    }
    case 0xfc0:
    L_fc0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[after attack]", 0xfc0));
            goto L_fc5;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x2b);
        values.push_back(v);
        break;
    }
    case 0xfc5:
    L_fc5:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[party count]", 0xfc5));
            goto L_fd7;
        }
    {
        // ORIG 0x899e498：push 0x2c；ScanStr 比较符 >/< /=/>=/=>/<=/=<
        // → 0/1/2/3/4（未命中保持 0），再 push 一个 ScanInt。
        values.push_back(0x2c);
        ScanStr(&token);
        k = 0;
        if (token == ">")
        {
            k = 0;
        }
        else if (token == "<")
        {
            k = 1;
        }
        else if (token == "=")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0xfd7:
    L_fd7:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[use skill]", 0xfd7));
            goto L_fdf;
        }
    {
        values.push_back(0x15);
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0xfdf:
    L_fdf:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[use command]", 0xfdf));
            goto L_fe4;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x29);
        values.push_back(v);
        break;
    }
    case 0xfe4:
    L_fe4:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill frame trigger]", 0xfe4));
            goto L_ff3;
        }
    {
        // ORIG 0x899e809（tag 0x31）：ScanStr(job)→enum；ScanInt；ScanStr；
        // info 非空时该串 addString 得 key（否则 -1）；ScanInt；
        // push 0x31, job, int, key, int。
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);
        k = ScanInt((bool*)0);
        ScanStr(&token);
        int key = -1;
        if (info != 0)
        {
            key = info->addString(token);
        }
        int last = ScanInt((bool*)0);
        values.push_back(0x31);
        values.push_back(v);
        values.push_back(k);
        values.push_back(key);
        values.push_back(last);
        break;
    }
    case 0xff3:
    L_ff3:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill frame state]", 0xff3));
            goto L_1005;
        }
    {
        // ORIG 0x899e9a3（tag 0x32）：同 trigger，末尾多一个 ScanInt；
        // push 0x32, job, int, key, int, int。
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);
        k = ScanInt((bool*)0);
        ScanStr(&token);
        int key = -1;
        if (info != 0)
        {
            key = info->addString(token);
        }
        int last = ScanInt((bool*)0);
        int extra = ScanInt((bool*)0);
        values.push_back(0x32);
        values.push_back(v);
        values.push_back(k);
        values.push_back(key);
        values.push_back(last);
        values.push_back(extra);
        break;
    }
    case 0x1005:
    L_1005:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[change status in range]", 0x1005));
            goto L_1016;
        }
    {
        // ORIG 0x899eb64（tag 0x30）：ScanStr hp/mp/vp→0/1/2 push，再 push
        // 两个 ScanInt。
        values.push_back(0x30);
        ScanStr(&token);
        k = 0;
        if (token == "hp")
        {
            k = 0;
        }
        else if (token == "mp")
        {
            k = 1;
        }
        else if (token == "vp")
        {
            k = 2;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        break;
    }
    case 0x1016:
    L_1016:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[change status]", 0x1016));
            goto L_103c;
        }
    {
        // ORIG 0x899ecd3（tag 0x2f）do-while 循环：每轮 ScanStr 状态名
        // hp/mp/vp→0/1/2、"end"→跳出、其它→getChangeStatusType；随后
        // ScanStr 比较符→0..4、ScanStr "%"→bool、ScanInt——全部 push。
        values.push_back(0x2f);
        for (;;)
        {
            ScanStr(&token);
            k = 0;
            if (token == "hp")
            {
                k = 0;
            }
            else if (token == "mp")
            {
                k = 1;
            }
            else if (token == "vp")
            {
                k = 2;
            }
            else if (token == "end")
            {
                break;
            }
            else
            {
                k = getChangeStatusType(token);
            }
            values.push_back(k);
            ScanStr(&token);
            k = 0;
            if (token == ">")
            {
                k = 0;
            }
            else if (token == "<")
            {
                k = 1;
            }
            else if (token == "=")
            {
                k = 2;
            }
            else if (token == ">=" || token == "=>")
            {
                k = 3;
            }
            else if (token == "<=" || token == "=<")
            {
                k = 4;
            }
            values.push_back(k);
            ScanStr(&token);
            values.push_back(token == "%" ? 1 : 0);   // ORIG EOi 形
            values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        }
        break;
    }
    case 0x103c:
    L_103c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stat change]", 0x103c));
            goto L_105a;
        }
    {
        // ORIG 0x899efdf（tag 0x16）：一次 ScanStr hp/mp/vp→0/1/2 + 比较符
        // + "%"bool + ScanInt（无循环）。
        values.push_back(0x16);
        ScanStr(&token);
        k = 0;
        if (token == "hp")
        {
            k = 0;
        }
        else if (token == "mp")
        {
            k = 1;
        }
        else if (token == "vp")
        {
            k = 2;
        }
        values.push_back(k);
        ScanStr(&token);
        k = 0;
        if (token == ">")
        {
            k = 0;
        }
        else if (token == "<")
        {
            k = 1;
        }
        else if (token == "=")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        ScanStr(&token);
        values.push_back(token == "%" ? 1 : 0);   // ORIG EOi 形
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        break;
    }
    case 0x105a:
    L_105a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target stat]", 0x105a));
            goto L_1076;
        }
    {
        // ORIG 0x899f2b2（tag 0x17）：仅 hp/mp→0/1 + 比较符 + "%"bool + ScanInt。
        values.push_back(0x17);
        ScanStr(&token);
        k = 0;
        if (token == "hp")
        {
            k = 0;
        }
        else if (token == "mp")
        {
            k = 1;
        }
        values.push_back(k);
        ScanStr(&token);
        k = 0;
        if (token == ">")
        {
            k = 0;
        }
        else if (token == "<")
        {
            k = 1;
        }
        else if (token == "=")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        ScanStr(&token);
        v = token == "%" ? 1 : 0;
        values.push_back(v);
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        break;
    }
    case 0x1076:
    L_1076:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[check damage]", 0x1076));
            goto L_107d;
        }
    {
        // ORIG 0x899f55f（tag 0x18）：ScanStr "%"→bool push + ScanInt push。
        values.push_back(0x18);
        ScanStr(&token);
        v = token == "%" ? 1 : 0;
        values.push_back(v);
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        break;
    }
    case 0x107d:
    L_107d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[set my state]", 0x107d));
            goto L_1082;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x19);
        }
        break;
    }
    case 0x1082:
    L_1082:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target death]", 0x1082));
            goto L_1087;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1a);
        }
        break;
    }
    case 0x1087:
    L_1087:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target cooltime]", 0x1087));
            goto L_108c;
        }
    {
        values.push_back(0x2a);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0x108c:
    L_108c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[party death]", 0x108c));
            goto L_1091;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x27);
        }
        break;
    }
    case 0x1091:
    L_1091:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[revive]", 0x1091));
            goto L_1096;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1b);
        }
        break;
    }
    case 0x1096:
    L_1096:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[combo]", 0x1096));
            goto L_109b;
        }
    {
        values.push_back(0x1c);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0x109b:
    L_109b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[aerial]", 0x109b));
            goto L_10a0;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1d);
        }
        break;
    }
    case 0x10a0:
    L_10a0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[overkill]", 0x10a0));
            goto L_10a5;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1e);
        }
        break;
    }
    case 0x10a5:
    L_10a5:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[disable pvp]", 0x10a5));
            goto L_10aa;
        }
    {
        if (ScanInt((bool*)0) == 1)
        {
            values.push_back(0x1f);
        }
        break;
    }
    case 0x10aa:
    L_10aa:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[time]", 0x10aa));
            goto L_10bc;
        }
    {
        // ORIG 0x899fbdf：三次 ScanInt，第二个仅保留「!=0」bool：
        // push 0x20, int1, (int2!=0), int3。
        int t1 = ScanInt((bool*)0);
        int t2 = ScanInt((bool*)0);
        int t3 = ScanInt((bool*)0);
        values.push_back(0x20);
        values.push_back(t1);
        values.push_back(t2 != 0 ? 1 : 0);
        values.push_back(t3);
        break;
    }
    case 0x10bc:
    L_10bc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill]", 0x10bc));
            goto L_10cc;
        }
    {
        // ORIG 0x899fce4（tag 0x21）：while ScanTypeOrStr 循环：每个非 type
        // token 经 getCharacterJobToEnumValue push，随后 push 一个 ScanInt；
        // 读到 type（下一标签）退出。
        values.push_back(0x21);
        isType = false;
        while (!isType)
        {
            ScanTypeOrStr(isType, token, true);
            if (!isType)
            {
                v = getCharacterJobToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
                values.push_back(v);
                v = ScanInt((bool*)0);
                values.push_back(v);
            }
        }
        break;
    }
    case 0x10cc:
    L_10cc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attacker]", 0x10cc));
            goto L_10d4;
        }
    {
        values.push_back(0x22);
        ScanStr(&token);
        v = getObjectTypeToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0x10d4:
    L_10d4:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[event use skill]", 0x10d4));
            goto L_10dc;
        }
    {
        values.push_back(0x23);
        ScanStr(&token);
        v = getCharacterJobToEnumValue(token);   // ORIG RKi 具名物化（@0x899d585 等 17 位点形）
        values.push_back(v);
        v = ScanInt((bool*)0);
        values.push_back(v);
        break;
    }
    case 0x10dc:
    L_10dc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[event hit]", 0x10dc));
            goto L_10e1;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x24);
        values.push_back(v);
        break;
    }
    case 0x10e1:
    L_10e1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[event attack success]", 0x10e1));
            goto L_10e6;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x25);
        values.push_back(v);
        break;
    }
    case 0x10e6:
    L_10e6:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[module]", 0x10e6));
            goto L_10f1;
        }
    {
        // ORIG 0x899a011f：push 0x26 后 while (scanModuleType() != 0x11)
        // 循环 push 每个返回值；0x11 为模块流结束标记。
        values.push_back(0x26);
        while ((v = scanModuleType()) != 0x11)
        {
            values.push_back(v);
        }
        break;
    }
    case 0x10f1:
    L_10f1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[cooltime group]", 0x10f1));
            goto L_10fb;
        }
    {
        v = ScanInt((bool*)0);
        values.push_back(0x2d);
        values.push_back(v);
        break;
    }
    case 0x10fb:
    L_10fb:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attack success effect animation]", 0x10fb));
            goto L_110c;
        }
    {
        // ORIG 0x899a0276：构造一个临时 EffectAnimationInfo（EAI ctor 1 + dtor
        // 2：正常出口 + ScanStr 抛出时的清理路径），随后 count = ScanInt；循环
        // count 次各消费 ScanStr + ScanInt + ScanInt 写入该对象后随作用域销毁，
        // 不 push 任何值。
        // [R7a05 2026-09-12 ④ 真语义修复] ORIG 写入目标是**该临时对象自身**，
        //   不是函数局部 token：0x089a02fc `lea -0x748(%ebp),%eax; add $0x8,%eax;
        //   mov %eax,(%esp); call ScanStr(std::string*)`（对象基址 -0x748，
        //   +8 = m_str）；0x089a0319 `mov %eax,-0x748(%ebp)`（eai.m_a）、
        //   0x089a032b `mov %eax,-0x744(%ebp)`（eai.m_b）。
        //   旧形 `ScanStr(&token); ScanInt(0); ScanInt(0);` 既污染 token，
        //   又漏掉两处成员写入 ⇒ 已按 ORIG 逐点改回。
        EffectAnimationInfo eai;
        int count = ScanInt((bool*)0);
        for (int i = 0; i < count; ++i)
        {
            ScanStr(&eai.m_str);
            eai.m_a = ScanInt((bool*)0);
            eai.m_b = ScanInt((bool*)0);
        }
        break;
    }
    case 0x110c:
    L_110c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[dungeon check]", 0x110c));
            goto L_112c;
        }
    {
        // ORIG 0x899a0372（tag 0x2e）：push 0x2e 后 while 循环
        // ScanTypeOrInt：读到非 type token 时 push 其整数值；
        // 读到 type（下一标签）退出。
        values.push_back(0x2e);
        isType = false;
        isInt = false;
        while (!isType)
        {
            v = ScanTypeOrInt(isType, token, isInt, true);
            if (!isType)
            {
                values.push_back(v);
            }
        }
        break;
    }
    case 0x112c:
    L_112c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill cooltime]", 0x112c));
            goto L_113e;
        }
    {
        // ORIG 0x89a04c7（tag 0x33）：push 0x33；ScanStr 比较符
        // （=→0 >→1 <→2 >=/=>→3 <=/=<→4）push；push 一个 ScanInt。
        values.push_back(0x33);
        ScanStr(&token);
        k = 0;
        if (token == "=")
        {
            k = 0;
        }
        else if (token == ">")
        {
            k = 1;
        }
        else if (token == "<")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        break;
    }
    case 0x113e:
    L_113e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[slot option]", 0x113e));
            goto L_116b;
        }
    {
        // ORIG 0x89a06ae（tag 0x34）：ScanInt 映射 1→0/2→2/3→0xc/0→3/其它→0x1c
        // push；push 两个 ScanInt；ScanStr 比较符（=→0 >→1 <→2 >=/=>→3
        // <=/=<→4）push；push ScanInt。
        values.push_back(0x34);
        int m = ScanInt((bool*)0);
        k = 0x1c;
        if (m == 0)
        {
            k = 3;
        }
        else if (m == 1)
        {
            k = 0;
        }
        else if (m == 2)
        {
            k = 2;
        }
        else if (m == 3)
        {
            k = 0xc;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        ScanStr(&token);
        k = 0;
        if (token == "=")
        {
            k = 0;
        }
        else if (token == ">")
        {
            k = 1;
        }
        else if (token == "<")
        {
            k = 2;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 3;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 4;
        }
        values.push_back(k);
        values.push_back(ScanInt((bool*)0));   // ORIG EOi 直接形（叶区间实证）
        break;
    }
    case 0x116b:
    L_116b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[keep my state]", 0x116b));
            goto L_1176;
        }
    {
        // ORIG 0x89a0952（tag 0x35）：push 0x35；ScanStr→getStateToEnumValue
        // 得 v；ScanInt（int* 出参）得 ok；依次 push v、ok。
        values.push_back(0x35);
        ScanStr(&token);
        v = getStateToEnumValue(token);
        int ok = 0;
        ScanInt(&ok);
        values.push_back(v);
        values.push_back(ok);
        break;
    }
    case 0x1176:
    L_1176:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[is stat]", 0x1176));
            goto L_11a1;
        }
    {
        std::string tok;   // ORIG 0x89a0a3a：局部串槽 -0x770（C1+2×D1，非共享 token）
        // ORIG 0x899a09da（tag 0x36）：全链必须成功，任一步失败立即 return：
        // getTargetTag(t) + ScanInt(tag) + getStatusType(st) + ScanStr 比较符
        // （<→1/<=/=<→2/=→3/>=/=>→4/>→5/其它失败）+ ScanInt(val)；
        // 成功 push 0x36, t, tag, st, op, val。
        int t = 0;
        if (!getTargetTag(&t))
        {
            return;
        }
        int tag = 0;
        if (!ScanInt(&tag))
        {
            return;
        }
        int st = 0;
        if (!getStatusType(&st))
        {
            return;
        }
        if (!ScanStr(&tok))
        {
            return;
        }
        k = 0;
        if (tok == "<")
        {
            k = 1;
        }
        else if (token == "<=" || token == "=<")
        {
            k = 2;
        }
        else if (token == "=")
        {
            k = 3;
        }
        else if (token == ">=" || token == "=>")
        {
            k = 4;
        }
        else if (token == ">")
        {
            k = 5;
        }
        else
        {
            return;
        }
        int val = 0;
        if (!ScanInt(&val))
        {
            return;
        }
        values.push_back(0x36);
        values.push_back(t);
        values.push_back(tag);
        values.push_back(st);
        values.push_back(k);
        values.push_back(val);
        break;
    }
    case 0x11a1:
    L_11a1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[equipment upgrade]", 0x11a1));
            goto L_11dc;
        }
    {
        std::string tok;   // ORIG 0x89a0d4d：局部串槽 -0x77c（while 循环消费）
        // ORIG 0x899a0ced（tag 0x37）：push 0x37 后 while ScanStr 循环每条
        // 升级规格：第一词 upgrade→0/amplify→1/separate upgrade→2（其它即
        // 失败返回）；第二词比较符 <→1/<=/=<→2/=→3/>=/=>→4/>→5（其它失败
        // 返回）；随后两个 ScanInt 必须成功；push kind, op, int, int。
        // tok 耗尽（ScanStr 失败）为正常结束。
        values.push_back(0x37);
        while (ScanStr(&tok))
        {
            k = 0;
            if (tok == "upgrade")
            {
                k = 0;
            }
            else if (tok == "amplify")
            {
                k = 1;
            }
            else if (tok == "separate upgrade")
            {
                k = 2;
            }
            else
            {
                return;
            }
            if (!ScanStr(&tok))
            {
                return;
            }
            int op = 0;
            if (tok == "<")
            {
                op = 1;
            }
            else if (tok == "<=" || tok == "=<")
            {
                op = 2;
            }
            else if (tok == "=")
            {
                op = 3;
            }
            else if (tok == ">=" || tok == "=>")
            {
                op = 4;
            }
            else if (tok == ">")
            {
                op = 5;
            }
            else
            {
                return;
            }
            int a2 = 0;
            int b2 = 0;
            if (!ScanInt(&a2) || !ScanInt(&b2))
            {
                return;
            }
            values.push_back(k);
            values.push_back(op);
            values.push_back(a2);
            values.push_back(b2);
        }
        break;
    }
    case 0x11dc:
    L_11dc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[remove equip]", 0x11dc));
            goto L_11df;
        }
    {
        values.push_back(0x38);
        break;
    }
    case 0x11df:
    L_11df:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[is awakening state]", 0x11df));
            goto L_11e4;
        }
    {
        values.push_back(0x39);
        break;
    }
    case 0x11e4:
    L_11e4:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[timing]", 0x11e4));
            goto L_chain_end;
        }
    {
        // ORIG 0x899a1128（tag 0x3a）：push 0x3a；ScanStr 后
        // getEquipmentEventTiming 转换结果 push。
        values.push_back(0x3a);
        ScanStr(&token);
        values.push_back(getEquipmentEventTiming(token));
        break;
    }
    default:
        break;
    }
 L_chain_end: ;
    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
        goto relaunch;
    }
}