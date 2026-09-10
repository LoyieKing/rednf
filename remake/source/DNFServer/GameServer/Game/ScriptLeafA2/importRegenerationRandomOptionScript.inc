int importRegenerationRandomOptionScript(RandomOptionItem_Regenerator& reg,
                                         const char* path)
{
    // [b28d-solo-ScriptLeafA2] 判词 = N（槽序/帧差，P28/N82 族；p167 CLEAR 47=47）。
    //   已修：①4 处 token 比较改「字面量在左」复现 operator==(char const*, string)
    //   （P196 族，原 4×EXTRA 反向）；②[choose part] 去 entry 引用缓存、循环内
    //   重写 reg.m_map0[part].m_vec8（P157/P183，补回 1×operator[]）。
    //   [本波实测] 循环头改 `for(;;){ if(!ScanType(...)) break; }` 复现 ORIG
    //   0x08a7e673 的 `xor $1; test; jne`：**实测劣化**（我方 raw 300→301、
    //   OURS-only 增 setl/je、ORIG-only jl 丢失）⇒ 已回滚。ORIG 的 jne 远跳
    //   （exit 在函数尾 0x3ba）+ 4.4 恒 `je <body>` 短跳，方向不可选择 ⇒ N。
    //   残余 = 局部槽基差（ORIG token@-0x2c/s@-0x30/ok@-0x31/part@-0x38 与我方
    //   -0x24/-0x28/-0x29/-0x30 整体浅 8，帧同 0x60 ⇒ N126 版本首槽基址常数）；
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }

    std::string token;
    std::string s;
    bool ok = false;
    int part = 0;

    while (ScanType(token, true))
    {
        // [b28d-solo-ScriptLeafA2] 4 处 token 比较按 ORIG 写成「字面量在左」
        // （p167：ORIG 调 operator==(char const*, string const&)，我方原为
        // (string const&, char const*) 反向 4 处 EXTRA/MISSING，P196 族）。
        if ("[choose part]" == token)
        {
            part = ScanInt(&ok);
            if (!ok)
            {
                return 0x1d;
            }
            ScanStr(&s);
            // ORIG 循环内重新 operator[]（p167 MISSING 1×operator[]；P157/P183
            // 容器成员禁缓存引用），故不设 entry 引用。
            reg.m_map0[part].m_name4 = s;
            while (ScanStr(&s))
            {
                reg.m_map0[part].m_vec8.push_back(s);
            }
        }
        else if ("[adjust quantity ratio]" == token)
        {
            part = ScanInt(&ok);            // ORIG 不校验 ok
            ScanStr(&s);
            reg.m_map0[part].m_name4 = s;
            for (int i = 0; i < 3; ++i)
            {
                reg.m_vec30.push_back(ScanInt(&ok));
            }
        }
        else if ("[adjust value ratio]" == token)
        {
            part = ScanInt(&ok);            // ORIG 不校验 ok
            ScanStr(&s);
            reg.m_map0[part].m_name4 = s;
            _str_option_value_ratio ratio(0.0f, 0.0f, 0);  // ORIG 无默认 ctor 符号
            ratio.m_scaleRange0 = 0.0f;
            ratio.m_scaleBase4 = 0.0f;
            ratio.m_weight8 = 0;
            while (ScanFloat(&ratio.m_scaleRange0))
            {
                ScanFloat(&ratio.m_scaleBase4);
                ratio.m_weight8 = static_cast<unsigned int>(
                    ScanInt(reinterpret_cast<int*>(&ratio.m_weight8)));
                reg.m_vec3c.push_back(ratio);
                reg.m_field48 += ratio.m_weight8;
            }
        }
        else if ("[regeneration price]" == token)
        {
            int limit = ScanInt(&ok);       // ORIG 不校验 ok
            for (;;)
            {
                int group;
                if (!ScanInt(&group))
                {
                    break;
                }
                if (reg.m_map18.find(group) != reg.m_map18.end())
                {
                    printf("random option item script error %u, group : %d\n",
                           0x5c, group);
                    return 0x5d;
                }
                std::map<int, _str_cost>& costs = reg.m_map18[group];
                int key;
                do
                {
                    ScanInt(&key);          // 三连 ScanInt(int*)，均不校验（ORIG 同）
                    int a;
                    int b;
                    ScanInt(&a);
                    ScanInt(&b);
                    _str_cost cost(a, b);
                    costs[key] = cost;
                } while (key < limit);
            }
        }
    }
    return 0;
