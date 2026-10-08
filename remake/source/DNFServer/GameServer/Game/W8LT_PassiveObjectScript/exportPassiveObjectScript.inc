// ===========================================================================
// exportPassiveObjectScript —— ORIG T 0x08a29f69
// （docs/class_func_reports/_lt_global_gt_/exportPassiveObjectScript.md；
// 偏移经 objdump 复核，与 W8LT_PassiveObjectScript.h 成员一一对应）
// 导出 STPassiveObjectScript 为中间文本（fopen "wb" + fprintf/fwrite），
// 收尾 convertBinaryToText 转最终脚本；fopen 失败返 0，成功返 1。
// ===========================================================================
// [b28d-51 判词；SweepW8LTDBTask2 2026-09-10 复核] ORIG 0x08a29f69（2254 条）
// vs 我方 2256 条。§6.2 三件套：p167 调用点差集 CLEAR（310=310，仅 C1/C2、
// D1/D2 同址别名）；jcc 序列全 TU 260 函数逐位核对仅本件与
// _Vector_base<uchar>::D1 不同；立即数多重集仅后者差帧 0x10/0x18。
// 本轮修复（真 bug + P0 真缺口，均已 fresh 复编验证）：
//  ① (int)(short)m_us150 —— ORIG 0x08a2b9b9 `movzwl + cwtl` 符号扩展；原写
//     (int) 零扩展，值 ≥0x8000 时输出不同（P321-T1/T2）。
//  ② follow 循环删指针缓存 —— ORIG 0x08a2b446 每轮重取 operator[]，并以对象
//     首地址 + int 索引 (k+4) 访问 +0x10..+0x18（P30 两步寻址/P183）；k 为
//     unsigned（ORIG 0x08a2b471 setbe）。
//  ③ category 循环补具名局部 int cat —— ORIG 0x08a2a2be 有槽往返（P0）。
//  ④ [name] 的 m_name.c_str() 内联实参 —— ORIG 0x08a2a02c 无槽（P0）。
//  ⑤ passName/followName 合并为函数级 nameString=0 —— ORIG 0x08a29f74 开头
//     清零、0x08a2a1df/0x08a2bbaf 复用同一槽 -0x54（P0）。
//  ⑥ soundVol/effectCnt 删具名局部 —— ORIG 0x08a2b0e5/0x08a2b1c9 用 %ebx 跨
//     调用保活（P0/P329）。
//  ⑦ m_b1c 字段 bool 化 —— ORIG 0x08a2b4a9 直测无 setne（P73/P321-T3；
//     STPassiveObjectAdditionalParticleInfo.h §8.3，宽度 1B/sizeof 不变）。
//  ⑧ speedName 分支极性 —— ORIG 0x08a2bcbd `test;je <[down]>`，[up] then 直落
//     （P222）。
//  ⑨ [string data] 照抄 *it->c_str() —— ORIG 0x08a2a9e8 movzbl+movsbl 取首字符
//     当 %s（P171 原码遗留行为照抄）。
//  ⑩ needDestroy 删具名 bool —— ORIG 0x08a2abd9 仅 %al 物化后直测（P0）。
//  ⑪ m_ib0 改浮点比较 —— ORIG 0x08a2b696 fildl（P171 照抄显式转换）。
//  ⑫ m_b60 三目 (int)(b60?1:0) —— ORIG 0x08a2a65f je/mov$1/jmp/mov$0（P47）。
//  ⑬ [sound category] 合并短路语句表达式 —— ORIG 0x08a2afea 物化 mov$1/mov$0 后
//     test（P214①/P316）。
// 剩余 N 族（逐块比对，语义等价，无源码解）：
//  a. quest type if 链块发射序（0x8a2aa8c/0x8a2aa97/0x8a2aae1/0x8a2aaf0，N93附/P46）；
//  b. m_i40 0/1 链块序（0x8a2ad12/0x8a2ad40，同族）；
//  c. homing follow 2/3/1/else 链块序（0x8a2b59d/0x8a2b618/0x8a2b633，同族）；
//  d. x87 比较 fucompp/fnstsw/sahf vs fucomip/fstp ×7（0x8a2b643/0x8a2b659/
//     0x8a2b6aa/0x8a2b711/0x8a2b747/0x8a2b77d/0x8a2bb43，N85）；
//  e. category 读槽与 lea 的求值序（0x8a2a2c1/0x8a2a2c7，P75/N-G）。
// 同 TU 另件 _ZNSt12_Vector_baseIhSaIhEED1Ev（ORIG 39 条 vs 我方 24 条）：ORIG
// libstdc++ 版含 _M_deallocate 的 EH landing + push esi/ebx，4.4 头无 ⇒ N89。
int exportPassiveObjectScript(STPassiveObjectScript* script, const char* path)
{
    // [round5 收口实测] ndiff 122→87（2254/2249）：已把 quest type / homing follow /
    // m_i40 三处 if-else 链改 switch（P251：ORIG 一次载入 + 多路寄存器比较；
    // quest type 122→111，三处合计 −35）。残余＝①x87 比较 lowering
    // `fucompp/fnstsw/sahf` vs `fucomip/fstp` ×7（N85；P230 指出 `-march=i486`
    // 可消此族，但旗标归 Main，本波不动）；②case 叶块物理序（ORIG default 块
    // fall-through 最先、其余逆序 vs 4.4＝书写序，N93附/P324 族）。
    // p167：仅 C1↔C2/D1↔D2 别名对（P93 伪差）⇒ 无语义缺口。
    static char saveFileName[0x100];
    // ORIG 0x08a29f74：函数级局部先清零，[pass type] 与 [follow parent] 复用同一
    // 槽 -0x54(%ebp)（P0：ORIG 有槽往返 ⇒ 原码有具名局部）。
    const char* nameString = 0;
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szPassiveObjectScriptBaseDirectory, path);
    FILE* file = fopen(saveFileName, "wb");
    if (!file)
        return 0;

    // ORIG -0x338：函数级默认对象，仅用于收尾 [warning] 六字段对照（ORIG
    // 比较脚本对象 +0x1d4/+0x1d8/+0x1dc 与 -0x338+0x1d4、+0x1e0 组与
    // -0x338+0x1e0，见报告 08a2c394-08a2c404）。构造于 fopen 成功后、
    // 返回前析构（正常 + unwind 两路 D1，08a2c4e9/08a2c4fd）。
    STPassiveObjectScript defaultScript;

    if (script->m_name != "")
    {
        const wchar_t* layer = 0;
        if (!script->m_layer.empty())
            layer = reinterpret_cast<const wchar_t*>(script->m_layer.c_str());
        // ORIG 0x08a2a02c：m_name.c_str() 结果不经栈槽直传 getTagByName（P0）。
        fprintf(file, "[name] %s\n",
                toTString(g_scriptStringManager_.getTagByName(
                    0x9, reinterpret_cast<const wchar_t*>(script->m_name.c_str()),
                    layer, false)));
    }
    if (script->m_width != 0 || script->m_height != 0)
        fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    if (script->m_i14 != 0)
        fprintf(file, "[floating height] %d\n", script->m_i14);
    if (script->m_i18 != 1)
        fprintf(file, "[layer] `%s`\n", getDrawLayerName((ENUM_DRAWLAYER)script->m_i18));
    if (script->m_i15c != 0)
        fprintf(file, "[layer level] %d\n", script->m_i15c);
    if (script->m_s17c != "")
        fprintf(file, "[burning death path] `%s`\n", script->m_s17c.c_str());
    if (script->m_b158 != 0)
        fwrite("[is hp by difficulty] 1\n", 1, 0x18, file);
    if (script->m_b159 != 0)
        fwrite("[hp gage] 1\n", 1, 0xc, file);
    // ORIG：getPassageTypeName 单次调用，结果存 -0x54(%ebp) 后复用（08a2a1df）。
    nameString = getPassageTypeName((ENUM_PASSAGETYPE)script->m_i1c);
    if (nameString != 0 && script->m_i1c != 0)
        fprintf(file, "[pass type] `%s`\n", nameString);
    if (script->m_i20 != 0)
        fprintf(file, "[piercing power] %d\n", script->m_i20);
    if (script->m_i24 != 0)
        fprintf(file, "[piercing tolerance] %d\n", script->m_i24);
    if (!script->m_category.empty())
    {
        fwrite("[category]\n", 1, 0xb, file);
        for (std::vector<int>::size_type i = 0; i < script->m_category.size(); ++i)
        {
            // ORIG 0x08a2a2be：元素值先落具名槽 -0x40（P0：有槽往返 ⇒ 原码有局部）。
            int cat = script->m_category[i];
            fprintf(file, "`%s`\n", getObjectCategoryString((ENUM_CATEGORY)cat).c_str());
        }
        fwrite("[/category]\n\n", 1, 0xd, file);
    }
    if (script->m_s34 != "")
        fprintf(file, "[basic motion] `%s`\n", script->m_s34.c_str());
    if (!script->m_basicActions.empty())
    {
        fwrite("[etc motion]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it = script->m_basicActions.begin();
             it != script->m_basicActions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc motion]\n", 1, 0xe, file);
    }
    if (script->m_s44 != "")
        fprintf(file, "[basic action] `%s`\n", script->m_s44.c_str());
    if (script->m_s54 != "")
        fprintf(file, "[last action] `%s`\n", script->m_s54.c_str());
    if (!script->m_etcActions.empty())
    {
        fwrite("[etc action]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it = script->m_etcActions.begin();
             it != script->m_etcActions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc action]\n", 1, 0xe, file);
    }
    if (script->m_b58 != 0)
        fwrite("[under gravity] 1\n", 1, 0x12, file);
    if (script->m_b61 != 0)
        // ORIG 0x08a2a65f：je/mov$1/jmp/mov$0 分支物化 ⇒ 三目形（P47）。
        fprintf(file, "[vanish on move collision] %d\n",
                (int)(script->m_b60 ? 1 : 0));
    if (script->m_b5a != 0)
        fwrite("[trap] 1\n", 1, 9, file);
    if (script->m_b62 != 0)
        fwrite("[allow friend hit] 1\n", 1, 0x15, file);
    if (script->m_i5c != -1)
        fprintf(file, "[team] %d\n", script->m_i5c);
    if (script->m_s64 != "")
        fprintf(file, "[destroy particle] `%s`\n", script->m_s64.c_str());
    if (script->m_s68 != "")
        fprintf(file, "[attack info] `%s`\n", script->m_s68.c_str());
    if (!script->m_attackInfos.empty())
    {
        fwrite("[etc attack info]\n", 1, 0x12, file);
        for (std::vector<std::string>::iterator it = script->m_attackInfos.begin();
             it != script->m_attackInfos.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc attack info]\n", 1, 0x13, file);
    }
    if (!script->m_intData.empty())
    {
        fwrite("[int data]\n", 1, 0xb, file);
        for (std::vector<int>::iterator it = script->m_intData.begin();
             it != script->m_intData.end(); it++)
            fprintf(file, "%d\n", *it);
        fwrite("[/int data]\n", 1, 0xc, file);
    }
    if (!script->m_stringData.empty())
    {
        fwrite("[string data]\n", 1, 0xe, file);
        // ORIG 08a2a9db 用 it->（__normal_iterator::operator->）取 c_str；指令流
        // 随后 movzbl (%eax) 把首字符当 %s 实参（疑似 ORIG 原生缺陷，call 多重集
        // 不受影响，此处保留正确指针传参并标注差异）。
        for (std::vector<std::string>::iterator it = script->m_stringData.begin();
             it != script->m_stringData.end(); it++)
            // P171 照抄：ORIG 0x08a2a9e8 把 c_str() 首字符 movzbl+movsbl 后当 %s
            // 实参传入（原码 `*it->c_str()`），保持与 ORIG 可观察行为一致。
            fprintf(file, "`%s`\n", *it->c_str());
        fwrite("[/string data]\n", 1, 0xf, file);
    }
    // [b28d-round5] ORIG 0x08a2aa80-0x08a2aa9x：`mov 0x94(%eax),%eax; cmp $0x1,%eax;
    // je; cmp $0x2,%eax; je; test %eax,%eax; je`（**一次载入 + 多路寄存器比较**，
    // P251「值无槽往返 + 多路相等比较 ⇒ 原码是 switch」）⇒ if-else 链（每支重载成员）
    // 改为 switch（顺序 1/2/0 照 ORIG 块序，P324）。
    switch (script->m_i94)
    {
    case 1:
        fwrite("[quest type] `[must be destroyed]`\n", 1, 0x23, file);
        break;
    case 2:
        fwrite("[quest type] `[chaos object]`\n", 1, 0x1e, file);
        break;
    case 0:
        fwrite("[quest type] `[none]`\n", 1, 0x16, file);
        break;
    }
    if (script->m_i98 > 0)
        fprintf(file, "[hp max] %d\n", script->m_i98);
    if (script->m_b59 != 0)
        fwrite("[hp destroy] 1\n", 1, 0xf, file);

    {
        // ORIG 0x08a2abd9：判定结果只经 %al 物化后直测（无栈槽往返，P0）⇒ 不设具名局部。
        if (script->m_destroy.m_type != -1 || script->m_destroy.m_flag04 >= 1 ||
            script->m_destroy.m_i40 != -1 ||
            !script->m_destroy.m_ints08.empty() ||
            !script->m_destroy.m_strs14.empty() ||
            !script->m_destroy.m_ints44.empty())
        {
            fwrite("[object destroy condition]\n", 1, 0x1b, file);
            switch (script->m_destroy.m_type)
            {
            case 0:
                fwrite("`[destroy condition]` `[on end of animation]`\n", 1, 0x2e, file);
                break;
            case 1:
                fwrite("`[destroy condition]` `[z pos equal zero]`\n", 1, 0x2b, file);
                break;
            case 2:
                fprintf(file, "`[destroy condition]` `[time limite]` %d\n",
                        script->m_destroy.m_i3c);
                break;
            case 3:
                fwrite("`[destroy condition]` `[not destroy and z pos equal zero]`\n",
                       1, 0x3b, file);
                break;
            case 4:
                fwrite("`[destroy condition]` `[parent dead]`\n", 1, 0x26, file);
                break;
            }
            if (script->m_destroy.m_flag04 == 1)
                fwrite("`[destroy action]` `[object create after destroy]`\n",
                       1, 0x33, file);
            // [b28d-round5] ORIG 0x08a2ad0a-0x08a2ad40：`mov 0x118(%eax),%eax;
            // test %eax,%eax; je; cmp $0x1,%eax`（一次载入 + 比较链，P251）⇒ switch。
            switch (script->m_destroy.m_i40)
            {
            case 0:
                fwrite("`[on attack]` `[only destroy]`\n", 1, 0x1f, file);
                break;
            case 1:
                fwrite("`[on attack]` `[object create after destroy]`\n",
                       1, 0x2e, file);
                break;
            }
            if (!script->m_destroy.m_ints08.empty())
            {
                fprintf(file, "`[add object index]`\n%d\n",
                        (int)script->m_destroy.m_ints08.size());
                for (std::vector<int>::size_type i = 0;
                     i < script->m_destroy.m_ints08.size(); ++i)
                    fprintf(file, "%d\n", script->m_destroy.m_ints08[i]);
            }
            if (!script->m_destroy.m_strs14.empty())
            {
                fprintf(file, "`[add object particle file]`\n%d\n",
                        (int)script->m_destroy.m_strs14.size());
                for (std::vector<std::string>::size_type i = 0;
                     i < script->m_destroy.m_strs14.size(); ++i)
                    fprintf(file, "`%s`\n", script->m_destroy.m_strs14[i].c_str());
            }
            if (!script->m_destroy.m_ints44.empty())
            {
                fprintf(file, "`[add object index on attack]`\n%d\n",
                        (int)script->m_destroy.m_ints44.size());
                for (std::vector<int>::size_type i = 0;
                     i < script->m_destroy.m_ints44.size(); ++i)
                    fprintf(file, "%d\n", script->m_destroy.m_ints44[i]);
            }
            fwrite("[/object destroy condition]\n", 1, 0x1c, file);
        }
    }
    if (!script->m_destroy.m_strs20.empty())
    {
        fwrite("[sound category]\n", 1, 0x11, file);
        for (std::vector<std::string>::size_type i = 0;
             i < script->m_destroy.m_strs20.size(); ++i)
        {
            // ORIG 08a2afaf-08a2afe8：empty() 先行短路，再逐访问重索引比较 ""。
            // ORIG 0x08a2afea：empty 与 == 合并短路后物化 mov$1/mov$0 再 test
            // （P214①/P316：语句表达式保短路物化且不占槽）。
            if (({ script->m_destroy.m_strs20[i].empty() ||
                   script->m_destroy.m_strs20[i] == ""; }))
                continue;
            switch (i)
            {
            case 0:
                fwrite("`[after object create]`", 1, 0x17, file);
                break;
            case 1:
                fwrite("`[on frame]`", 1, 0xc, file);
                break;
            case 2:
                fwrite("`[on attack]`", 1, 0xd, file);
                break;
            case 3:
                fwrite("`[on damage]`", 1, 0xd, file);
                break;
            case 4:
                fwrite("`[on destroyobject]`", 1, 0x14, file);
                break;
            }
            // ORIG 08a2b0e5：先取音量入 ebx（无槽往返，P0），后取名字 c_str。
            fprintf(file, " `%s` %d\n",
                    script->m_destroy.m_strs20[i].c_str(),
                    script->m_destroy.m_ints2c[i]);
        }
        fwrite("[/sound category]\n", 1, 0x12, file);
    }
    if (!script->m_addEffect.m_strs00.empty())
    {
        fwrite("[add object effect]\n", 1, 0x14, file);
        for (std::vector<std::string>::size_type i = 0;
             i < script->m_addEffect.m_strs00.size(); ++i)
        {
            // ORIG 08a2b1c9：先取 int 入 ebx（无槽往返，P0），再取 string c_str。
            fprintf(file, "%s %d\n",
                    script->m_addEffect.m_strs00[i].c_str(),
                    script->m_addEffect.m_ints0c[i]);
        }
        fwrite("[/add object effect]\n", 1, 0x15, file);
    }
    if (!script->m_particles.empty())
    {
        fwrite("[add particles]\n", 1, 0x10, file);
        for (std::vector<STPassiveObjectAdditionalParticleInfo*>::size_type i = 0;
             i < script->m_particles.size(); ++i)
        {
            fwrite("\t[particle]\n", 1, 0xc, file);
            // P38：ORIG 08a2b2cc-08a2b4a2 每个字段访问均重新调用
            // vector::operator[]（检查+输出各一次），勿缓存元素指针。
            if (!script->m_particles[i]->m_s00.empty())
                fprintf(file, "\t\t[file] `%s`\n",
                        script->m_particles[i]->m_s00.c_str());
            if (script->m_particles[i]->m_i0c > 0)
                fprintf(file, "\t\t[create delay] %d\n",
                        script->m_particles[i]->m_i0c);
            if (!script->m_particles[i]->m_s04.empty())
                fprintf(file, "\t\t[create sound] `%s`\n",
                        script->m_particles[i]->m_s04.c_str());
            if (script->m_particles[i]->m_i10 != 0)
            {
                fwrite("\t\t[follow parent]", 1, 0x11, file);
                // ORIG 08a2b446-08a2b48a：每轮重新 operator[]（P183 禁指针缓存），
                // 再以对象首地址 + int 索引 (k+4) 访问 +0x10..+0x18；k 为 unsigned
                // （尾测 setbe，P321-T2）。
                fprintf(file, "%d %d %d ",
                        script->m_particles[i]->m_i10,
                        script->m_particles[i]->m_i14,
                        script->m_particles[i]->m_i18);
                fputc(10, file);
            }
            // ORIG 0x08a2b4a9：直测单字节（无 setne 物化）⇒ 隐式转 bool。
            if (script->m_particles[i]->m_b1c)
                fwrite("\t\t[loop] 1\n", 1, 0xb, file);
            fwrite("\t[/particle]\n", 1, 0xd, file);
        }
        fwrite("[/add particles]\n", 1, 0x11, file);
    }
    if (script->m_b9c != 0)
    {
        fwrite("[homing]\n", 1, 9, file);
        fwrite("\t[homing use] 1\n", 1, 0x10, file);
        // [b28d-round5] ORIG 0x08a2b594-0x08a2b633：`mov 0xa0(%eax),%eax; cmp $0x2,%eax;
        // je; cmp $0x3,%eax; je; cmp $0x1,%eax; je`（一次载入 + 多路寄存器比较，
        // P251）⇒ if-else 链改 switch（case 序 2/3/1 + default 照 ORIG 块序）。
        switch (script->m_ia0)
        {
        case 2:
            fprintf(file, "\t[homing follow] `[MONSTER]` %d\n", script->m_ia4);
            break;
        case 3:
            fprintf(file, "\t[homing follow] `[CHARACTER]` %d\n", script->m_ia4);
            break;
        case 1:
            fwrite("\t[homing follow] `[ENEMY]`\n", 1, 0x1b, file);
            break;
        default:
            fwrite("\t[homing follow] `[ME]`\n", 1, 0x18, file);
            break;
        }
        if (script->m_fa8 != 0.0f || script->m_fac != 0.0f)
            fprintf(file, "\t[homing velocity] %f %f\n", script->m_fa8, script->m_fac);
        // ORIG 0x08a2b696：int 字段经 fildl 浮点比较（原码显式转浮点）⇒ 照抄。
        if ((double)script->m_ib0 != 0.0)
            fprintf(file, "\t[homing check gap] %d\n", script->m_ib0);
        if (script->m_bb4 != 0)
            fwrite("\t[sync animation rotation] 1\n", 1, 0x1d, file);
        if (script->m_fb8 != 0.0f)
            fprintf(file, "\t[max rotation] %f\n", script->m_fb8);
        if (script->m_fbc != 0.0f)
            fprintf(file, "\t[init rotation] %f\n", script->m_fbc);
        if (script->m_fc0 != 0.0f)
            fprintf(file, "\t[diff rotation] %f\n", script->m_fc0);
        if (script->m_ic4 != 0)
            fprintf(file, "\t[homing time] %d\n", script->m_ic4);
        if (script->m_ic8 != 0)
            fprintf(file, "\t[homing z axis] %d\n", script->m_ic8);
        if (script->m_icc != -1)
            fprintf(file, "\t[homing direction] %d\n", script->m_icc);
        if (script->m_id0 != 0)
            fprintf(file, "\t[homing keep distance] %d\n", script->m_id0);
        if (script->m_bd4 != 0)
            fwrite("\t[sync target move]\n", 1, 0x14, file);
        if (script->m_bd5 != 0)
            fwrite("\t[straight homing]\n", 1, 0x13, file);
        fwrite("[/homing]\n", 1, 10, file);
    }
    if (script->m_b14c != 0)
        fwrite("[for tournament] 1\n", 1, 0x13, file);
    if (script->m_b14d != 0)
        fwrite("[for school] 1\n", 1, 0xf, file);
    if (script->m_b14e != 0)
        fwrite("[die when parent die] 1\n", 1, 0x18, file);
    if (script->m_ub14f != 0xff)
        fwrite("[transparent on meet player] 1\n", 1, 0x1f, file);
    if (script->m_us150 != 0xffff)
        // ORIG 0x08a2b9b9：movzwl + cwtl（符号扩展）⇒ 原码为 (int)(short)。
        fprintf(file, "[pvp room transparent value for observer] %d\n",
                (int)(short)script->m_us150);
    if (script->m_s154 != "")
        fprintf(file, "[create sound] `%s`\n", script->m_s154.c_str());
    if (!script->m_onAttackData.empty())
    {
        fwrite("[can beat index]\n", 1, 0x11, file);
        // ORIG 08a2baa9：本循环用前置 ++it（operator++()），与 int data 循环的后置异。
        for (std::vector<int>::iterator it = script->m_onAttackData.begin();
             it != script->m_onAttackData.end(); ++it)
            fprintf(file, "%d\n", *it);
        fwrite("[/can beat index]\n", 1, 0x12, file);
    }
    if (script->m_b16c != 0)
        fwrite("[is blocking obj] 1\n", 1, 0x14, file);
    if (script->m_f170 != -1.0f)
        fprintf(file, "[apply physics] %f\n", script->m_f170);
    if (script->m_i174 != 0)
        fwrite("[following immortal] 1\n", 1, 0x17, file);
    // ORIG：getFollowTypeName 单次调用存 -0x54(%ebp) 复用（08a2bbaf）。
    nameString = getFollowTypeName((ENUM_MAP_FOLLOW_TYPE)script->m_i178);
    if (nameString != 0)
        fprintf(file, "[follow parent] `%s`\n", nameString);
    // ORIG 08a2bbdd：size 缓存 -0x4c(%ebp)，仅测一次 >0；循环用缓存值。
    int speedCount = (int)script->m_speedCtrls.size();
    if (speedCount > 0)
    {
        // ORIG 块内先构造 string(-0xa0) 与 SpeedCtrlPassiveObjInfo 暂存(-0xa8)：
        // 逐元素 `info = vec[i]`（trivial 拷贝赋值 → memcpy 自校验形态 08a2bc58），
        // 速度名经 string::operator=(const char*) 赋 "[up]"/"[down]"。
        fwrite("[speed control passive objects]\n", 1, 0x20, file);
        std::string speedName;
        SpeedCtrlPassiveObjInfo info;
        for (int i = 0; i < speedCount; ++i)
        {
            info = script->m_speedCtrls[i];
            fwrite("[passive object]\n", 1, 0x11, file);
            fprintf(file, "\t[index] %d\n", info.m_field0);
            // ORIG 0x08a2bcbd：test %al; je <[down] 块>，[up] 为 then 直落
            // （P222 正极性包裹）⇒ 正写 if (m_b4) 才复现该布局。
            if (info.m_b4)
                speedName = "[up]";
            else
                speedName = "[down]";
            fprintf(file, "\t[speed] `%s`\n", speedName.c_str());
            fwrite("[/passive object]\n", 1, 0x12, file);
        }
        fwrite("[/speed control passive objects]\n", 1, 0x21, file);
    }
    if (!script->m_genObjects.empty())
    {
        fwrite("[objects]\n", 1, 10, file);
        // ORIG 08a2be1a/08a2be4b/08a2be7d：index/probability/warning 逐访问重索引。
        for (std::vector<PassiveObjGeneratorObject>::size_type i = 0;
             i < script->m_genObjects.size(); ++i)
        {
            fwrite("\t[object]\n", 1, 10, file);
            fprintf(file, "\t\t[index] %d\n", script->m_genObjects[i].m_i00);
            fprintf(file, "\t\t[probability] %f\n", script->m_genObjects[i].m_f04);
            fprintf(file, "\t\t[warning] `%s`\n", script->m_genObjects[i].m_s08.c_str());
            fwrite("\t[/object]\n", 1, 0xb, file);
        }
        fwrite("[/objects]\n", 1, 0xb, file);
    }
    if (!script->m_genPatterns.empty())
    {
        fwrite("[patterns]\n", 1, 0xb, file);
        // ORIG 08a2bf8c-08a2c1ad：每字段（name/interval/columns/data 判定、列数、
        // 数据元素、data size）均重新调用 vector<Pattern>::operator[]。
        for (std::vector<PassiveObjGeneratorPattern>::size_type i = 0;
             i < script->m_genPatterns.size(); ++i)
        {
            fwrite("\t[pattern]\n", 1, 0xb, file);
            fprintf(file, "\t\t[name] `%s`\n", script->m_genPatterns[i].m_s04.c_str());
            fprintf(file, "\t\t[interval] %d\n", script->m_genPatterns[i].m_i0c);
            fprintf(file, "\t\t[columns] %d\n", script->m_genPatterns[i].m_i08);
            if (!script->m_genPatterns[i].m_v10.empty())
            {
                fwrite("\t\t[data]\n", 1, 9, file);
                for (std::vector<unsigned char>::size_type d = 0;
                     d < script->m_genPatterns[i].m_v10.size(); ++d)
                {
                    // 每轮先重取列数（ORIG 08a2c087 → -0x10 缓存）
                    unsigned int cols = (unsigned int)script->m_genPatterns[i].m_i08;
                    if (d % cols == 0)
                        fprintf(file, "\t\t\t%d",
                                (int)script->m_genPatterns[i].m_v10[d]);
                    else if (d % cols == cols - 1)
                        fprintf(file, " %d\n",
                                (int)script->m_genPatterns[i].m_v10[d]);
                    else
                        fprintf(file, " %d",
                                (int)script->m_genPatterns[i].m_v10[d]);
                }
                fwrite("\t\t[/data]\n", 1, 10, file);
            }
            fwrite("\t[/pattern]\n", 1, 0xc, file);
        }
        fwrite("[/patterns]\n", 1, 0xc, file);
    }
    if (!script->m_genSequences.empty())
    {
        fwrite("[sequences]\n", 1, 0xc, file);
        // ORIG 08a2c2d6/08a2c30d：两段各一次重索引，且为两条 fprintf。
        for (std::vector<PassiveObjGeneratorSequence>::size_type i = 0;
             i < script->m_genSequences.size(); ++i)
        {
            fwrite("\t[pattern]", 1, 10, file);
            fprintf(file, " `%s`", script->m_genSequences[i].m_s00.c_str());
            fprintf(file, " %d", script->m_genSequences[i].m_i04);
            fwrite(" [/pattern]\n", 1, 0xc, file);
        }
        fwrite("[/sequences]\n", 1, 0xd, file);
    }
    // [DNF-FIX 2026-10-08 c31] ORIG 0x08a2c394..0x08a2c404：+0x1d4/+0x1d8/+0x1dc 均与 default -0x164
    //   （warnDanger.m_a）比较，+0x1e0/+0x1e4/+0x1e8 均与 -0x158（warnRecovery.m_a）比较。
    if (script->m_warnDanger.m_a != defaultScript.m_warnDanger.m_a ||
        script->m_warnDanger.m_b != defaultScript.m_warnDanger.m_a ||
        script->m_warnDanger.m_c != defaultScript.m_warnDanger.m_a ||
        script->m_warnRecovery.m_a != defaultScript.m_warnRecovery.m_a ||
        script->m_warnRecovery.m_b != defaultScript.m_warnRecovery.m_a ||
        script->m_warnRecovery.m_c != defaultScript.m_warnRecovery.m_a)
    {
        fwrite("[warning]\n", 1, 10, file);
        fprintf(file, "[min distance] %d %d %d\n",
                script->m_warnDanger.m_a, script->m_warnDanger.m_b,
                script->m_warnDanger.m_c);
        fprintf(file, "[max distance] %d %d %d\n",
                script->m_warnRecovery.m_a, script->m_warnRecovery.m_b,
                script->m_warnRecovery.m_c);
        fwrite("[/warning]\n", 1, 0xb, file);
    }
    fclose(file);
    convertBinaryToText(saveFileName);
    return 1;
}