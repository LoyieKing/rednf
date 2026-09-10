bool CUser::_fnStatQuestClear(unsigned int questIdx, char* context)
{
    // [b28d-solo-CUserMissingA2②] ORIG 0x08664e9e/0x08664eaa/0x08664eb6 对 context
    // 逐次 `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov %edx,0xc(%ebp); mov (%eax),%eax`
    // = 读旧指针 + 形参就地推进（每处均写回，第三处同样推进）。
    // 4.4 对形参 `context += 4`/`context++` 恒折单条 `add DWORD PTR [ebp+0xc],0x4`；
    // 4.1 的 `mov/lea/mov` 三连（探针 /tmp/probe_ctx*.cpp 四组穷举：4.1/4.4 均不出 lea 写回形）
    // ⇒ 该 1 条/处之差属跨版本 add-vs-lea 结构性差异（N1/N107 族），语义等价。
    CUser* user = *(CUser**)context;
    context += 4;
    std::vector<int>* elixirIds = *(std::vector<int>**)context;
    context += 4;
    std::vector<std::pair<int, int> >* outList =
        *(std::vector<std::pair<int, int> >**)context;
    context += 4;

    Quest* q = G_CDataManager()->find_quest((int)questIdx);
    if (q == 0)
        return 1;
    if (q->m_fieldB8 != 0 && q->m_fieldB8 != 0x19)
        return 1;

    // [b28d-solo-CUserMissingA2②] ORIG 0x08664f07 `mov DWORD PTR [ebp-0xc],0x0`
    // 预清 + 0x08664f13 赋值 ⇒ 两句式具名局部（P198①）。
    const std::vector<RewardItemInfo>* rewards = 0;
    rewards = user->getUserQuestRewardItem(q, user);
    // P50 复活（b28c C28L29/C28L14）：ORIG 0x08664e8e 有独立 `const_iterator::C1Ev`
    // （×4）+ `C1IPiE` 转换构造（×2）+ 纯 const `ne` ⇒ 原码是「先声明后赋值 +
    // 缓存 end」形（初始化式形会被 4.4 折掉 C1Ev 并改 ne 重载）。
    // [③] 初始化顺序照 ORIG：外层 begin/end（0x08664f53/0x08664f6e）先、内层
    // begin/end（0x08664f89/0x08664fb6）后；ie 只求这一次。
    std::vector<RewardItemInfo>::const_iterator ri;
    std::vector<RewardItemInfo>::const_iterator re;
    std::vector<int>::const_iterator ii;
    std::vector<int>::const_iterator ie;
    ri = rewards->begin();
    re = rewards->end();
    ii = elixirIds->begin();  // ORIG 0x08664f89 初始化段的一次 begin
    ie = elixirIds->end();    // ORIG 0x08664fb6：内层 end 全程只求这一次
    for (; ri != re; ++ri)
    {
        // ORIG 0x08665044 内层条件在体外、0x08665039 `ppEv` 在体内末尾；begin 重求
        // （0x0866505a）位于**内层循环之后、外层 ++ri 之前**（非内层 for-init）。
        while (ii != ie)
        {
            if (ri->m_itemIdx == *ii)
            {
                user->add_matched_list(*outList, ri->m_itemIdx, ri->m_count);
            }
            ++ii;
        }
        ii = elixirIds->begin();  // ORIG 0x0866505a：每轮外层末重求内层 begin
    }
    return 1;
