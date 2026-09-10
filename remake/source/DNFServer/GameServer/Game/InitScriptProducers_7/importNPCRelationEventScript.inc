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
