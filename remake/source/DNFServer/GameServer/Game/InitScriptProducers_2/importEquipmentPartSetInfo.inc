// ============================================================================
// importEquipmentPartSetInfo —— ORIG T 0x089073d9（init.md:4749，
// "Etc/EquipmentPartSet.etc", ENUM_MODULE_TYPE 3）。返回 bool（真=成功）。
// 解析 [equipment part set] → g_equipmentPartSetMap_；随后逐集合
// new STEquipmentScript 并 ImportEquipmentScript(name,false,module,key)；
// 成功后把该脚本 m_paramMap678[-1]（存在时）迁移为键=pieces.size() 的条目。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N89 封顶（原 DIFF 336）ORIG
//   0x089073d9 orig/ours=374/349，p167 仅剩 `_ZNKSt17_Rb_tree_iterator<
//   pair<const int,STEquipmentPartSetData>>::ptEv` ×7（4.4 内联头内 operator->，
//   4.1 out-of-line；N89 类不可控）。本波修：①operator== 正形（去 operator!=）；
//   ②`data.m_field0 = ScanInt((bool*)0)` 去独立 key 局部；③do-while 复用
//   ScanStr 返回值（去重复调用）；④`paramMap[-1]` 替代 `def->second`（补
//   map::operator[] 调用点）；⑤P164 goto 主循环。
// [sweep2-InitScriptProducers_2] verdict=N 封顶（ndiff 467→4，gap 25→0）：
//   orig/ours=372/372。本波修：①删 `STEquipmentScript* script` 具名局部——
//   ORIG 0x08907619 new 结果经 ebx 跨 ctor 保活后直存 it->second.m_script
//   （P0/P183），后续 10 处 `call operator->; mov 0xc(%eax),%eax` 逐点重读；
//   ②删 `paramMap` 引用局部，`it->second.m_script->m_paramMap678` 全展开；
//   ③find/end 改单表达式（ORIG 0x08907741 先 end、0x08907775 后 find，P293）；
//   ④do-while 的 ok 改 `register bool`（ORIG ebx 承载、无栈槽，P231）。
//   残余 4 行 = ORIG `test %ebx,%ebx; je exit; jmp body` vs 4.4 `testb %bl; jne body`
//   （bool 寄存器宽度 + do-while 尾布局，跨版本，N 封顶）。
bool importEquipmentPartSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x168b,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3"
                              "\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4"
                              "\xd9.",
                              path);
        return false;
    }

    std::string token;
    std::string name;
    std::string pieceName;
    std::string tag;
    // ORIG 用 ebx 承载 ok（无栈槽；0x08907503 mov $0,%ebx / 0x089075a5 mov $1,%ebx /
    // 0x089075cc test %ebx,%ebx）⇒ 原码是 register 形（P231 纯 register 合法，非 asm
    // 绑定）。[LaneG 2026-09-11 实验·三形对照，判 N] ORIG 0x089075cc 是
    // `test %ebx,%ebx; je <exit>; jmp <body>`（32 位 test/直跳，无 setne 物化）。
    // 实测：①register bool ok + do-while(ok)：`test %bl; jne`（宽度差，gap 0）；
    // ②register int ok + do-while(ok)：4.4 恒发 `test %ebx; setne %al; test %al;
    // jne`（+2 setne 物化，gap −2）；③int + do-while(ok != 0)：同②逐值相同
    // （4.4 对 do-while 尾 int 条件恒 bool 物化，N122/P168a 族）；④while(true)+
    // 尾 `if(!ok) break`：反劣（our 377，多 5 条 ok=1→test→jne 折叠块）。
    // ⇒ 本轮再试⑤`while(true)+if(ok == 0)break;`（显式 int 标量比较）：
    // 实测 our=377（+5 条 `mov $1,%ebx; test; jne; mov $0,%ebx; jmp`，与④同形）
    // ⇒ 亦反劣，回退①。五形全部试毕，①（bool+do-while）为最短形（our=374=ORIG
    // 行数、唯一残差 = test 宽度 + jcc 方向）⇒ 维持判 N（跨版本 do-while 尾
    // bool lowering）。
    register bool ok;

    g_equipmentPartSetMap_.clear();

    // P164：主循环 goto 形复位头部条件块（ORIG 0x08907477）；ScanType 失败落
    // scan_done（直接进入下方 for 循环，非 return）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        // ORIG 0x0890749d 正向 `if (token == X)` 形（用 operator==，无 operator!=）。
        if (token == "[equipment part set]")
        {
            STEquipmentPartSetData data;
            // ORIG 直接 `data+0 = ScanInt(0)`（无独立 key 局部；键取 data.m_field0）。
            data.m_field0 = ScanInt((bool*)0);
            ScanStr(&name);
            data.m_name4 = name;

            // [LaneG 2026-09-11 实验⑤反劣已回退] 回到①do-while(bool)形。
            do
            {
                STEquipmentPartSetPiece piece;           // ctor 三 int=-1
                // ORIG 0x089074fc 正向 `if (!ScanStr)`：失败臂 mov $0,%ebx、成功臂尾
                // mov $1,%ebx（两臂各写 ok，非 `ok = ScanStr(...)` 单赋值）。
                if (!ScanStr(&pieceName))
                {
                    ok = false;
                }
                else
                {
                    piece.m_name = pieceName;
                    ScanStr(&tag);
                    piece.m_field4 = getSameStringIndex(tag.c_str(),
                                                        characterEquipmentTypeTags,
                                                        0x1a);
                    piece.m_field8 = ScanInt((bool*)0);
                    piece.m_fieldc = ScanInt((bool*)0);
                    data.m_pieces.push_back(piece);

                    g_equipmentPartSetMap_[data.m_field0] = data;
                    ok = true;
                }
            } while (ok);
        }
    }
    goto scan_top;
