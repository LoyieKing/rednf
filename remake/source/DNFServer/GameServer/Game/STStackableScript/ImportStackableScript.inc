bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx)
{
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, path))
    {
        return false;
    }
    script->Clear();
    std::string line;
    std::string token;
    int vzero = 0;   // ORIG -0x144c：函数级 int 暂存槽，顶部 movl $0 初始化
    (void)vzero;
    bool ok = false; // ORIG -0x144d：顶部唯一一次 movb $0 清零

    // ORIG 0x089aeec8..0x089ba83b（10197 指令）结构（docs 报告 ImportStackableScript.md
    // 汇编 16..10217 / 反编译 C 10218..17101）：
    //   while (ScanType(line,1)) {
    //     if (line=="[name]") ScanStr(&m_strac(+0xac)); flag 置位则立即跳出循环（
    //       jmp +0xb8cd 收尾）；
    //     else if (line=="[emancipate ticket]") m_field10e=1 后 ScanInt 写 +0x394(short)，
    //       失败仅继续循环；                    [+0x394.. = m_394 ushort]
    //     else if (line=="[equipment reinforcement ticket]") m_396=1、ScanInt(+ok)→m_398、
    //       再 ScanInt→+0x39a 字节；任一 !ok → return false（mov $0,%ebx 出口）；
    //     else if (line=="[check usable itemlevel]") 两 ScanInt 写 +0x43c/+0x440，
    //       任一 !ok → return false；ok 后 m_438=1；
    //     else if (line=="[passive object in stackable]") 内层循环连读 4 int（field0..C）
    //       push 到 m_passive208(+0x208)，行首失败跳出；
    //     else if (line=="[equipment amplify reinforcement ticket]") m_39b=1、ScanInt→m_39c、
    //       ScanInt→+0x3a0 字节；任一 !ok → return false；
    //     else if (line=="[amplification random value]") m_3a1=1 后循环 {ScanInt v1(v2)；
    //       !ok 跳出；pair(v1,v2) insert m_map3a4}；
    //     else if (line=="[/amplification random value]") 空（继续循环）；
    //     else 主分发：静态 __lookup 惰性填充（[name2]..[attach type] 等 157 主标签
    //       → 下述 hash，case 顺序即 hash 升序填充链）后 find(line.c_str())，switch 分派；
    //       命中失败（无 case）→ 089ba768 桩（continue）。
    //   每行处理完毕都经公共桩 089ba768 → +0x80 回 ScanType 循环。
    // 扫描结束（ScanType false）→ +0xb8cd 收尾：if(m_field24==-1) m_field24=m_field14；
    //   idx==-1 → m_field0=FindIndexByFullScan(path) else m_field0=idx；返回 true。
    // 转写形态（本轮三步法 Step2 重构，ORIG 证据同上报告）：
    //   主表 + 4 张子表(attach/stackable-type/my-status/itemmaking)均为
    //   for(;;){ find-或-双检锁 → switch(tag){ case V: if(!__inited){单条
    //   value_type 惰性 insert} else {work; break;} } → if(__inited)break;
    //   leave;置位 } 的**逐例 guard 链**（非集中批量 insert）；case 0 与首个
    //   case 共叶、子表首 case 亦然；[enchant] 内层循环复用主表 __lookup
    //   （[/enchant]/[pvp] 作为主表条目在循环内 guard 链登记）。
    //   gcc4.4.7 对 if(!__inited){insert} else {work;break} + case 贯穿生成的
    //   块布局（check:xor/test/je → insert 块 → jmp 下叶 → work 块）与 ORIG
    //   逐条同形（探针验证，见 docs/asm_translation_patterns 新范式提报）。
    while (ScanType(line, true))
    {
        if (line == "[name]")
        {
            ScanStr(&script->m_strac);  // +0xac（ORIG +0x3c..+0x46 ScanStr(&+0xac)）
            if (flag)
            {
                break;  // ORIG 089aef9a：flag → jmp +0xb8cd 收尾
            }
        }
        else if (line == "[emancipate ticket]")
        {
            // ORIG 089aefbb..089aeff1：movb $1,0x10e；ScanInt(&ok)→(short)+0x394；
            // 其后 if(!ok) continue 的物化检查（双 landing pad +0x76/+0x7f，
            // 两向均回主循环——逐指令转写需要保留该语句）。
            script->m_field10e = 1;
            script->m_394 = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                continue;
            }
        }
        else if (line == "[equipment reinforcement ticket]")
        {
            // ORIG 089aeff6..089af080：movb $1,0x396；ScanInt→0x398(short)；
            // ScanInt→0x39a(byte)；!ok 任一 → return false。
            script->m_396 = 1;
            script->m_398 = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
            script->m_39a = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
        }
        else if (line == "[check usable itemlevel]")
        {
            // ORIG 089af081..089af107：两 ScanInt 写 +0x43c/+0x440；任一失败 return false；
            // 成功 movb $1,0x438。
            script->m_pair43c.first = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_pair43c.second = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_438 = 1;
        }
        else if (line == "[passive object in stackable]")
        {
            // ORIG 089af108..089af1ce：局部 STPassiveObjectInStackable 反复 5×ScanInt
            // （+0x00..+0x10；仅每轮首值失败即跳出），成功后 push_back 到 m_passive208
            // （call 089c6f9e = vector::push_back；+0x208 元素 sizeof 0x10）。
            // 089af126：段内局部 bool ok2 先置 true，5 次 ScanInt 全部传 &ok2。
            bool ok2 = true;
            STPassiveObjectInStackable obj;
            for (;;)
            {
                obj.m_field0 = ScanInt(&ok2);
                if (!ok2)
                {
                    break;
                }
                // ORIG 089af163..089af1ad：首值失败才结束本段；后四次
                // ScanInt 的返回值直接写入对象，不检查 ok2（实参仍是 &ok2）。
                obj.m_field4 = ScanInt(&ok2);
                obj.m_field8 = ScanInt(&ok2);
                obj.m_fieldC = ScanInt(&ok2);
                obj.m_field10 = ScanInt(&ok2);
                script->m_passive208.push_back(obj);
            }
        }
        else if (line == "[equipment amplify reinforcement ticket]")
        {
            // ORIG 089af1d3..089af25a：movb $1,0x39b；ScanInt→0x39c；
            // ScanInt→0x3a0(byte)；!ok 任一 return false。
            script->m_39b = 1;
            script->m_39c = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
            script->m_3a0 = static_cast<char>(ScanInt(&ok));
            if (!ok)
            {
                return false;
            }
        }
        else if (line == "[amplification random value]")
        {
            // ORIG 089af25b..089af34e：movb $1,0x3a1 后循环 {v1=ScanInt(!ok break);
            // v2=ScanInt(!ok break); m_map3a4.insert(pair(v1,v2))}。
            script->m_3a1 = 1;
            for (;;)
            {
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_map3a4.insert(std::make_pair(v1, v2));
            }
            // ORIG 089af339..089af34b：循环出口后的 ok 物化检查（两向均 continue）。
            if (!ok)
            {
                continue;
            }
        }
        else if (line == "[/amplification random value]")
        {
            // ORIG 089af350..089af36a：无操作，继续主循环。
        }
        else
        {
    static std::map<char const*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    int tag;
    for (;;)
    {
        if (__inited)
        {
            std::map<char const*, int, less_TCHAR>::const_iterator it = __lookup.find(line.c_str());
            tag = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            tag = 0;
        }
        switch (tag)
        {
            case 0:
            case 0x2064:  // "[name2]"


                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[name2]", 0x2064));
                }
                else
                {
    ScanStr(&script->m_strb0);
    break;
                }
            case 0x2067:  // "[explain]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain]", 0x2067));
                }
                else
                {
    ScanStr(&script->m_strb4);
    break;
                }
            case 0x206b:  // "[explain2]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain2]", 0x206b));
                }
                else
                {
    ScanStr(&script->m_strb4);
    break;
                }
            case 0x206f:  // "[flavor text]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[flavor text]", 0x206f));
                }
                else
                {
    ScanStr(&script->m_strb8);
    break;
                }
            case 0x2072:  // "[grade]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[grade]", 0x2072));
                }
                else
                {
    script->m_field4 = ScanInt((bool*)0);
    break;
                }
            case 0x2075:  // "[attach type]"
                if (!__inited)
                {
                    __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[attach type]", 0x2075));
                }
                else
                {
{
    ScanStr(&token);
    static std::map<char const*, int, less_TCHAR> __lookup_0;
    static SpinLock __init_lock_0;
    static bool __inited_0 = false;
    int sub;
    for (;;)
    {
        if (__inited_0)
        {
            std::map<char const*, int, less_TCHAR>::const_iterator it0 = __lookup_0.find(token.c_str());
            sub = (it0 != __lookup_0.end()) ? it0->second : -1;
        }
        else
        {
            __init_lock_0.enter();
            if (__inited_0)
            {
                __init_lock_0.leave();
                continue;
            }
            sub = 0;
        }
        switch (sub)
        {
            case 0:
            case 0x2079:  // "[free]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[free]", 0x2079));
                }
                else
                {
    script->m_field8 = 0x0; break;   // "[free]"
                }
            case 0x207c:  // "[trade]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade]", 0x207c));
                }
                else
                {
    script->m_field8 = 0x1; break;   // "[trade]"
                }
            case 0x207f:  // "[trade delete]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade delete]", 0x207f));
                }
                else
                {
    script->m_field8 = 0x2; break;   // "[trade delete]"
                }
            case 0x2082:  // "[sealing]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[sealing]", 0x2082));
                }
                else
                {
    script->m_field8 = 0x3; break;   // "[sealing]"
                }
            case 0x2085:  // "[account]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[account]", 0x2085));
                }
                else
                {
    script->m_field8 = 0x5; break;   // "[account]"
                }
            case 0x2089:  // "[trade limit]"
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[trade limit]", 0x2089));
                }
                else
                {
    script->m_field8 = 0x7; break;   // "[trade limit]"
                }
            default:
    break;
        }
        if (__inited_0)
        {
            break;
        }
        __init_lock_0.leave();
        __inited_0 = true;
    }
