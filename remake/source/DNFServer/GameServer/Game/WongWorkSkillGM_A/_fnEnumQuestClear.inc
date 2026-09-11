// ============================================================================
// ORIG T 0x0860a6da static _fnEnumQuestClear(uint,char*)：enumQuestClear 回调。
// va 打包区依次：int& spA,int& spB,int& sfpA,int& sfpB,this,user。
// 查任务；仅限 m_fieldB8 == 0 或 25（一般/重复类任务）统计其奖励物品；
// SP 书（0x407/0x40e 与表格项）与 SFP 书（0x4b4/0x4b5）分别累计。恒返回 true。
// [P3S3 定稿] N82 已解（帧 0x64→0x54、槽与符号全对齐）：
//   ① rewards 提为 `const std::vector<...>*`：ORIG 0x0860a7aa/0x0860a7bf 的
//      begin/end 目标 = `_ZNKSt6vectorI14RewardItemInfoSaIS0_EE5beginEv/3endEv`
//      （const 版）且**无** iterator→const_iterator 转换 ctor 调用 ⇒ 我方旧形
//      多出 `__normal_iterator<const T*>::C1<T*>` + 槽 -0x3c（帧 0x64）。
//   ② end 缓存在 for-init 双声明：ORIG 0x0860a7bf 的 end() 只调一次
//      （0x0860a843 条件处仅 `ne`）⇒ it -0x34 / end -0x38 与 ORIG 逐槽一致，
//      `ne` 符号亦降为 2 模板参 const 版（ORIG 0x083e5e57 同名）。
//   残余 N107（唯一 diff，gap +5）：ORIG **六组**形参自增 `mov 0xc(%ebp),%eax;
//   lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)`（2 条/组，含 0x0860a727 第 6 组读
//   user 后亦自增 ⇒ 我方同步补第 6 次 `args += 4`，our 114→115，gap +6→+5）vs
//   4.4 恒 RMW 折叠 `addl $0x4,0xc(%ebp)`（1 条/组）。已实测六形均不可复制 lea
//   形：`args += 4` / `args = args + 4` / `args = &args[4]` /
//   `(char*)((int)args+4)` / `(char*)((int**)args+1)` 全产 addl；
//   `(char*)((unsigned int)args + 4u)`（P5 类型轴旋钮③）确能解除 RMW 折叠但产
//   `add $0x4,%eax; mov %eax,0xc(%ebp); mov 0xc(%ebp),%eax`（多 1 条 reload）
//   ⇒ 劣化（our 123、gap -3），已回滚。判 N 封顶（4.1↔4.4 固有）。
//   历史：[A28 round5] rewards 预清两句式（ORIG 0x0860a781 `movl $0x0,-0x14`，P198①）。
// ============================================================================
bool CSkillChanger::_fnEnumQuestClear(unsigned int questIdx, char* args)
{
    // b28d-L3 复测（P0/P183）：ORIG 0x0860a6da 为**形参自增指针**形 —— 每个槽
    //   `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov %edx,0xc(%ebp); mov (%eax),%eax;
    //    mov %eax,-0x30…(%ebp)`（6 连，偏移步进 4）⇒ 直接对 args 做 `+= 4` 步进，
    //   不设 `slots` 中间层（旧形多一具名槽 -0x34 且读形为 `mov -0x34(%ebp),%eax;
    //   add $0x4,%eax`）。
    int* spA = *(int**)args;
    args += 4;
    int* spB = *(int**)args;
    args += 4;
    int* sfpA = *(int**)args;
    args += 4;
    int* sfpB = *(int**)args;
    args += 4;
    CSkillChanger* self = *reinterpret_cast<CSkillChanger**>(args);
    args += 4;
    CUser* user = *reinterpret_cast<CUser**>(args);
    args += 4;                            // ORIG 0x0860a727-0x0860a72d：第 6 组读后亦自增

    Quest const* quest = G_CDataManager()->find_quest(static_cast<int>(questIdx));
    if (quest == 0)
        return true;
    if (quest->m_fieldB8 != 0 && quest->m_fieldB8 != 0x19)
        return true;

    // ORIG 0x0860a781：`movl $0x0,-0x14(%ebp)` 预清后再 call（P198① 两句式）。
    // const 视图 + end 缓存：见顶部 [P3S3 定稿] ①②（ORIG 0x0860a7aa/0x0860a7bf）。
    const std::vector<RewardItemInfo>* rewards = 0;
    rewards = user->getUserQuestRewardItem(quest, user);   // ORIG _ZN5CUser22…PKS_
    for (std::vector<RewardItemInfo>::const_iterator it = rewards->begin(),
             end = rewards->end();
         it != end; ++it)
    {
        const int itemIdx = it->m_itemIdx;   // +0x00
        const int count = it->m_count;       // +0x04
        self->_addSPRelatedItem(spA, spB, itemIdx, count);
        self->_addSFPRelatedItem(sfpA, sfpB, itemIdx, count);
    }
    return true;
}