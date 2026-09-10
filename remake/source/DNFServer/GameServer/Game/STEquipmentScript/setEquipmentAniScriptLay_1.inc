void setEquipmentAniScriptLay(
    const std::map<std::string, STEquipmentAniScript>* table,
    STEquipmentAniScript* ani, std::string* key, int layerId,
    std::string* path, STEquipmentScript* script,
    ENUM_CHARACTERJOB job, equipmentAniType::T aniType)
{
    // [SweepSTEquip 2026-09-09] 0x089c1cb9：ORIG 774 → 我方 772（gap 44→2）。
    // 本轮四处修复：① `find(key->c_str())` 隐式构造临时 string（0x089c1cec..d02）；
    // ② `m_level5c.rbegin()->push_back(*ani)` 语义纠错（ORIG ptEv + 内层 push_back，
    //   旧译文 deEv + 外层 push_back 把 ani 追加错层）；③ 迭代器先默认构造再
    //   `it = begin()`（P50/P314 复活，0x089c25c7 C1Ev）；④ 两循环复用同一 it 槽
    //   （P160）。p167 147=147 CLEAR（仅余 C1↔C2/D1↔D2 同址别名命名差）；残余
    //   2 条为帧差（0x25c vs ORIG 0x26c，N126/N115）。
    // [SweepSTEquipScript 2026-09-09 重判] 两处修正：①补 `if (ani->m_field0 != 0)`
    //   守卫（ORIG 0x089c1d7c..d83 `mov 0xc(%ebp),%eax; mov (%eax),%eax; test; je`，
    //   仅层号非 0 才提交；旧译文无条件 push_back ⇒ 真语义缺口，Ghidra C 同形）；
    //   ②T_level/else 删具名 `info` 引用、直用 addAniInfo 返回值（ORIG 0x089c1e09
    //   `lea 0x18(%eax),%edx` / 0x089c1e2a `mov %eax,(%esp)`，P0）。
    //   ③`ani->m_field0` 赋值由三目改 if/else（ORIG 0x089c1e49 `cmpl $0; je 0x1a0`
    //   是 layerId 分支在前、def 后置，P47 块排布镜像）。读数 ORIG 774 / 我方 774
    //   （gap 2→0；TU near 2→3 / diff 15→14，DIFF→NEAR）。
    // ORIG 用 `lea -0x24c(%ebp),%ebx; mov $0x0,%eax;
    // mov $0x80,%edx; rep stos %eax,%es:(%edi)`（0x80 dwords = 512B）⇒ 原码是
    // `char buf[0x200] = {0};`（显式零初始化触发 4.4 rep stos）；旧 `memset` 生 call。
    char buf[0x200] = {0};
    // ORIG 0x089c1cec..0x089c1d02：`key->c_str()` → `string::C2(const char*, allocator)`
    // 构造**临时 string** 再传 find（隐式转换），非直接传 *key 引用（p167 MISSING
    // string::C2PKcRKSaIcE / ~string / allocator::C2Ev 各 1）。
    const STEquipmentAniScript& def = table->find(key->c_str())->second;

    // ORIG 0x089c1d7c..0x089c1d83：`mov 0xc(%ebp),%eax; mov (%eax),%eax; test %eax,%eax;
    // je 0x185` ⇒ 三分支提交整体被 `if (ani->m_field0 != 0)` 守卫（层号 0 = 无效，
    // 不提交；Ghidra C 同形）。旧译文无条件提交 ⇒ 真语义缺口。
    if (ani->m_field0 != 0)
    {
    // [b28d-solo-STEquipmentScript] p167：ORIG 三个分支各有独立 addAniInfo 调用点
    // （MISSING addAniInfo×2），不是分支前统一取一次。
    if (aniType == equipmentAniType::T_job)
    {
        EquipmentAniInfoScript& info = script->addAniInfo(job);
        if (!info.m_level5c.empty())
        {
            // ORIG 0x089c1dcd..0x089c1dea：`rbegin()` → reverse_iterator::operator->
            // （ptEv）取**末个子 vector 的地址**，再对它 `push_back(ani)`；旧译文
            // 写成外层 `m_level5c.push_back(*rbegin())`（deEv）语义错（复制末子表
            // 而非把 ani 追加进末子表）——p167 MISSING ptEv / vector<AniScript>::
            // push_back，EXTRA deEv / vector<vector<>>::push_back。
            // [2026-09-10] ORIG 0x089c1dcd..0x089c1dea 把 rbegin() 的 sret 临时放在
            // 深槽 -0x4c（与 it -0x48 / find sret -0x44 同区）；匿名临时被 4.4 放浅槽
            // -0x3c 并把其余临时整体挤深 4（NEAR 的根因）⇒ 用**具名** reverse_iterator
            // 让 4.4 按具名对象分组分配。
            std::vector<std::vector<STEquipmentAniScript> >::reverse_iterator rit =
                info.m_level5c.rbegin();
            rit->push_back(*ani);
        }
    }
    else if (aniType == equipmentAniType::T_level)
    {
        // ORIG 0x089c1e09：`lea 0x18(%eax),%edx` 直接用 addAniInfo 返回值 ⇒ 无具名
        // 引用局部（P0；具名版多 store+load 两条）。
        script->addAniInfo(job).m_ani18.push_back(*ani);
    }
    else
    {
        // ORIG 0x089c1e2a：同上（`mov %eax,(%esp)` 直传返回值）。
        script->addAniInfo(job).m_ani0.push_back(*ani);
    }
    }

    ani->clear();
    // ORIG 0x089c1e49 `cmpl $0x0,0x14(%ebp); je 0x1a0`（layerId 分支在前、
    // def 分支后置）⇒ 源形是 if/else 而非三目（P47 块排布镜像）。
    if (layerId != 0)
        ani->m_field0 = layerId;
    else
        ani->m_field0 = def.m_field0;
    ani->m_str60 = path->c_str();
