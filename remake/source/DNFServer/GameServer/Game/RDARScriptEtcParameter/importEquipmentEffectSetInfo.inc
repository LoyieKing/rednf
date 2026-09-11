// ===========================================================================
// importEquipmentEffectSetInfo —— ORIG T 0x0890793f
// （docs/class_func_reports/_lt_global_gt_/importEquipmentEffectSetInfo.md）
// 阶段一：解析 "[effect equipment part set]" 块 → STEquipmentPartSetData
//   （m_field0=ScanInt、m_name4=ScanStr、m_pieces 每件
//   getSameStringIndex(characterEquipmentTypeTags,0x1a)），写入
//   g_equipmentEffectSetMap_[id]；阶段二：遍历生成 STEquipmentScript 并
//   ImportEquipmentScript(script, name, false, moduleType, id)，失败记日志
//   （0x1712）删除后返 0；m_paramMap678 若含 key=-1 档则复制后清空，
//   以套装件数为 key 重存。loadRDARScriptFile 失败记日志（0x16df）返 0。
// 验收：P167 差集 66/66 归零（op-> 13×、op[] 3×、find/end/clear/size/EPI
// 拷贝链逐点对应）；残余 = 循环出口多路 dtor 链块序与槽号
// （N82/EH merge 跨版本族）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95/N82：ORIG 0x0890793f。本波修：
// ①okPiece 具名局部删除、改 `for(;;){ piece; if(!ScanStr(&pieceName)) break; ... }`
// ⇒ 消 4.4 落槽物化（`mov %al,-0x21; cmpb`），与 ORIG 寄存器直测同族；
// ②`pNew` 具名局部删除、内联 `it->second.m_script = new STEquipmentScript;`
// ⇒ 消 -0x20 槽往返（ORIG new 结果留 %ebx）。347/343，ndiff 278→173。
// 残余 = 帧顶槽基差 4（ORIG token -0x34 vs 我方 -0x30；4.4 帧顶 gap 0x24 vs
// 4.1 0x28）+ 帧 0x50c vs 0x4fc：未使用标量/数组局部 4.4 不占槽（int/float
// 取址探针零效），double framePad 反致 +0xc 过度下移 ⇒ N95/N82 族封顶。
// p167 CLEAR（66/66），语义闭合。
int importEquipmentEffectSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              // Q7S4: ORIG 0x0890796c movl $0x8e103d8 = 完整前缀串（我方此前仅 "%s"）
                              __FUNCTION__, 0x16df,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    std::string token;                    // ORIG 三 string 先构造（-0x34/-0x38/-0x3c）
    std::string name;
    std::string pieceName;
    g_equipmentEffectSetMap_.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[effect equipment part set]")
        {
            STEquipmentPartSetData data;
            data.m_field0 = ScanInt((bool*)0);
            ScanStr(&name);
            data.m_name4 = name;
            // ORIG 无 okPiece 槽（ScanStr 结果留寄存器 xor$1/test/je，P0）⇒ 用 for(;;)+早退
            for (;;)
            {
                STEquipmentPartSetPiece piece;
                if (!ScanStr(&pieceName))
                    break;
                piece.m_field4 = getSameStringIndex(pieceName.c_str(),
                                                    characterEquipmentTypeTags, 0x1a);
                data.m_pieces.push_back(piece);
                g_equipmentEffectSetMap_[data.m_field0] = data;
            }
        }
    }
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentEffectSetMap_.begin();
         it != g_equipmentEffectSetMap_.end(); ++it)
    {
        // P157：ORIG 每次字段访问重调 _Rb_tree_iterator::operator->（共 13+ 次），
        // 禁缓存 it->second 引用；m_script 传参/删除前均重读。
        it->second.m_script = new STEquipmentScript;   // ORIG 无 pNew 槽（new 结果留 %ebx，P0）
        if (!ImportEquipmentScript(it->second.m_script,
                                   it->second.m_name4.c_str(), false,
                                   (int)moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x1712,
                                  "ERROR : \xc0\xe5\xba\xf1\xba\xce\xc0\xa7\xbc\xc2\xc6\xae \xb1\xb8\xbc\xba\xc7\xb0 %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
                                  it->second.m_name4.c_str());
            // ORIG 同一次 op-> 内读字段两遍（ebx 缓存）：具名指针缓存复现
            // [b28d-solo-D02 实测] 表达式形 `if (it->second.m_script) delete
            // it->second.m_script;` 反劣（347→351：4.4 发 `test;setne;test;je`
            // 双物化 + 两次字段读）⇒ 具名缓存形保留，本处按 P0/N18a 封顶。
            STEquipmentScript* pDel = it->second.m_script;
            if (pDel)
                delete pDel;                  // ORIG 不回写 m_script=0（P171 照抄）
            return 0;
        }
        // key=-1 档存在性检查（find 以 int& 具名槽传参，RS4_ 形）
        int kDel = -1;
        if (it->second.m_script->m_paramMap678.find(kDel) !=
            it->second.m_script->m_paramMap678.end())
        {
            int kNeg = -1;
            EquipmentParameterInfo tmp(
                it->second.m_script->m_paramMap678[kNeg]);   // op[] 引用拷贝构造（0x4bc 大 temp）
            it->second.m_script->m_paramMap678.clear();
            int nPieces = (int)it->second.m_pieces.size();
            it->second.m_script->m_paramMap678[nPieces] = tmp;
        }
    }
    return 1;
}