scan_done:

    // ---- 逐集合装载装备脚本并合并 [-1] 参数条目 ----
    // P183：ORIG 无 script / paramMap 具名局部——每处重新 `call operator->;
    // mov 0xc(%eax),%eax` 取 it->second.m_script（0x08907653/0x0890765e/0x08907686/
    // 0x089076b7/0x089076fd/0x08907729/0x08907756/0x089077a4/0x089077d9/0x089077f4）。
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentPartSetMap_.begin();
         it != g_equipmentPartSetMap_.end(); ++it)
    {
        // ORIG 0x08907619：new 结果经 ebx 跨 ctor 保活后直存成员（无具名指针局部）。
        it->second.m_script = new STEquipmentScript;         // ORIG 0x894 字节

        if (!ImportEquipmentScript(it->second.m_script,
                                   it->second.m_name4.c_str(), false,
                                   moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x16c2,
                                  "ERROR : \xc0\xe5\xba\xf1\xba\xce\xc0\xa7\xbc"
                                  "\xc2\xc6\xae \xb1\xb8\xbc\xba\xc7\xb0 %s "
                                  "\xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc"
                                  "\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
                                  it->second.m_name4.c_str());
            delete it->second.m_script;
            return false;
        }

        // ORIG 0x08907741 先 map::end、0x08907775 后 map::find（单表达式形，P293）。
        if (it->second.m_script->m_paramMap678.find(-1) !=
            it->second.m_script->m_paramMap678.end())
        {
            // ORIG 用 `paramMap[-1]` 取引用再拷贝构造（p167 MISSING ixERS4_），
            // 非 `def->second`（后者多一次 EquipmentParameterInfo 迭代器 ptEv）。
            const EquipmentParameterInfo merged(
                it->second.m_script->m_paramMap678[-1]);
            it->second.m_script->m_paramMap678.clear();
            it->second.m_script->m_paramMap678[it->second.m_pieces.size()] =
                merged;
        }
    }
    return true;
}