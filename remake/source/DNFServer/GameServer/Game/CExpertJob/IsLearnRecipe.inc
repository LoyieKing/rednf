bool CExpertJob::IsLearnRecipe(CUser* user, CStackableItem const* item)
{
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
    {
        // [b28d-62] P227 全表达式临时链：ORIG 0x0849b5c9 先把 getCurCharacNo 结果
        //   入 %ebx 跨 cMyTrace::C1 保活，再调 op()；具名 trace 对象会槽化并错序。
        cMyTrace(__PRETTY_FUNCTION__, 0x13f, 0)(
            "EXPERT_JOB_ERROR : GetExpertJobScript() charac_no(%d)",
            user->getCurCharacNo());
        return false;
    }
    // [b28d-62] P198① 预清必写：ORIG 0x0849b60f `movl $0xffffffff,-0xc` 后独立赋值；
    //   符号测试用 P266 字节收窄形（4.4 对 `idx < 0` 恒折 cmpl/jns）。
    int idx = -1;
    idx = script->GetRecipeInfoIndex(((CItem*)item)->get_index());
    if ((unsigned char)((unsigned int)idx >> 31) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x146, 0)(
            "EXPERT_JOB_ERROR : GetRecipeInfoIndex() charac_no(%d)",
            user->getCurCharacNo());
        return false;
    }
    // [b28d-62] ORIG 0x0849b6b0 `call get_recipe; xor $0x1,%eax; test %al,%al; je`
    //   ⇒ 取反物化形（`!call()`）；expand 结果**免槽直用**（P0/P134：ORIG 无
    //   expand 槽，%eax 直接作 get_recipe 的 this）⇒ 内联表达式。
    if (!((CCharacExpertJob*)user->GetCharacExpandData(
              (ENUM_CHARAC_EXPAND_TYPE)0))
             ->get_recipe(idx))
    {
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x14e, 0)(
                "EXPERT_JOB_ERROR : get_recipe() charac_no(%d)",
                user->getCurCharacNo());
            return false;
        }
    }
    return true;
