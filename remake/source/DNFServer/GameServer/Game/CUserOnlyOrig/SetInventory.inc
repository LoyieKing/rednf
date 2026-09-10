bool CUser::SetInventory(SIG_LOAD_INVENTORY* sig)
{
    unsigned int money = sig->m_money;
    unsigned int coin = sig->m_coin;
    unsigned int payCoin = sig->m_payCoin;
    unsigned int eventCoin = sig->m_eventCoin;
    char* inven1 = (char*)sig->m_invenType1;
    char* inven2 = (char*)sig->m_invenType2;
    char* equipMirror = (char*)sig->m_equipMirror;
    char* cargo = (char*)sig->m_cargo;
    int cargoCapacity = static_cast<int>(sig->m_cargoCapacity);
    char* inven3 = (char*)sig->m_invenType3;
    if (getCurCharacR() == 0) {
        // [FL5 2026-09-10 负结论·已回滚] P37/P227 链在本件 ndiff 317→317 中性 ⇒ 维持具名形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1271, 5);
        trace("CUser::SetInventory m_selected is null");
        return 0;
    }
    for (int i = 0; i < 0xc; ++i) {
        const CInventory* invenR = getCurCharacInvenR();
        // [A06 2026-09-10 C05④/P225 试行记录·已回退] ORIG 0x0864d1a4 段为
        // `cmp %eax,%edx; setne %al; test %al,%al; je <T>`；按 P225 改 `while (cond) { body; break; }`
        // 实测**无效**（227/238→227/239、delta 268→267），且本处两侧均非具名局部
        // （元素字段 vs 镜像字段）⇒ 命中 Main 转发的 C05② 边界（仅「两个具名局部比较」命中）⇒ 回退。
        if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(invenR) + i * 0x3d + 0x1e) !=
            *reinterpret_cast<int*>(equipMirror + i * 0x3d + 2)) {
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
    getCurCharacInvenW()->SetInvenData(1, inven1, 0x4a58);
    getCurCharacInvenW()->SetInvenData(2, inven2, 0x1905);
    memcpy(reinterpret_cast<char*>(getCurCharacInvenW()) + 0x1c, equipMirror, 0x53e);
    memcpy(reinterpret_cast<char*>(getCurCharacInvenW()) + 0x55a, inven3 + 0x38b6, 0xf4);
    for (int i = 0x17; i < 0x1a; ++i) {
        // ORIG 0x0864d3f7 `lea 0xc(%eax),%ebx`：LoadArtifact 第三实参（元素指针）先物化
        // %ebx 并跨两次 receiver 调用保活 ⇒ 单表达式链式（P52），禁具名 invenW/mgr（旧稿两槽）。
        getCurCharacInvenW()->GetCreatureMgrW()->LoadArtifact(i,
            reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(getCurCharacInvenW()) + i * 0x3d + 0x1c));
    }
    getCurCharacInvenW()->SetInvenData(3, inven3, 0x39aa);
    if (cargoCapacity == 0) {
        getCurCharacCargoW()->SetCapacity(8);
    }
    else {
        getCurCharacCargoW()->SetCapacity(cargoCapacity);
        getCurCharacCargoW()->set_cargo(cargo);
    }
    return 1;
