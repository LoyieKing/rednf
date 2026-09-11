// ============================================================================
// importNPCRelationEventScript — ORIG 0x0890d44f（init.md +0x5a5f，
// "Event/NPCRelationEvent.evt" → m_npcRelation）。
// 顶层仅接受 "[NPC]"（其他 token 直接成功返回）；块内 "[index]" new 0x48 节点
// 并 AddNPCInfo；"[good]/[soso]" 进入好感段：每 kind token 先 GetFavorIndex，
// "[item]" 成对入 AddFavorItemInfo、"[reward]" 单条入 AddRewardItemInfo；
// "[can not give item]" 连续 id 入 AddDislikeItemInfo。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N82/N112] ORIG 0x0890d44f
// 按 ORIG 重排（已修）：
//  ① sub(-0x4c) 移入 [NPC] 内层循环内声明（ORIG 每轮 C1/D1，非函数级）；
//  ② favorKind 改裸短路 if 条件（ORIG eax 物化 mov$1/mov$0，非 int 槽）；
//  ③ 内层循环改 `for(;;)`；curIdx 循环内预清（去函数头重复 store）；
//  ④ 真 bug：`token != "[NPC]"`/`sub != "[can not give item]"` 改 operator==
//     正判（p167 实测 MISSING eq×2 / EXTRA ne×2，ne 选错重载）；
//  ⑤ [NPC] 非匹配与循环 EOF 共享 done 返回岛（goto done）。
// 残余 DIFF 归因（N 封顶，p167 40/40 CLEAR、orig/ours=309/314）：
//  sub 槽 -0x4c vs -0x54（4.4 块内 string 槽序）+ 4.4 continue 标志编码
//  `mov $0x2,%ebx` vs 4.1 `mov $0x1,%ebx` + AddFavor/RewardItemInfo 实参
//  寄存器 edx↔ecx 角色差（N82/N112/N7 族）。
bool importNPCRelationEventScript(const char* path, CNPCRelationEventManager* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                         // ORIG -0x40（ctor 首位）
    std::string deadStr;                       // ORIG -0x44（prologue ctor、尾 dtor，全程无用的死串）
    int dead3c = 0; (void)dead3c;              // ORIG -0x3c prologue $0 写
    bool ok = false;                           // ORIG -0x45
    if (out == NULL)                           // ORIG 判空在 ok 初始化之后
    {
        return false;
    }
    int curIdx;                                // ORIG -0x38（循环内每轮预清）

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        curIdx = 0;
        // ORIG 用 operator== 正判（p167：eq 2 处；写 != 会误选 ne 重载）
        if (token == "[NPC]")
        {
        }
        else
        {
            goto done;                          // ORIG：非 [NPC] 顶层 token 即结束（共享返回岛）
        }

        for (;;)
        {
            std::string sub;                   // ORIG -0x4c（每轮 C1/D1）
            if (!ScanType(sub, true))
            {
                break;
            }

            if (sub == "[index]")
            {
                curIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stNPCRelationEventInfo* info = new stNPCRelationEventInfo;
                out->AddNPCInfo(static_cast<unsigned short>(curIdx), info);
                continue;
            }

            // ORIG 0x0890d5aa：裸短路链物化（mov $1,%eax / mov $0,%eax + test），
            // 非具名 int 局部（具名形产 setne 槽化，见旧 DIFF）。
            if (sub == "[good]" || sub == "[soso]" || sub == "[bad]")
            {
                // ---- 好感段：kind token 循环 ----
                for (;;)
                {
                    std::string kindToken;
                    if (!ScanType(kindToken, true))
                    {
                        break;
                    }
                    const ENUM_NPC_RELATION_EVENT_FAVOR_KIND favor =
                        out->GetFavorIndex(kindToken.c_str());

                    if (kindToken == "[item]")
                    {
                        for (;;)
                        {
                            const int itemId = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            const int count = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            out->AddFavorItemInfo(static_cast<unsigned short>(curIdx), favor,
                                                  static_cast<unsigned int>(itemId),
                                                  static_cast<unsigned int>(count));
                        }
                    }
                    else if (kindToken == "[reward]")
                    {
                        const int a = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        const int b = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        out->AddRewardItemInfo(static_cast<unsigned short>(curIdx), favor,
                                               static_cast<unsigned int>(a),
                                               static_cast<unsigned int>(b));
                        continue;                 // ORIG LAB_0890d75a：bVar2=true 回内层
                    }
                    else
                    {
                        break;                  // 未知 kind token 结束好感段
                    }
                }
                continue;                       // 好感段结束 → 回 [NPC] 内层循环头
            }

            // ORIG 0x0890d78a：operator== 正判（p167：eq 补 2 处 / ne 多 2 处）
            if (sub == "[can not give item]")
            {
                int dislikeId = 0;              // ORIG -0x1c（块首预清）
                for (;;)
                {
                    dislikeId = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    out->AddDislikeItemInfo(static_cast<unsigned short>(curIdx),
                                            static_cast<unsigned int>(dislikeId));
                }
            }
        }
    }
done:
    return true;
}