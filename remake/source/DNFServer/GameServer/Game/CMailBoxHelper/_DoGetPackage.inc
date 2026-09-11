// ---- 取件内部实现 ----
// [b28d-solo-CMailBoxHelper 本波] 修复：①具名 box 局部（P0/P115，ORIG
// 0x0855466f `mov %eax,-0x18(%ebp)` 存槽判空；直判形 4.4 折
// `test %eax,%eax; sete %al; test %al,%al; je`）；②P198① 两处预写
// （money=0 @0x08554682 / limit=0x7fffffff @0x0855470e）。
// 残余 N：ORIG 0x08554697 槽比零物化 `cmpl $0; setne %al; test %al,%al; je`
// 被 4.4 恒折 `cmpl $0; je`（N197④，P168a④ 补：非绑定不可造）+ 帧
// 0x110 vs 0x120 与槽基差 0xc（N82/P112）；p167 CLEAR 29/29，243 vs 244。
// [本波 2026-09-09 真 bug] isAvatarPottery 调用从局部 `namespace
// AvatarRouletteServer { bool isAvatarPottery(unsigned long); }` 假声明改为
// 权威类成员调用 `((AvatarRouletteServer*)GlobalData::s_pAvatarRoulette)
// ->isAvatarPottery(itemIdx)`：ORIG 0x085548ac..0x085548b8 压栈
// `mov &s_pAvatarRoulette,%eax; mov %eax,(%esp); mov itemIdx,0x4(%esp)` 实证
// 是成员调用（this=s_pAvatarRoulette）。旧形少压 this ⇒ 运行期 this 为垃圾。
// 243 vs 245（+2 为 this 装载/压栈），p167 仍 CLEAR（符号名相同，差集看不出）。
bool CMailBoxHelper::_DoGetPackage(CUser* user, CMailBox::CMail const* mail,
                                   std::vector<std::pair<int, int> >& items,
                                   bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08554650
    // [b28d-solo-CMailBoxHelper] P0/P115：ORIG 0x0855466f `mov %eax,-0x18(%ebp)`
    // 存槽 + 0x08554672 `cmpl $0x0,-0x18(%ebp); jne` 判空 ⇒ 原码具名 box 局部
    // （直判形 4.4 折 `test %eax,%eax; sete %al; test %al,%al; je`，P197④）。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return false;

    // P157/P183：ORIG 三次独立 GetPackageItem()（bReset 分支 / !bReset 分支 /
    // 主流程），无引用缓存（Ghidra C local_ac/local_6f/local_e9 三处 sret）。
    // [b28d-solo-CMailBoxHelper] P198①：ORIG 0x08554682 `movl $0x0,-0x14(%ebp)`
    // 在 call GetPackageMoney 之前 ⇒ 原码两句式（预清 + 赋值）。
    unsigned int money = 0;
    money = mail->GetPackageMoney();

    // [2026-09-09 SweepCMailBoxHelper-2 复验] P225 while+break 形在此**反劣**
    // （245→249，4.4 对大 body 生成真循环回边；探针 f2 仅单语句 body 时同形）⇒
    // 维持 if 形，残余 setne/test 物化 2 条归 P197④/N122 跨版本族。
    // [本波 2026-09-10] P225：ORIG 0x085546db 为 `cmpl $0x0,-0x14(%ebp);
    // setne %al; test %al,%al; je`（4.1 对整型槽比零的条件物化）；4.4 对
    // `if (money != 0)` 恒折 `cmpl; je` 直跳 ⇒ 用 while+break 形复现四段物化
    // （实测 ndiff 273→2；残余 2 条 `jmp <exit>` 系 4.4 为多出口 body 发的中继，
    //  单 case switch 形反劣 ndiff=5 已回退）。
    // [W13G 2026-09-11 落地·探针实测] while+break → **if**（去 break；语义恒等：
    //   `while(x){…;break;}` ≡ `if(x){…}`）。
    //   实测（routed 影子编译）：`_DoGetPackage` gap **+2 → 0**（ours 243 = ORIG 243），
    //   TU 四数 142/309/1/6 逐项不变。
    //   ⛔ **负结论（勿再试旋钮）**：残余唯一 diff 块 = ORIG 的 2 条条件物化
    //   `0x0855469b setne %al; 0x0855469e test %al,%al`（4.1 对**内存槽比零**
    //   `cmpl $0x0,-0x14(%ebp)` 的物化形）；4.4 对 `if (整型 != 0)` 恒折
    //   `cmpl; je` 直跳，**不物化**。
    //   已实测四形：① `if(x!=0)` ⇒ 无物化、无中继（**本形**）；② `while+break`
    //   ⇒ 有物化但为「多出口 body + break」另发 2 条中继 `jmp <exit>`（净 +2，
    //   即改前状态）；③ 具名 `bool b=(x!=0); if(b)` ⇒ 多「存槽+cmpp」2 条
    //   （探针 `/tmp/w13/probe/p3.cpp` f5：`setne %al; movb %al,-9(%ebp);
    //   cmpb $0,-9(%ebp)`，非 ORIG 的 `test %al,%al`）；④ 单 case switch 形
    //   早前实测 ndiff=5（注释历史）。
    //   ⇒ 4.4 无任何源码形可同时产「物化」与「无中继」⇒ 跨版本 N 档封顶。
    if (money != 0)
    {
        if (bReset)
        {
            if (IsCeraPointItem((unsigned long)(unsigned int)
                                    mail->GetPackageItem().m_addInfo))
            {
                if (user->GetCeraPoint() + (int)money > 1000000000u)
                {
                    err = (ENUM_ERROR)0xb;
                    return false;
                }
            }
            else
            {
                // [b28d-solo-CMailBoxHelper] P198①：ORIG 0x0855470e
                // `movl $0x7fffffff,-0x10(%ebp)` 预写 ⇒ 原码两句式。
                int limit = 0x7fffffff;
                limit = G_CDataManager()->GetMoneyLimitPerLevel(
                    user->get_charac_level(), user->get_acc_name());
                if (limit - user->getCurCharacMoney() < (int)money)
                {
                    err = (ENUM_ERROR)0xa;
                    return false;
                }
            }
        }
        else
        {
            if (!IsCeraPointItem((unsigned long)(unsigned int)
                                     mail->GetPackageItem().m_addInfo))
            {
                // [2026-09-09 Lane20] P52：ORIG 0x085547a1 段实参先物化 %ebx
                // （`call GetSenderName; mov %eax,%ebx`）再求 receiver
                // （`call GetGoldControl`）⇒ 单语句链式表达式；具名 gold 局部
                // 会反序（先算 receiver 存槽，实参经槽中转）。
                user->GetGoldControl()->SetMailCharName(mail->GetSenderName());
                user->getCurCharacInvenW()->gain_money((int)money,
                                                       (eMoneyAddReason)0xe, true, 0);
            }
            // ORIG 0x08554650 段：make_pair<ENUM_ITEMSPACE, INVEN_SLOT>
            items.push_back(
                std::make_pair((ENUM_ITEMSPACE)0, (Inven_Item::INVEN_SLOT)0));
        }
    }

    // 主流程一次 sret（ORIG local_e9），isAvatarPottery 分支复用同一 sret。
    // [本波 2026-09-10] 按值直构（P0）：ORIG 0x8554896 `lea -0xe7(%ebp),%eax` +
    // `mov -0xe5(%ebp),%eax` 直读对象槽字段（对象基址+0x02 折叠）⇒ 原码是
    // `Inven_Item pkg = mail->GetPackageItem();`（sret 直构进具名对象槽、无引用
    // 指针槽）；旧 `Inven_Item const& pkg` + `unsigned long itemIdx` 使 4.4 生成
    // 指针槽 + 两级解引用 `mov -0x18(%ebp),%eax; mov 0x2(%eax),%eax` 且多一枚
    // itemIdx 槽（与 _DoGetPackageItem 同族修法）。
    Inven_Item pkg = mail->GetPackageItem();
    if (pkg.m_addInfo == 0)
        goto return_true; // ORIG 0x0854a05 尾块 `mov $0x1,%eax`（je 直跳共享出口）
    if (pkg.m_addInfo == 1)
        return _DoGetPackageCoin(user, mail, items, bReset, err);

    // [本波 2026-09-09 真 bug 修复] ORIG 0x085548ac..0x085548b8 压栈实证：
    // this = GlobalData::s_pAvatarRoulette（`mov &s_pAvatarRoulette,%eax; mov %eax,(%esp)`）
    // + 1 个显式实参 ⇒ 是**成员调用**。旧局部 `namespace AvatarRouletteServer {
    // bool isAvatarPottery(unsigned long); }` 声明把成员当命名空间函数调用，
    // 少压 this ⇒ 运行期 this 为垃圾（读 this+0x18）。改权威类成员调用。
    if (reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
            ->isAvatarPottery((unsigned long)(unsigned int)pkg.m_addInfo))
        return AvatarCoin::GetPackage(
            user, pkg.m_addInfo2, bReset); // +0x07 avatar uid

    {
        CItem* proto = G_CDataManager()->find_item((int)pkg.m_addInfo);
        if (proto == 0)
        {
            err = (ENUM_ERROR)1;
            return false;
        }
        if (proto->IsAvatarItem())
            return _DoGetPackageAvatar(user, mail, items, bReset, err);
        if (proto->IsCreatureItem())
            return _DoGetPackageCreature(user, mail, items, bReset, err);
        return _DoGetPackageItem(user, mail, items, bReset, err);
    }
return_true:
    return true;
}