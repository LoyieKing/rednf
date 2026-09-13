// ============================================================================
// readAction —— ORIG 0x89a13e5（0x602e 字节）逐分支转写。
// param 映射：line=param_1（仅 [target] 失败日志用）、info=param_2（字段写入）、
// out=param_3（关键字，dispatch 依据）、values=param_4（float 输出栈）、idx=param_5。
// 每个关键字 push 一组 float 并消费对应 token（ScanInt/ScanStr/ScanFloat/
// ScanTypeOrInt/ScanTypeOrStr/ScanType）；任一扫描失败返回 false（0），
// 成功返回 true（1）。ORIG 的 __lookup 静态 map + 惰性初始化仅为编译期
// 优化，行为等价于按关键字字符串 if/else；此处省略惰性初始化直接分派。
// ============================================================================
// [b28d-solo-EquipmentParameterInfo] N：P165 循环抽象 vs ORIG 物理展开 + N82 全帧槽号；
// p167 MISSING ScanFloat×2 / push_backERKf×4（我方右值实参选 EOf，重载错位）+
// EXTRA _Unwind_Resume×2/string D1×2/ScanInt/ScanTypeOrInt/size 等。5275 vs 5339。
bool readAction(const std::string& line, EquipmentParameterInfo* info,
                std::string& out, std::vector<float>& values, int idx)
{
    // [ORIG 结构还原] 0x89a13e5：函数级静态查表 + Duff 式惰性注册链。
    // __inited==0 时不查表，id 强制 0 进 switch；每 case 头一个
    // `if (!__inited) { insert(own tag); goto next-case-head }` 块，链式逐 tag 注册，
    // 末位 0x16d3 注册后跳链尾共享块（ORIG 0x5fc7）leave+置位并跳回重查；
    // 建表完成后 guard 短路，解析 body 尾走共享尾返回 true（ORIG break 形）。
    std::string token;
    int iv = -1;
    float fv;
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    (void)idx;
    int id;
 relaunch:
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(out.c_str());
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
    // [b54FIX · 建表轮入口 `case 0:` 缺失 ⇒ 自旋锁永久锁死]
    // ORIG 089a1649/089a165f：分派链 `cmp $0x1248,%eax; je 0x89a199e` 之后紧跟
    //   `test %eax,%eax; je 0x89a199e` ⇒ **id==0 与 id==0x1248 同一目标**，即 ORIG
    //   源码首例写作 `case 0: case 0x1248:`（与 readCondtion.inc:49、
    //   importEquipmentParameterInfo.inc:145 同形，那两处本就带 `case 0:`）。
    // 本方旧形漏写 `case 0:` ⇒ `__inited==0` 的建表轮 `id=0` 落进本文件末位
    //   `default: return false;`，**既不逐例 insert，也走不到 `L_chain_end` 的
    //   `__init_lock.leave(); __inited = true;`** ⇒ 自旋锁永久保持锁定、`__inited`
    //   永远为 0；此后每次 readAction 都在 `__init_lock.enter()` 内空转。
    // 实机 A/B 实证：`character/common/jacket/cloth/tuxedo_phantom.equ` 的 `[target]`
    //   分派后（Frida /data/ab_eq.js：ScanType ret=1 @readAction）再无任何 getToken
    //   或 Scan*；/tmp/ab_fn.js 钩该 TU 内 1247 个符号，计数在 3s 处冻结、进程态恒
    //   为 R、VmRSS 恒 351872 kB（非内存增长）⇒ 自旋锁死锁。
    case 0:
    case 0x1248:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[consume item]", 0x1248));
            goto L_1257;
        }
    {
        values.push_back(0.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        goto L_chain_end;
    }
    // 0x1257 [target]
    case 0x1257:
    L_1257:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[target]", 0x1257));
            goto L_126a;
        }
    {
        int t = 0;
        if (!getTargetTag(&t)) return false;
        if (!ScanInt(&iv))
        {
            LogManager::logFormat(0, "../RDARScriptItem.cpp", __FUNCTION__, 0x125f,
                "[SCRIPT] invalid [TARGET] tag arguments. [%s]", line.c_str());
            iv = -1;
        }
        values.push_back(1.0f);
        values.push_back((float)t);
        values.push_back((float)iv);
        goto L_chain_end;
    }
    // 0x126a [duration]
    case 0x126a:
    L_126a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[duration]", 0x126a));
            goto L_127b;
        }
    {
        values.push_back(2.0f);
        if (!scanScriptValue(values)) return false;
        goto L_chain_end;
    }
    // 0x127b [equipment duration]
    case 0x127b:
    L_127b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[equipment duration]", 0x127b));
            goto L_128c;
        }
    {
        values.push_back(3.0f);
        if (!scanScriptValue(values)) return false;
        goto L_chain_end;
    }
    // 0x128c [skill duration]
    case 0x128c:
    L_128c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill duration]", 0x128c));
            goto L_12a0;
        }
    {
        values.push_back(52.0f);
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        if (!ScanStr(&token)) return false;
        values.push_back((float)(unsigned int)getSkillDataExpansionToEnumValue(token));
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        goto L_chain_end;
    }
    // 0x12a0 [reduce duration at pvp type module]
    case 0x12a0:
    L_12a0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce duration at pvp type module]", 0x12a0));
            goto L_12ac;
        }
    {
        values.push_back(26.0f);
        values.push_back(1.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12ac [reduce duration at pvp module]
    case 0x12ac:
    L_12ac:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce duration at pvp module]", 0x12ac));
            goto L_12b8;
        }
    {
        values.push_back(26.0f);
        values.push_back(2.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12b8 [reduce duration to human armor]
    case 0x12b8:
    L_12b8:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce duration to human armor]", 0x12b8));
            goto L_12c4;
        }
    {
        values.push_back(26.0f);
        values.push_back(3.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12c4 [reduce duration to human armor at pvp type module]
    case 0x12c4:
    L_12c4:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce duration to human armor at pvp type module]", 0x12c4));
            goto L_12d0;
        }
    {
        values.push_back(26.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12d0 [reduce duration to human armor at pvp module]
    case 0x12d0:
    L_12d0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce duration to human armor at pvp module]", 0x12d0));
            goto L_12dc;
        }
    {
        values.push_back(26.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12dc [delay]
    case 0x12dc:
    L_12dc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[delay]", 0x12dc));
            goto L_12e7;
        }
    {
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12e7 [probability]
    case 0x12e7:
    L_12e7:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[probability]", 0x12e7));
            goto L_12f2;
        }
    {
        values.push_back(5.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12f2 [reduce probability at pvp type module]
    case 0x12f2:
    L_12f2:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce probability at pvp type module]", 0x12f2));
            goto L_12fe;
        }
    {
        values.push_back(27.0f);
        values.push_back(1.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x12fe [reduce probability at pvp module]
    case 0x12fe:
    L_12fe:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce probability at pvp module]", 0x12fe));
            goto L_130a;
        }
    {
        values.push_back(27.0f);
        values.push_back(2.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x130a [reduce probability to human armor]
    case 0x130a:
    L_130a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce probability to human armor]", 0x130a));
            goto L_1316;
        }
    {
        values.push_back(27.0f);
        values.push_back(0.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1316 [reduce probability to human armor at pvp type module]
    case 0x1316:
    L_1316:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce probability to human armor at pvp type module]", 0x1316));
            goto L_1322;
        }
    {
        values.push_back(27.0f);
        values.push_back(3.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1322 [reduce probability to human armor at pvp module]
    case 0x1322:
    L_1322:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduce probability to human armor at pvp module]", 0x1322));
            goto L_132e;
        }
    {
        values.push_back(27.0f);
        values.push_back(4.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x132e [active status] —— 先压 tag/状态/数值，再扫描附加状态子键（armor break…curse）
    case 0x132e:
    L_132e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status]", 0x132e));
            goto L_136b;
        }
    {
        values.push_back(6.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getActiveStatusToEnumValue(token);
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        // 附加状态子表：ORIG 0x89a2af7 起是「首建表后查表」状态机（P165/P333）——
        // 用状态名 token 查 __lookup_0 得 sid，switch 贯穿 insert 链（各 case 头
        // `if(!__inited_0) insert` 后直落下一 case，case 0 为建表哨兵与 0x1344 共享），
        // 各链尾是共享 body；链尾 0x89a337c `if(!__inited_0){leave;__inited_0=true;
        // goto retry}` 回跳重查；default 直落链尾、不扫描参数。
 sid_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        float f = 0.0f;
        int sid;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            sid = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto sid_retry;
            }
            sid = 0;
        }
        switch (sid)
        {
        case 0:
        case 0x1344:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("blind", 0x1344));
        case 0x1345:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("slow", 0x1345));
        case 0x1346:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("haste", 0x1346));
                goto L_134f;
            }
            for (int k = 0; k <= 1; ++k)
            {
                if (!ScanFloat(&f)) return false;
                values.push_back(f);
            }
            break;
 L_134f:
        case 0x134f:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("stone", 0x134f));
        case 0x1350:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("confuse", 0x1350));
        case 0x1351:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("weakness", 0x1351));
        case 0x1352:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("armor break", 0x1352));
        case 0x1353:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("weapon break", 0x1353));
        case 0x1354:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("poison", 0x1354));
        case 0x1355:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("lightning", 0x1355));
        case 0x1356:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("bleeding", 0x1356));
                goto L_135c;
            }
            if (!ScanFloat(&f)) return false;
            values.push_back(f);
            break;
 L_135c:
        case 0x135c:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("burn", 0x135c));
        case 0x135d:
            if (!__inited)
                __lookup.insert(std::pair<const char* const, int>("bless", 0x135d));
        case 0x135e:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("curse", 0x135e));
                break;
            }
            for (int k = 0; k <= 3; ++k)
            {
                if (!ScanFloat(&f)) return false;
                values.push_back(f);
            }
            break;
        default:
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto sid_retry;
        }
        goto L_chain_end;
    }
    // 0x136b [disenchant]
    case 0x136b:
    L_136b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[disenchant]", 0x136b));
            goto L_1374;
        }
    {
        if (!ScanInt(&iv)) return false;
        if (iv == 1)
            values.push_back(7.0f);
        goto L_chain_end;
    }
    // 0x1374 [buff]
    case 0x1374:
    L_1374:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[buff]", 0x1374));
            goto L_137e;
        }
    {
        values.push_back(8.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getBuffToEnumValue(token);
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x137e [appendage]
    case 0x137e:
    L_137e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[appendage]", 0x137e));
            goto L_138a;
        }
    {
        values.push_back(33.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x138a [appendage unique]
    case 0x138a:
    L_138a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[appendage unique]", 0x138a));
            goto L_1396;
        }
    {
        values.push_back(34.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1396 [aura]
    case 0x1396:
    L_1396:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[aura]", 0x1396));
            goto L_13a1;
        }
    {
        values.push_back(35.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x13a1 [add absolute damage]
    case 0x13a1:
    L_13a1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add absolute damage]", 0x13a1));
            goto L_13c8;
        }
    {
        values.push_back(9.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x13c8 [add absolute defense]
    case 0x13c8:
    L_13c8:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add absolute defense]", 0x13c8));
            goto L_13e6;
        }
    {
        values.push_back(10.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x13e6 [add absolute defense percent]
    case 0x13e6:
    L_13e6:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add absolute defense percent]", 0x13e6));
            goto L_13fe;
        }
    {
        values.push_back(43.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanFloat(&fv)) return false;
        // ORIG 0x89a3b0f..0x89a3b2b：fv 先拷入临时槽 -0x4e0 再按右值传参
        // （`push_backEOf`）⇒ 源码是 prvalue 形 `push_back((float)fv)`。
        values.push_back((float)fv);
        goto L_chain_end;
    }
    // 0x13fe [aura active]
    case 0x13fe:
    L_13fe:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[aura active]", 0x13fe));
            goto L_1419;
        }
    {
        values.push_back(11.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)(int)getChangeStatusType(token);
        values.push_back(fv);
        if (!ScanStr(&token)) return false;
        if (!ScanFloat(&fv)) return false;
        if (!ScanInt(&iv)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        values.push_back(fv);
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1419 [apply status in range]
    case 0x1419:
    L_1419:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[apply status in range]", 0x1419));
            goto L_141e;
        }
    {
        GetApplyStatusInRange(&values);
        goto L_chain_end;
    }
    // 0x141e [stat by condition]
    case 0x141e:
    L_141e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stat by condition]", 0x141e));
            goto L_1423;
        }
    {
        GetStatByCondition(&values);
        goto L_chain_end;
    }
    // 0x1423 [stat level replace] —— ORIG 0x899a3dd3：push 40.0 后两次 ScanStr
    // 与一次 ScanInt 均无成功检查（失败时值取 0/默认继续，返回 true）。
    case 0x1423:
    L_1423:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stat level replace]", 0x1423));
            goto L_1432;
        }
    {
        values.push_back(40.0f);
        ScanStr(&token);
        fv = (float)(int)getChangeStatusType(token);
        values.push_back(fv);
        ScanStr(&token);
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1432 [stat]
    case 0x1432:
    L_1432:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stat]", 0x1432));
            goto L_1435;
        }
    {
        GetStat(&values);
        goto L_chain_end;
    }
    // 0x1435 [active status resistance]
    case 0x1435:
    L_1435:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status resistance]", 0x1435));
            goto L_144a;
        }
    {
        values.push_back(13.0f);
        if (!ScanStr(&token)) return false;
        float st = (float)getActiveStatusToEnumValue(token);
        if (!ScanInt(&iv)) return false;
        int v = iv;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(st);
        values.push_back(pct);
        fv = (float)v;
        (void)iv;  // ORIG 0x89a34xx leaf [1461]：ScanInt 结果不 push（Ghidra 丢弃）
        goto L_chain_end;
    }
    // 0x144a [element resistance]
    case 0x144a:
    L_144a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[element resistance]", 0x144a));
            goto L_1461;
        }
    {
        values.push_back(14.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getElementToEnumValue(token);
        values.push_back(fv);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        values.push_back(pct);
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1461 [damage rate]
    case 0x1461:
    L_1461:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[damage rate]", 0x1461));
            goto L_146e;
        }
    {
        values.push_back(15.0f);
        if (!ScanStr(&token)) return false;
        if (!ScanInt(&iv)) return false;
        values.push_back((float)iv);
        goto L_chain_end;
    }
    // 0x146e [active status duration]
    case 0x146e:
    L_146e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status duration]", 0x146e));
            goto L_1478;
        }
    {
        values.push_back(41.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1478 [active status buff event type]
    case 0x1478:
    L_1478:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status buff event type]", 0x1478));
            goto L_14af;
        }
    {
        values.push_back(42.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x14af [passive object] —— ORIG 0x899a44eb：push 16.0 + 组数占位后循环；
    // 每组以 ScanTypeOrInt 取首值，随后 5 个 ScanInt 与 1 个 ScanStr（字符串仅
    // 校验成功不 push），push 6 个数值并递增组数；任一步扫描失败返回 false。
    // ScanTypeOrInt 读到 type 且 token=="[/passive object]" 时写回组数占位结束。
    case 0x14af:
    L_14af:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[passive object]", 0x14af));
            goto L_14ec;
        }
    {
        std::string tok;   // ORIG 0x89a4569：body 局部串槽 -0x984
        values.push_back(16.0f);
        std::string tok2;  // ORIG 0x89a4765：body 局部串槽 -0x9a0
        int cnt = 0;
        values.push_back(0.0f);
        bool isType = false;
        bool isInt = false;
        for (;;)
        {
            int r = ScanTypeOrInt(isType, tok, isInt, true);
            if (isType && tok == "[/passive object]")
            {
                // ORIG 0x89a484d..0x89a4865：无 size() 守卫，直接 at(1)=cnt
                // （越界走 at 自身抛异常路径，P140/P171 照抄）。
                values.at(1) = (float)cnt;   // ORIG 0x89a485b：vector<float>::at
                goto L_chain_end;
            }
            fv = (float)r;
            values.push_back(fv);
            int b = 0;
            if (!ScanInt(&b)) return false;
            int c = 0;
            if (!ScanInt(&c)) return false;
            int d = 0;
            if (!ScanInt(&d)) return false;
            int e = 0;
            if (!ScanInt(&e)) return false;
            int f = 0;
            if (!ScanInt(&f)) return false;
            if (!ScanStr(&token)) return false;
            fv = (float)b;
            values.push_back(fv);
            fv = (float)c;
            values.push_back(fv);
            fv = (float)d;
            values.push_back(fv);
            fv = (float)e;
            values.push_back(fv);
            fv = (float)f;
            values.push_back(fv);
            cnt = cnt + 1;
        }
    }
    // 0x14ec [summon monster]
    case 0x14ec:
    L_14ec:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[summon monster]", 0x14ec));
            goto L_1502;
        }
    {
        values.push_back(17.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1502 [event summon monster] —— ORIG 0x899a4a0b：4×ScanInt 均无成功检查
    //（ScanInt((bool*)0)），失败亦返回 true。
    case 0x1502:
    L_1502:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[event summon monster]", 0x1502));
            goto L_150e;
        }
    {
        values.push_back(17.0f);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x150e [summon apc]
    case 0x150e:
    L_150e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[summon apc]", 0x150e));
            goto L_1523;
        }
    {
        values.push_back(18.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1523 [restore]
    case 0x1523:
    L_1523:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[restore]", 0x1523));
            goto L_153f;
        }
    {
        values.push_back(25.0f);
        if (!ScanStr(&token)) return false;
        float mp = (token == "mp") ? 1.0f : 0.0f;
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        if (!ScanInt(&iv)) return false;
        int v1 = iv;
        if (!ScanInt(&iv)) return false;
        values.push_back(mp);
        values.push_back(pct);
        fv = (float)v1;
        values.push_back(fv);
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x153f [skill]
    case 0x153f:
    L_153f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill]", 0x153f));
            goto L_1553;
        }
    {
        values.push_back(19.0f);
        // 3 个具名 float（ORIG -0x9dc/-0x9e0/-0x9e4）先物化，末尾连续 3 次 push_backERKf。
        if (!ScanStr(&token)) return false;
        float f1 = (float)getCharacterJobToEnumValue(token);
        if (!ScanInt(&iv)) return false;
        float f2 = (float)iv;
        if (!ScanInt(&iv)) return false;
        float f3 = (float)iv;
        values.push_back(f1);
        values.push_back(f2);
        values.push_back(f3);
        goto L_chain_end;
    }
    // 0x1553 [perform skill]
    case 0x1553:
    L_1553:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[perform skill]", 0x1553));
            goto L_156b;
        }
    {
        values.push_back(20.0f);
        if (!ScanStr(&token)) return false;
        // ORIG 0x89a5090..0x89a51a9：4 个具名 float 槽（-0x9e8/-0x9ec/-0x9f0/-0x9f4）
        // 先全部物化，末尾 4 次 push_backERKf；且只有 3 次 ScanInt（旧实现多 1 次）。
        float f1 = (float)getCharacterJobToEnumValue(token);
        if (!ScanInt(&iv)) return false;
        float f2 = (float)iv;
        if (!ScanInt(&iv)) return false;
        float f3 = (float)iv;
        if (!ScanInt(&iv)) return false;
        float f4 = (float)iv;
        values.push_back(f1);
        values.push_back(f2);
        values.push_back(f3);
        values.push_back(f4);
        goto L_chain_end;
    }
    // 0x156b [end skill]
    case 0x156b:
    L_156b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[end skill]", 0x156b));
            goto L_157a;
        }
    {
        values.push_back(21.0f);
        // 2 个具名 float（ORIG -0x9f8/-0x9fc）先物化，末尾连续 2 次 push_backERKf。
        if (!ScanStr(&token)) return false;
        float f1 = (float)getCharacterJobToEnumValue(token);
        if (!ScanInt(&iv)) return false;
        float f2 = (float)iv;
        values.push_back(f1);
        values.push_back(f2);
        goto L_chain_end;
    }
    // 0x157a [thorn]
    case 0x157a:
    L_157a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[thorn]", 0x157a));
            goto L_1585;
        }
    {
        values.push_back(22.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1585 [knockback]
    case 0x1585:
    L_1585:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[knockback]", 0x1585));
            goto L_158f;
        }
    {
        int en = 0;
        if (!ScanInt(&en)) return false;
        if (en != 0)
            values.push_back(23.0f);
        goto L_chain_end;
    }
    // 0x158f [reduction]
    case 0x158f:
    L_158f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[reduction]", 0x158f));
            goto L_15a0;
        }
    {
        values.push_back(24.0f);
        if (!ScanStr(&token)) return false;
        fv = (float)getEquipmentTypeToEnumValue(token);
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x15a0 [elemental weapon]
    case 0x15a0:
    L_15a0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[elemental weapon]", 0x15a0));
            goto L_15ab;
        }
    {
        values.push_back(28.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getElementToEnumValue(token));
        goto L_chain_end;
    }
    // 0x15ab [skill levelup]
    case 0x15ab:
    L_15ab:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x15ab));
            goto L_15b1;
        }
    {
        if (!GetSkillLevelUp(&values)) return false;
        goto L_chain_end;
    }
    // 0x15b1 [increase damage]
    case 0x15b1:
    L_15b1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[increase damage]", 0x15b1));
            goto L_15c2;
        }
    {
        values.push_back(30.0f);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x15c2 [increase critical damage]
    case 0x15c2:
    L_15c2:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[increase critical damage]", 0x15c2));
            goto L_15d3;
        }
    {
        values.push_back(31.0f);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x15d3 [weakness]（action 变体）—— ORIG 0x899a5928：ScanInt 无成功检查
    //（ScanInt((bool*)0) 值直接入栈，解析失败亦返回 true）。
    case 0x15d3:
    L_15d3:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[weakness]", 0x15d3));
            goto L_15dc;
        }
    {
        values.push_back(32.0f);
        fv = (float)ScanInt((bool*)0);
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x15dc [then probability] —— ORIG 0x899a59dd：仅 ScanFloat 消费一个数值
    // 校验成功即返回（不向 values push 任何内容）。
    case 0x15dc:
    L_15dc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[then probability]", 0x15dc));
            goto L_15e9;
        }
    {
        // ORIG 0x89a5a68/0x89a5a3a（0x15dc 体）：ScanFloat 到专属零初始化临时槽
        // （0x89a5a55 `mov $0x0,%eax; mov %eax,-0xa24` + lea），**不向 values push**，
        // 成功即经共享尾 return true；失败 ebx=0 返回 false。旧实现误 push。
        float fs = 0.0f;
        if (!ScanFloat(&fs)) return false;
        goto L_chain_end;
    }
    // 0x15e9 [skill data up] —— 在 info->m_skill1e8 追加一项并 importSkillDataUp
    case 0x15e9:
    L_15e9:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill data up]", 0x15e9));
            goto L_15f5;
        }
    {
        // ORIG 0x89a5b03..0x89a5b49：size() 结果存 -0x30；循环外构造
        // `std::vector<EquipmentSkillDataUp> tmp`（C1 0x89a5b16）→ **两参
        // resize(sz+1, tmp)**（0x89a5b44）→ tmp D1（0x89a5b66）→ operator[]
        // 读 -0x30 → importSkillDataUp。旧实现用单参 resize（缺 tmp C1/D1）。
        values.push_back(36.0f);
        unsigned sz = info->m_skill1e8.size();
        {
            std::vector<EquipmentSkillDataUp> tmp;
            info->m_skill1e8.resize(sz + 1, tmp);
        }
        importSkillDataUp(info->m_skill1e8[sz]);
        fv = (float)(int)sz;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x15f5 [speech on] —— 写 info->m_vec1f4
    case 0x15f5:
    L_15f5:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[speech on]", 0x15f5));
            goto L_1619;
        }
    {
        std::string t;     // ORIG 0x89a5d28：body 局部串槽 -0xa30
        values.push_back(37.0f);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        if (!ScanInt(&iv)) return false;
        fv = (float)iv;
        values.push_back(fv);
        unsigned sz = info->m_vec1f4.size();
        if (!ScanStr(&token)) return false;
        info->m_vec1f4.push_back(token);
        values.push_back((float)(int)sz);
        if (!ScanType(t, true)) return false;
        if (t != "[/speech on]") return false;
        goto L_chain_end;
    }
    // 0x1619 [replace avatar ani] —— 写 info->m_vec468
    case 0x1619:
    L_1619:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[replace avatar ani]", 0x1619));
            goto L_1635;
        }
    {
        bool isType = false;
        for (;;)
        {
            if (!ScanTypeOrStr(isType, token, true)) return false;
            if (isType && token == "[/replace avatar ani]") break;
            int idxV = -1;
            if (!ScanInt(&idxV)) return false;
            // ORIG 0x89a5ee4：std::make_pair(ENUM_EQUIPMENTTYPE&, int&) —— 两实参
            // 均为命名左值（P61/P81），首个槽为枚举类型（-0xa3c enum）。
            ENUM_EQUIPMENTTYPE et = (ENUM_EQUIPMENTTYPE)GetEquipmentTypeStringToEnum(token);
            info->m_vec468.push_back(std::make_pair(et, idxV));
        }
        if (!info->m_vec468.empty())
            values.push_back(44.0f);
        goto L_chain_end;
    }
    // 0x1635 [skill super armor]
    case 0x1635:
    L_1635:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill super armor]", 0x1635));
            goto L_1642;
        }
    {
        values.push_back(46.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        // ORIG 0x89a5fe5..0x89a6078：具名 float 槽 -0xa40（0.0f 初始化）承 ScanFloat。
        float f = 0.0f;
        if (!ScanFloat(&f)) return false;
        values.push_back(f);
        goto L_chain_end;
    }
    // 0x1642 [skill unbreakable]
    case 0x1642:
    L_1642:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill unbreakable]", 0x1642));
            goto L_164e;
        }
    {
        values.push_back(47.0f);
        if (!ScanStr(&token)) return false;
        values.push_back((float)getCharacterJobToEnumValue(token));
        // ORIG 0x89a6140..0x89a617a：具名 float 槽 -0xa44（0.0f 初始化）承 ScanFloat。
        float f = 0.0f;
        if (!ScanFloat(&f)) return false;
        values.push_back(f);
        goto L_chain_end;
    }
    // 0x164e [skill cooltime reset]
    case 0x164e:
    L_164e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill cooltime reset]", 0x164e));
            goto L_1663;
        }
    {
        values.push_back(48.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        if (!ScanStr(&token)) return false;
        float pct = (token == "%") ? 1.0f : 0.0f;
        values.push_back(pct);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x1663 [set hp]
    case 0x1663:
    L_1663:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[set hp]", 0x1663));
            goto L_166d;
        }
    {
        values.push_back(49.0f);
        if (!ScanFloat(&fv)) return false;
        values.push_back(fv);
        goto L_chain_end;
    }
    // 0x166d [stat change by stat]
    case 0x166d:
    L_166d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stat change by stat]", 0x166d));
            goto L_1698;
        }
    {
        int t1 = 0; if (!getTargetTag(&t1)) return false;
        int v1 = -1; if (!ScanInt(&v1)) return false;
        int s1 = 0; if (!getStatusType(&s1)) return false;
        int v2 = 0; if (!ScanInt(&v2)) return false;
        int s2 = 0; if (!getStatusType(&s2)) return false;
        int sg = 0; if (!getSign(&sg)) return false;
        int v3 = 0; if (!ScanInt(&v3)) return false;
        int v4 = 0; if (!ScanInt(&v4)) return false;
        values.push_back(50.0f);
        values.push_back((float)t1);
        values.push_back((float)v1);
        values.push_back((float)s1);
        values.push_back((float)v2);
        values.push_back((float)s2);
        values.push_back((float)sg);
        values.push_back((float)v3);
        values.push_back((float)v4);
        goto L_chain_end;
    }
    // 0x1698 [add stat at once]
    case 0x1698:
    L_1698:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add stat at once]", 0x1698));
            goto L_16ac;
        }
    {
        int st = 0; if (!getStatusType(&st)) return false;
        int sg = 0; if (!getSign(&sg)) return false;
        int v = 0; if (!ScanInt(&v)) return false;
        values.push_back(51.0f);
        values.push_back((float)st);
        values.push_back((float)sg);
        values.push_back((float)v);
        goto L_chain_end;
    }
    // 0x16ac [add skill summon]
    case 0x16ac:
    L_16ac:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add skill summon]", 0x16ac));
            goto L_16d3;
        }
    {
        std::string t;     // ORIG 0x89a68c2：body 局部串槽 -0xa80
        values.push_back(53.0f);
        int a = 0; if (!ScanInt(&a)) return false;
        int b = 1; if (!ScanInt(&b)) return false;
        int c = -1; if (!ScanInt(&c)) return false;
        if (!ScanType(t, true)) return false;
        values.push_back((float)a);
        values.push_back((float)b);
        values.push_back((float)c);
        goto L_chain_end;
    }
    // 0x16d3 [swap stat] —— 嵌套 hp/mp/vp 双组交换
    case 0x16d3:
    L_16d3:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[swap stat]", 0x16d3));
            goto L_chain_end;
        }
    {
        // ORIG 0x89a6b07：body 直接 ScanStr(&token)（无 body 局部串槽 -0xa90）；
        // lhs=3 初值在 ScanStr 之后、查表之前（ORIG 0x89a6b26）。
        if (!ScanStr(&token)) return false;
        int lhs = 3;
        // ORIG 0x89a6d4e 起 __lookup_1：lhs 侧 hp/mp/vp 子表 —— 「首建表后查表」状态机
        // （P165/P333）：switch 贯穿 insert 链，inited 时各 case 写 lhs=0/1/2 后跳出；
        // 链尾 0x89a6e81 `if(!__inited){leave;__inited=true;goto retry}` 回跳重查。
        int lid;
 L_lid_retry:
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (__inited)
            {
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(token.c_str());
                lid = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    goto L_lid_retry;
                }
                lid = 0;
            }
            switch (lid)
            {
            case 0:
            case 0x16da:
                if (__inited)
                {
                    lhs = 0;
                    break;
                }
                __lookup.insert(std::pair<const char* const, int>("hp", 0x16da));
            case 0x16df:
                if (__inited)
                {
                    lhs = 1;
                    break;
                }
                __lookup.insert(std::pair<const char* const, int>("mp", 0x16df));
            case 0x16e4:
                if (__inited)
                {
                    lhs = 2;
                    break;
                }
                __lookup.insert(std::pair<const char* const, int>("vp", 0x16e4));
                break;
            default:
                break;
            }
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                goto L_lid_retry;
            }
        }
        int sg1 = 0; if (!getSign(&sg1)) return false;
        int v1 = 0; if (!ScanInt(&v1)) return false;
        if (!ScanStr(&token)) return false;
        int rhs = 3;
        // ORIG 0x89a6f20 起 __lookup_2：rhs 侧 hp/mp/vp 子表（同形状态机）。
        int rid;
 L_rid_retry:
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (__inited)
            {
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(token.c_str());
                rid = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    goto L_rid_retry;
                }
                rid = 0;
            }
            switch (rid)
            {
            case 0:
            case 0x16fa:
                if (__inited)
                {
                    rhs = 0;
                    break;
                }
                __lookup.insert(std::pair<const char* const, int>("hp", 0x16fa));
            case 0x16ff:
                if (__inited)
                {
                    rhs = 1;
                    break;
                }
                __lookup.insert(std::pair<const char* const, int>("mp", 0x16ff));
            case 0x1704:
                if (__inited)
                {
                    rhs = 2;
                    break;
                }
                __lookup.insert(std::pair<const char* const, int>("vp", 0x1704));
                break;
            default:
                break;
            }
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                goto L_rid_retry;
            }
        }
        if (!ScanFloat(&fv)) return false;
        values.push_back(54.0f);
        values.push_back((float)lhs);
        values.push_back((float)rhs);
        values.push_back((float)sg1);
        values.push_back((float)v1);
        values.push_back(fv);
        goto L_chain_end;
    }
    default:
        return false;
    }
 L_chain_end: ;
    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
        goto relaunch;
    }
    return true;
}