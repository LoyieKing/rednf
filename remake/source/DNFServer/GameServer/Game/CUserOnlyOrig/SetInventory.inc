// ---------------------------------------------------------------------------
// CUser::SetInventory(SIG_LOAD_INVENTORY* sig) T 0x0864d160 sz0x35a
// 报告：docs/class_func_reports/CUser/SetInventory.md
// SIG_LOAD_INVENTORY 权威字段（SigTypes_C.h）填充；CInventory +0x1c/+0x55a
// 两个 memcpy 目标暂为布局偏移（字段化 owner=CInventory 域批次）。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0864d160：p167=CLEAR，gap=11；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0864d160：N82 封顶——帧/槽差（p167 CLEAR，gap 11）。
bool CUser::SetInventory(SIG_LOAD_INVENTORY* sig)
{
    unsigned int money = sig->m_money;
    unsigned int coin = sig->m_coin;
    unsigned int payCoin = sig->m_payCoin;
    unsigned int eventCoin = sig->m_eventCoin;
    // [CoBatchB 2026-09-11 具名 size 槽] ORIG 0x0864d19a..0x0864d1c8：三个区各占相邻
    // **一对**槽（-0x3c/-0x38、-0x34/-0x30、-0x2c/-0x28），调用点从槽回读 size
    // （0x0864d340 `mov -0x38(%ebp),%edx; mov %edx,0xc(%esp)`）⇒ size 是具名 int 变量。
    // 注：整块 {char*,int} 结构体形实测 **217→237 过头**（4 个 data 值被 4.4 判为跨调用
    // 活跃 ⇒ 多 `push %esi` 保活 + 每次访问多一对 store/load）；故只把 size 具名化。
    char* inven1 = (char*)sig->m_invenType1;
    int inven1Size = 0x4a58;
    char* inven2 = (char*)sig->m_invenType2;
    int inven2Size = 0x1905;
    char* equipMirror = (char*)sig->m_equipMirror;
    int equipMirrorSize = 0x53e;
    char* cargo = (char*)sig->m_cargo;
    int cargoCapacity = static_cast<int>(sig->m_cargoCapacity);
    char* inven3 = (char*)sig->m_invenType3;
    int inven3Size = 0x39aa;                                   // ORIG 0x0864d…@-0x1c/-0x18
    if (getCurCharacR() == 0) {
        // [FL5 2026-09-10 负结论·已回滚] P37/P227 链在本件 ndiff 317→317 中性 ⇒ 维持具名形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1271, 5);
        trace("CUser::SetInventory m_selected is null");
        return 0;
    }
    // [K7 2026-09-11 单旋钮·复现 ORIG 独有段] ORIG 0x0864d24a `mov -0x2c(%ebp),%eax` +
    // 0x0864d24d `mov %eax,-0x14(%ebp)`（循环前 equipMirror 的栈副本槽，2 条）⇒ 原码有具名
    // 循环别名；我方 0x0000fb90 仅有 `movl $0x0,-0x14(%ebp)`（i 槽）。
    char* equipLoop = equipMirror;                 // ORIG 0x0864d24a/d24d
    for (int i = 0; i < 0xc; ++i) {
        // [M8COB 2026-09-11 P183 去缓存·已回滚] ORIG 0x0864d19x 比较左侧确实无 invenR 槽
        // （`mov -0x10(%ebp),%edx; imul $0x3d,%edx,%edx; add %edx,%eax; add $0x10,%eax;
        // mov 0xe(%eax),%edx`），但 4.4 把内联 receiver 的 cast 链重算成 3 条 add 链体
        // ⇒ 实测 227→**228**（gap 0→-1，反劣）⇒ 回滚，维持具名 invenR（4.4 最优 227）。
        // [A06 2026-09-10 C05④/P225 试行记录·已回退] ORIG 0x0864d1a4 段为
        // `cmp %eax,%edx; setne %al; test %al,%al; je <T>`；按 P225 改 `while (cond) { body; break; }`
        // 实测**无效**（227/238→227/239、delta 268→267），且本处两侧均非具名局部
        // （元素字段 vs 镜像字段）⇒ 命中 Main 转发的 C05② 边界（仅「两个具名局部比较」命中）⇒ 回退。
        const CInventory* invenR = getCurCharacInvenR();
        if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(invenR) + i * 0x3d + 0x1e) !=
            *reinterpret_cast<int*>(equipLoop + i * 0x3d + 2)) {
            m_characterView.enableSaveCharacView();
            break;
        }
    }
    // [P183/P52 去缓存] ORIG 全程无 inven 具名局部：每处 `call getCurCharacInvenW`
    // 结果直接 `mov %eax,(%esp)` 作 this（如 0x0864d2b1/0x0864d398），旧稿 -0x20 槽
    // 每处多 store+load 一对（10 处 ⇒ +20 条，且掩盖 ORIG 的 push %ebx 实参物化形）。
    getCurCharacInvenW()->m_avatarCoin.Set(sig->m_avatarCoin);  // +0x658 = m_avatarCoin
    getCurCharacInvenW()->set_money(money);
    getCurCharacInvenW()->SetCoin(coin);
    getCurCharacInvenW()->SetPayCoin(payCoin);
    getCurCharacInvenW()->SetEventCoin(eventCoin);
    getCurCharacInvenW()->SetInvenData(1, inven1, inven1Size);
    getCurCharacInvenW()->SetInvenData(2, inven2, inven2Size);
    memcpy(reinterpret_cast<char*>(getCurCharacInvenW()) + 0x1c, equipMirror,
           equipMirrorSize);
    memcpy(reinterpret_cast<char*>(getCurCharacInvenW()) + 0x55a, inven3 + 0x38b6, 0xf4);
    for (int i = 0x17; i < 0x1a; ++i) {
        // ORIG 0x0864d3f7 `lea 0xc(%eax),%ebx`：LoadArtifact 第三实参（元素指针）先物化
        // %ebx 并跨两次 receiver 调用保活 ⇒ 单表达式链式（P52），禁具名 invenW/mgr（旧稿两槽）。
        getCurCharacInvenW()->GetCreatureMgrW()->LoadArtifact(i,
            reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(getCurCharacInvenW()) + i * 0x3d + 0x1c));
    }
    getCurCharacInvenW()->SetInvenData(3, inven3, inven3Size);
    if (cargoCapacity == 0) {
        getCurCharacCargoW()->SetCapacity(8);
    }
    else {
        getCurCharacCargoW()->SetCapacity(cargoCapacity);
        getCurCharacCargoW()->set_cargo(cargo);
    }
    return 1;
}