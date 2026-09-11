// ---- ORIG T 0x089c1cb9：动画层提交（normal/level/job 三层 + def 模板逐槽拼接） ----
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

#define SETANI_SLOT(member)                                                    \
    do                                                                         \
    {                                                                          \
        /* ORIG 0x89c1e63..：格式串 = def.member（脚本模板），参数 = path */     \
        snprintf(buf, sizeof(buf), def.member.c_str(), path->c_str());         \
        ani->member = buf;                                                     \
    } while (0)

    // ORIG 拼接顺序（88c1e63..25c2）：…2c、3c、30、40、34、44、38、48 交错。
    SETANI_SLOT(m_str04);
    SETANI_SLOT(m_str08);
    SETANI_SLOT(m_str0c);
    SETANI_SLOT(m_arr10[0]);   // +0x10
    SETANI_SLOT(m_arr10[1]);   // +0x14
    SETANI_SLOT(m_str18);
    SETANI_SLOT(m_str1c);
    SETANI_SLOT(m_str20);
    SETANI_SLOT(m_str24);
    SETANI_SLOT(m_str28);
    SETANI_SLOT(m_arr2c[0]);   // +0x2c
    SETANI_SLOT(m_arr3c[0]);   // +0x3c
    SETANI_SLOT(m_arr2c[1]);   // +0x30
    SETANI_SLOT(m_arr3c[1]);   // +0x40
    SETANI_SLOT(m_arr2c[2]);   // +0x34
    SETANI_SLOT(m_arr3c[2]);   // +0x44
    SETANI_SLOT(m_arr2c[3]);   // +0x38
    SETANI_SLOT(m_arr3c[3]);   // +0x48
    SETANI_SLOT(m_str4c);
    SETANI_SLOT(m_str50);
    SETANI_SLOT(m_str54);
    SETANI_SLOT(m_str58);
    SETANI_SLOT(m_str5c);
#undef SETANI_SLOT

    // ORIG 0x089c25c7：迭代器先默认构造（`__normal_iterator::C1Ev`，P50 复活）
    // 再 `it = begin()`（0x089c25cd C1Ev → 0x089c25e5 beginEv → 0x089c25f3 赋值）；
    // 两个循环**复用同一 it 槽 -0x48**（P160 复用槽律）。
    std::vector<std::string>::const_iterator it;
    it = def.m_vec64.begin();
    for (; it != def.m_vec64.end(); ++it)
    {
        // ORIG 0x89c2601..0x89c2634：格式串 = it 元素，参数 = path
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
        ani->m_vec64.push_back(std::string(buf));
    }
    it = def.m_vec70.begin();
    for (; it != def.m_vec70.end(); ++it)
    {
        // ORIG 0x89c272b..0x89c275e：同上
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
        ani->m_vec70.push_back(std::string(buf));
    }
}