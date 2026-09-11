// ----------------------------------------------------------------------------
// ORIG T 0x08664e8e _ZN5CUser17_fnStatQuestClearEjPc（enumQuestClear 静态回调）：
// ctx=[this, elixirIds*, outList*]；find_quest 命中且任务种类（+0xb8）为
// 0/0x19 时，对奖励表×elixirIds 双层遍历，匹配项 add_matched_list 到 outList。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 8 族）：ORIG 0x08664e8e；ORIG 用迭代器遍历
// （begin/end/C1Ev/ptEv/deEv/ppEv/ne 共 14 次），我方索引遍历 ⇒ 符号族不同
// （P175/N89 类），语义等价。
// [b28d-solo-CUserMissingA2] 复验：const_iterator（rewards 指针 const 化）+ P50
// 先声明后赋值 + 缓存 end + 4 迭代器全头部声明。p167 差 2 = ORIG 非 const
// `vector<int>::begin()` + `C1IPiE` 转换构造实例未发射（N89 实例差异，语义等价：
// 我方同点位有 begin/C1IPiE 调用）；残差为 4.1 内层 for-init 旋转到外层体末尾、
// 4.4 恒置开头 ⇒ N164/N82 封顶。
// [b28d-solo-CUserMissingA2②] 本波：getUserQuestRewardItem 已 static（Main 代落
// CUser.h:3037）⇒ 调用点压 2 实参（[esp]=q/[esp+4]=user），与 ORIG 0x08664ecf
// 一致；rewards 补两句式预清（ORIG 0x08664f07 `mov [ebp-0xc],0x0`）。p167 22/24：
// MISSING = ORIG 额外一次 `vector<int>::begin()`+`C1IPiE`（ORIG 0x08664fe8 段内层
// for-init 重复发射，4.4 恒单次，语义等价）。context 自增：`context += 4` /
// `context = context + 4` / `&context[4]` / `(char*)((int*)context+1)` 四形 4.4
// 均折 `add DWORD PTR [ebp+0xc],0x4`（P322② 同族、形参无 volatile 位点），ORIG
// `lea edx,[eax+4]; mov [ebp+0xc],edx` 不可造 ⇒ N 封顶。
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank gap 16）⇒ 修真缺口 + gap 16→3：
// ORIG 初始化段（0x08664f53/0x08664f6e/0x08664f89/0x08664fb6）求外层 begin/end 与
// 内层 begin/end 各一次，此后**每轮外层迭代只重求内层 begin**（0x0866505a 段，位于
// ++ri 之前；内层 end 不再重求）⇒ 原实现把内层 begin+end 都写在体内，静态少 13 条。
// [④ 2026-09-09] 进一步复现 ORIG 块序：内层条件在体外（0x08665044）+ `ppEv` 在体内
// 末尾（0x08665039），begin 重求位于**内层循环之后、外层 ++ri 之前** ⇒ 源形改为
// `while (ii != ie) { …; ++ii; } ii = elixirIds->begin();`（原 for-init 形把 begin
// 重求放外层体开头，difflib 报整块移动）。改后行级 diff 循环段逐行对齐，仅剩 call
// 地址（.o 未重定位）差异。
// 残差 3 条 = 三处 context 形参自增（4.1 `mov;lea;mov;mov` 5 条/处 vs 4.4
// `mov;mov;mov;add mem` 4 条/处）；/tmp/probe_ctx{,2,3,4,5,6,7,8,9,10}.cpp 十组探针
// （4.1.2 与 4.4.7 双编译）穷举 `context++`/`+=4`/`=c+4`/`&c[4]`/`(int)c+4`/`*p++`/
// 引用 cast/逗号形，两侧均不出 `lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)` 写回形
// ⇒ 跨版本 add-vs-lea 结构性差异（N1/N107 族），语义等价（三处均 +4 推进）。
// p167 24/24（MISSING/EXTRA 仅 `__normal_iterator` C1↔C2 同址别名，P262）。
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
}