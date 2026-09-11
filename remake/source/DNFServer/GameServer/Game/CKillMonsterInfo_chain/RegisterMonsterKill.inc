// ---- RegisterMonsterKill(map&, mobIdx, diff, isAPC) —— ORIG T 0x0854d24c ---
// diff>4 → false；isAPC ? CAICharacterList::get==0 拒绝 : find_monster==0 拒绝；
// 已有记录 → flag |= diffFlag 并 alter()（charac_expand::CData::m_bIsSave=1）；
// 否则 insert(make_pair(idx,diffFlag))，插入失败记日志返回 false，成功 alter()。
// [b28d-solo-CKillMonsterInfo_chain 判词 · NEAR/N93] ORIG 0x0854d24c（129 条）：
//  · 已对齐：`if (!isAPC)` 极性（ORIG `movzbl isAPC; xor $1; test; je <APC>`）、
//    两校验结果具名槽（-0x10 find_monster / -0xc get + `cmpl $0x0,槽; jne`，
//    P0 槽往返）、两成功路径共享尾 `mov $1,%eax`、make_pair 右值 uchar、
//    单次 operator->（P167 CLEAR 16=16）。
//    ORIG 0854d325 `movzbl 0x4(%ebx),%esi` 单段直落亦已复现——复合赋值 RHS 写
//    `(unsigned char)GetDungeonDifficultyFlag(diff)` 即消 `mov %eax,%esi` 中转
//    （P248 档② 同族，129=129）。
//  · 残余（N93/N98 封顶）：仅 `it`(ORIG -0x34/-0x30) 与 `ins`(ORIG -0x2c/-0x28)
//    槽位互换——4.1 按声明序先声明深，4.4 按首次使用序先使用浅；it 的 find 必
//    先于 ins 的 insert，声明序翻转无效（N98），块内提头部需 ins 默认构造 ⇒
//    多 pair C1Ev call + 8 字节拷贝（违反 P215/发明符号禁令）⇒ 结构性封顶。
// [L6S5 P326 推翻 N93/N98] 旧判词根因是我方把 insert 的 pair<it,bool> 结果
//   **具名化**（`ins` 占最深槽 -0x34，把 ORIG 的 it 挤到 -0x2c）。ORIG 实为
//   「it 具名 + end()/insert() 右值临时」：去掉具名 `ins`、把
//   `kills.insert(...).second` 内联进条件后，三匿名槽回归 ORIG 序
//   （find -0x34 / end -0x30 / insert -0x2c，bool 读 -0x28）⇒ 已 **IDENTICAL**。
// [L6S5 P139 + §6.5 签名轴] 父泳道在 CKillMonsterInfo.h 落 `typedef std::map<
//   unsigned int, unsigned char> killMonsterMap`（ORIG prett 串直证）后，定义签名
//   改 `killMonsterMap&`（+ size_t），两处 logFormat 用回 `__PRETTY_FUNCTION__`
//   ⇒ .o 生成 ORIG 实有的
//   `_ZZN16CKillMonsterInfo19RegisterMonsterKill…__PRETTY_FUNCTION__`（size 0x5f，
//   与 ORIG @0x08c9ec20 逐字节一致）。
bool CKillMonsterInfo::RegisterMonsterKill(
    killMonsterMap& kills,
    unsigned int mobIdx, unsigned char diff, bool isAPC)
{
    if (diff > 4)
        return false;

    // [b28d-solo-CKillMonsterInfo_chain] ORIG 0854d24c：`movzbl isAPC; xor $1;
    // test; je <APC 路径>` ⇒ 原码极性为 `if (!isAPC)`（find_monster 直落）。
    // 两个校验结果均落具名槽（`mov %eax,-0x10/-0xc(%ebp); cmpl $0x0,槽; jne`，
    // P0 槽往返 ⇒ 原码有具名局部）；直比调用结果会多 sete 物化。
    // [L6S6b 2026-09-11 残差 129/129 gap0 ndiff14] 真实差异仅 3 个匿名 sret 槽的
    //   相对位置：ORIG find=-0x34/end=-0x30/insert(pair<it,bool>)=-0x2c（bool 读
    //   -0x28）vs 我方 find=-0x2c/end=-0x28/insert=-0x34（bool 读 -0x30）——即
    //   「it 深/max 浅」与「ins 深」两方向相反（4.1 与 4.4 的匿名临时分配方向差，
    //   N93/N82 族；与 box_item_index 的方向恰好相反，说明不可用统一声明序旋钮）。
    //   具名化/复制形会多 2~4 条 ⇒ 判 N 封顶，不再派单。
    if (!isAPC)
    {
        void* pMon = G_CDataManager()->find_monster(static_cast<int>(mobIdx));
        if (pMon == 0)
            return false;
    }
    else
    {
        CAICharacter* pAI = G_CDataManager()->m_aiCharacterList->get(mobIdx);
        if (pAI == 0)
            return false;
    }

    // [b28d-7 P167] ORIG 无具名 flag 局部：两分支各自内联
    // GetDungeonDifficultyFlag(diff)（ORIG 2 次 vs 旧版 1 次），make_pair 第二参
    // 为 uchar 右值（0x0854d2xx 符号 `make_pair<unsigned int&, unsigned char>`），
    // 且记录分支用 `(*it).second`（ORIG 无 operator->，用 operator*）。
    std::map<unsigned int, unsigned char>::iterator it = kills.find(mobIdx);
    if (it != kills.end())
    {
        // ORIG 记录分支只有 1 次 operator->（复合赋值单次取址），
        // 旧版读+写两次 it->second 多 1 个调用点。
        it->second |= (unsigned char)GetDungeonDifficultyFlag(diff);
        alter();
    }
    else
    {
        // [b28d-solo-CKillMonsterInfo_chain] ORIG 两成功路径共享尾 `mov $1,%eax`
        // （记录分支 `call alter; jmp +0x1bd`，插入成功分支 fall-through）⇒
        // 尾部单一 return true，分支内不得各自 return。
        // [L6S5 P326/P183] ORIG 0854d24c：insert 的 pair<it,bool> 结果**无具名局部**
        //   （ORIG 具名槽仅 it=-0x34，end 临时 -0x30，insert 临时 -0x2c / bool 读
        //   -0x28）；旧版具名 `ins` 占最深槽并把 it 挤到 -0x2c ⇒ 三槽整体错位。
        if (!kills.insert(std::make_pair(
                 mobIdx,
                 static_cast<unsigned char>(GetDungeonDifficultyFlag(diff)))).second)
        {
            LogManager::logFormat(
                1, "KillMonsterInfo.cpp",
                __PRETTY_FUNCTION__,
                199, "KillMonsterInfo::RegisterMonsterKill Fail(%u, %u)",
                mobIdx, static_cast<unsigned int>(diff));
            return false;
        }
        alter();
    }
    return true;
}