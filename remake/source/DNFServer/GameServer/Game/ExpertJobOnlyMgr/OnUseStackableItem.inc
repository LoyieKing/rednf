int CAlchemist::OnUseStackableItem(CUser* user, unsigned long itemUniqueId,
                                   CItem const* item)
{
    if (item->is_stackable())
    {
        // vtbl+0xc = GetItemType()==3 且对象 +0x199 字节非零 → 可学 recipe。
        if (((const CStackableItem*)item)->GetItemType() == 3 &&
            // [b23S3C] §4.3 字段化：+0x199 = CStackableItem::m_recipe174(+0x174)
            // .m_field25(+0x25)（CStackableItem.h RecipeInfo 布局注释实证），
            // 裸偏移 `add $0x199 + movzbl` 形 ⇒ 具名成员直读（ORIG movzbl 0x199）。
            ((const CStackableItem*)item)->m_recipe174.m_field25 != 0)
        {
            STExpertJobScript* script =
                (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(
                    user->GetCurCharacExpertJobType());
            if (script != 0)
            {
                // [b23S3C] §4.1 反发明符号 + P0/P48 去缓存：ORIG 0x085d17a4/0xb6
                // 两次直调 CUser::GetCharacExpandData(0)（无本域 helper），
                // exp/level/learnRecipe 结果全程 %eax/%ebx 无槽（帧 sub $0x24，
                // 仅 -0xc 一个 script 局部）⇒ 实参右起内联（exp→level→expand→
                // uid/script 落槽，与 ORIG call 序一致）。
                // [b23S3C] P191 极性（learnRecipe 返回 bool 后复查）：ORIG 0x85d17d5
                // `test %al,%al; je +0xeb`（false 跳转到 return 0xd 岛、成功路径
                // fall-through）⇒ 正向 if + else(return) 形（负形 if(!x) 使 4.4 产
                // xor $1 折叠，多 1 条）。
                if (((CCharacExpertJob*)user->GetCharacExpandData(
                         (ENUM_CHARAC_EXPAND_TYPE)0))
                        ->learnRecipe(
                            itemUniqueId, script,
                            user->GetCurExpertJobLevel(
                                user->GetCurCharacExpertJobExp())))
                {
                    ((CCharacExpertJob*)user->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)0))
                        ->SendExpertJobInfo(user);
                }
                else
                {
                    return 0xd;
                }
            }
        }

        // ORIG 0x085d17e2：sub_type()==2 的 APC 药水——记录使用截止时间
        // SetUseAPCPotionTime(curSec + m_int2d0/1000)。
        // 反汇编铁证：imul $0x10624dd3 + sar $0x6 = 除以 1000（毫秒→秒），
        // 非 1e6（0x431bde83+sar 0x12 才是 /1e6；旧注释误判魔数）。
        if (((const CStackableItem*)item)->get_sub_type() == 2)
        {
            user->SetUseAPCPotionTime(
                GlobalData::s_systemTime_.getCurSec() +
                ((const CStackableItem*)item)->m_int2d0 / 1000);
        }
    }

    // 固定道具唯一 id 分派（分支序照 ORIG 汇编比较树）：
    //   0x28e7cf / 0x27ac54 / 0x29f287 / 0x29f2ed → UseRevivalItem
    //   0x27acda → UseDelilah；其余不处理。
    // [b23S3C] ORIG 0x085d185c 起 `mov 0xc(%ebp),%eax` 一次载入后以
    // 0x28e7cf 为中枢二分（cmp/je、cmp/ja、cmp/je、cmp/je、jmp、cmp/je、cmp/jne）
    // ⇒ 原码是 switch(itemUniqueId)（或然链形会逐次 cmp 内存实参且无线性载入）。
    switch (itemUniqueId)
    {
    case 0x28e7cfUL:
    case 0x27ac54UL:
    case 0x29f287UL:
    case 0x29f2edUL:
        UseRevivalItem(user);
        break;
    case 0x27acdaUL:
        UseDelilah(user);
        break;
    default:
        break;
    }
    return 0;
