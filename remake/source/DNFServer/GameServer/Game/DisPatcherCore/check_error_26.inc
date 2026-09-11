// ORIG 0x081c2d9a T _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_DropItem/check_error.md 汇编+反编译 C 全文转写）：
// 角色空/数量<1 → -1；state==10 → 0x13；组队血副本 → 0x13；综合PVP频道 → 0x13；
// isRestricted(1,1) → param.m_c=1,0xd1；m_padTail!=1 时安全保护 0x1e（param.m_c=1,返码）；
// CheckItemLock → 0xd5；isHackUser → 0x13；space 1 → 0x17；space 3 且 pos<10 → 0x17；
// [b28d-29] N82/P211：形参副本槽 + store 装载序差，调用点与槽集合全等  (ORIG 0x081c2d9a insn 146->148)
// space 0 且 pos==0 且 count>1000 → 0x46；否则 0。（ORIG +0x18b 存在未读死局部 6）
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 56（ORIG 146 / ours 148），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
unsigned int DisPatcher_DropItem::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }
    // [DPC_CheckErr] P28/P331 声明序=槽序（ORIG 0x081c2d9a 起为
    // `call getCurCharacR; test %eax,%eax; sete %al; test %al,%al; je +0x25` 先判，
    // **之后** 0x081c2dc7 才落 msg 视图槽 `mov 0x10(%ebp),%eax; mov %eax,-0x20(%ebp)`，
    // 再判 `mov 0x14(%eax),%eax; test %eax,%eax; jg +0x3f`，
    // **之后** 0x081c2dd9 才落 param 视图槽 -0x1c）⇒ 两个视图声明必须分别后置于
    // 对应检查（我方原把 req/out 声明放函数首，视图槽写在检查之前）。
    const MSG_DROP_ITEM& req = *reinterpret_cast<const MSG_DROP_ITEM*>(&msg);
    if (req.m_14 < 1)
    {
        return -1;
    }
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    if (user->get_state() == 10)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // ORIG 槽序：req(-0x20) → out(-0x1c) → party(-0x18) → **deadLocal(-0x14)** →
    // dungeon(-0x10) → secu(-0xc)（`movl $0x6,-0x14(%ebp)` 落 party 之后）⇒
    // deadLocal 声明位于 party 与 dungeon 之间。
    int deadLocal;               // ORIG -0x14 槽（+0x18b 写 6 未再读）
    // [G5-MiscC] P0：ORIG 0x081c2e15 `mov -0x18(%ebp),%eax; mov 0xcac(%eax),%eax;
    // mov %eax,-0x10(%ebp)` 有具名槽 -0x10（随后 0x081c2e1e `cmpl $0x0,-0x10(%ebp)`、
    // 0x081c2e24 `mov -0x10(%ebp),%eax; movzbl 0x89f(%eax),%eax` 复用该槽）
    // ⇒ m_pDungeon 结果原码是具名局部，不是两次链式直读。
    // [DPC_CheckErr] 语义轴（补缺失校验）：ORIG 0x081c2e0c `cmpl $0x0,-0x18(%ebp)`
    // + 0x081c2e10 `je 0x081c2e3c`（落点 = 后续 IsIntegratedPvPBaseChannel 检查）
    // ⇒ 原码在解引用 party **之前**有判空；我方原 `party->m_battleField.m_pDungeon`
    // 在 party==0 时空指针解引用（行为分叉：ORIG 跳过 dungeon 检查继续走）。
    // ORIG 的 dungeon 槽写（0x081c2e1b `mov %eax,-0x10(%ebp)`）落在 party 判空
    // 之后，且**无** `movl $0x0,-0x10(%ebp)` 预清 ⇒ 原码是条件内赋值短路形。
    // [K6-DpcSigSmallCheckErr] 改前形 = 单条 if 的三段 `&&`（中间含赋值）→ 中间形 =
    // 外层 `party != 0 && (dungeon = ...) != 0` 仍含赋值（4.4 实测仍物化，151 未降）→
    // 改后形 = 赋值移出条件（`&&` 内不再有 MODIFY_EXPR）。依据 ORIG
    // 0x081c2e0c/0x081c2e1e/0x081c2e2c 三处 `je` 全部指向同一出口 0x081c2e3c（无
    // `mov $0x1/$0x0,%eax` 岛）⇒ 4.1 直跳折叠。
    CDungeon* dungeon;
    if (party != 0)
    {
        dungeon = party->m_battleField.m_pDungeon;
        if (dungeon != 0 && dungeon->m_bloodDungeonType != 0)
        {
            return 0x13;
        }
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        out.m_c = 1;
        return 0xd1;
    }
    if ((unsigned char)req.m_padTail[0] != 1)
    {
        int secu = GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x1e));
        if (secu != 0)
        {
            out.m_c = 1;
            return secu;
        }
    }
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_11), (int)req.m_12))
    {
        return 0xd5;
    }
    if (user->isHackUser())
    {
        return 0x13;
    }
    // [DPC_CheckErr] P28：ORIG 0x081c2f25 `je 0x081c2f3a`(=+0x18b) 的落点即
    // `movl $0x6,-0x14(%ebp)` ⇒ 赋值在 isHackUser 早退检查**之后**（我方原在其之前）。
    deadLocal = 6;               // ORIG +0x18b（未再读）
    // [G5-MiscC] P251/P261：ORIG 0x081c2f2c-0x081c2f40 是**单次**载入
    // （`mov -0x20(%ebp),%eax; movzbl 0x11(%eax),%eax; movsbl %al,%eax`）后
    // `cmp $0x1,%eax; je <case1 0x081c2f42>` / `cmp $0x3,%eax; je <case3 0x081c2f49>` /
    // `jmp <default 0x081c2f5d>` = switch 直比；case3 内 0x081c2f4c
    // `movzwl 0x12(%eax),%eax; cmp $0x9,%ax; jg <default>`（16 位无符号）
    // ⇒ 原码是 switch（我方两次 if 会重复载入 m_11 并多一次 cmp）。
    switch ((int)(char)req.m_11)
    {
    case 1:
        return 0x17;
    case 3:
        if (req.m_12 <= 9)
        {
            return 0x17;
        }
        break;
    default:
        break;
    }
    if ((unsigned char)req.m_11 == 0 && (unsigned short)req.m_12 == 0 &&
        (unsigned int)req.m_14 > 0x3e8)
    {
        return 0x46;
    }
    return 0;
}
