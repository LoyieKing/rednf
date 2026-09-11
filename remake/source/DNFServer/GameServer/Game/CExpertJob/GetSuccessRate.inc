// GetSuccessRate @ 0x849b2e8：查 m_recipeLevelMap（itemIndex→所需等级），
// 所需等级 - 当前等级 = 等级差 → GetCompoundRate(差) 取率表项；variation =
// ROUND(CInventory::GetExpertJobCompoundRateVariation(type, level))。
// [b28d-solo-CExpertJob] AE（110=110）：P0 去 requiredLevel/level 具名局部（ORIG
//   0x0849b3a6/0x0849b416 均 %ebx 免槽跨 call 保活）+ P198① 函数头预清 +
//   P309 levelDiff 先于 script/variation 声明（复现 -0x1c/-0x18 槽序）；p167 CLEAR。
//   ORIG 0x0849b2e8
stCompoundRate* CExpertJob::GetSuccessRate(CUser* user, CStackableItem const* item,
                                           int& variation)
{
    // [b28d-solo-CExpertJob P198①/P309] ORIG 函数头第一条即 `movl $0x0,-0x1c(%ebp)`
    // （levelDiff 预清）且其槽(-0x1c)深于 script(-0x18) ⇒ levelDiff 必须在 script
    // 之前声明（4.4 头部声明先声明深）、且先于 `variation = 0` 的写出参 store，
    // 否则预清落在中段/顺序反。
    int levelDiff = 0;
    variation = 0;
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return 0;
    int itemIndex = ((CItem*)item)->get_index();
    std::map<int, int>::iterator it = script->m_recipeLevelMap.find(itemIndex);
    if (it == script->m_recipeLevelMap.end())
        return 0;
    // [b28d-solo-CExpertJob P0] ORIG 0x0849b3a6 `mov 0x4(%eax),%ebx` 把 it->second
    // 直传 %ebx 跨 GetCurCharacExpertJobExp/GetCurExpertJobLevel 保活（无槽）⇒ 禁
    // 具名 requiredLevel 局部（P198① 预清见函数头声明）。
    levelDiff = it->second -
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    stCompoundRate* rate = script->GetCompoundRate(levelDiff);
    if (rate == 0)
        return 0;
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    // [b28d-solo-CExpertJob P0] ORIG 0x0849b416 `mov %eax,%ebx`：level 结果同样免槽
    // 跨 getCurCharacExpertJobType 保活（实参 R2L：level 先求值、type 后求值）。
    float f = inven->GetExpertJobCompoundRateVariation(
        (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(),
        user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()));
    // ORIG 0x849b436：fistpl 前 fldcw 置截断模式（mov $0xc,%ah，RC=11），
    // 直接向零截断，无 ±0.5 四舍五入分支。
    variation = (int)f;
    return rate;
}