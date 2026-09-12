// ============================================================================
// importEquipmentParameterInfo —— ORIG 0x089a75ea（_Z28importEquipmentParameterInfo...
//   PKcRiS4_，单 token 分派，非文件加载）。param_2(path) 为当前关键字 token；
//   param_3(dir) 供 [then]/[multiple then] 递归 scanThen(dir,...) 用；
//   param_4(a)/param_5(b) 为 [if]/[then] 嵌套深度计数。每个命中分支写对应
//   具名字段并消费 token；结尾对非 [if]/[then] token 调 scanRandomOptionValue
//   （ORIG 经 param_1+0x434 子指针，字段未建模则跳过），返回 1。
//   本版覆盖已具名字段关键字；其余关键字落入结尾（返回 1 不中断脚本）。
// ============================================================================
// [b28d-solo-EquipmentParameterInfo] N（结构差封顶）：本波 p167 **MISSING 清零**
// （LevelUpSkill lu 循环外构造+循环内 clear、push_backEOi 右值直传、0x19d6/0x19f4 拆成
// 两个独立分支各一次 getActiveStatusToEnumValue）；残余 EXTRA string D1×6/ScanInt×6/
// operator==×5 等 = switch 后死 if 链等历史结构差（P165 结构还原未做）。5088 vs 4801。
// ============================================================================
// [BigEqParamTail 2026-09-09] 大件专项（4801 指令）逐段收敛：改前 4800/ORIG 4801
// （gap +1，slot 归一后结构差 **90 块**）→ 改后 4837/4801（gap −36，结构差 **43 块**）。
// 已修段（每段 ORIG 地址 → 类型 → 修法）：
//  ① 0x89a7842 switch 决策树含未列 case 0 → 补 `case 0:`（P163）⇒ 二分树/范围逐点对齐。
//  ② 0x89a8249 [if]：i 无槽（edx 直传）→ `scanIf(info, a++)` 实参内联（P0）。
//  ③ 0x89a82d8/0x89a8303/0x89a83b1 [then]：ORIG `jg <then>`+else 直落 → 正向包裹
//     `if (a<=b){a=-a;b=-b;return false;}`（P222）+ `b++` 实参内联（P0）+
//     `if (!scanThen(...)) return false;` 负极性（P168a X 族）⇒ 逐条同形。
//  ④ 0x89abdeb..0x89abf7a [active status control info]：10 分支恢复具名 int
//     v0..v13（P0 槽往返，ORIG -0xa4..-0x70 逐槽；含未使用值仍落槽）。
//  ⑤ 0x89a89da [recovery status]：`for(;;){v2=ScanFloat(&ok); if(!ok) break; …}` +
//     ok 声明先于 i（P164/P283 循环旋转、P212 声明序）。
//  ⑥ 0x89ab366/0x89aaacd/0x89aabba：三处三目改 if/else 两段直写成员（P47）。
//  ⑦ 0x89ac54e/0x89ac5cb：importAllSkillItem/importItemOverpowerDesc int→bool
//     （P168a 档①；调用点 `call; xor $1; test; je` 直证）。
//  ⑧ 0x89abaa3/0x89abbd9 [active status probability/level]：`for(j=0;j<=0x11;++j)
//     if(st==j) arr[j]=val;` 循环查找形 + 删冗余 st 范围守卫（P51/P175）。
//  ⑨ 真语义修复（0x89ace7e 共享块）：default 与 [variable stat] insert 后统一
//     `goto L_unknown` ⇒ `if (__inited) return false;`——原实现 break 直达
//     chain_end，已初始化 + 未知 token 会错误返回 true。
// [ImpBig2 2026-09-12] 续修（BigP3A 之后）：助记符级结构块 38→26，gap +50 → **+23**
//   （指令 4873→4846，size 0x5a23→0x59ec）；逐行相似度 83.5% → **84.3%**
//   （difflib ndiff 1143 → 1056，差异块 495 → 471）。
//  ⑩ 真语义修复（8 处）：745/755/805/851/868/1200/1234/1270 各删去多余的
//     `if (ScanStr(&s))` 检查 —— ORIG 对应调用点（0x89a9e6b/0x89a9efd/0x89aa50f/
//     0x89aaab7/0x89aaba4/0x89aba2f/0x89abb65/0x89abc9b）的 `call _Z7ScanStrPSs`
//     之后**无** test/je，直接续 c_str/getActiveStatusToEnumValue/operator==
//     （P140/P349：ORIG 无防御时不得添加）。删后 15 个 ScanStr 调用点与 ORIG 一一
//     配对（verify_final.py）：**4 点后续 10 条逐条全等**、10 点助记符序列全等
//     （仅槽号/立即数值差）、1 点（0x89ac9e3=[decrease critical damage]）为已知
//     N85 族。−16 条（+连锁 −29）。
//  ⑪ `aura.m_field4 = (s == "%") ? 1 : 0;` → 直接赋值 `= (s == "%");`
//     （ORIG 0x89aacdc `call operator==; mov %al,-0xa30(%ebp)` 单条直存，
//     无 `mov $1/mov $0` 物化）。−3 条。
//  ⑫ 函数级共享标量临时还原（[推断]·P328④）：`int v = 0;`（ORIG 0x89a7604
//     `movl $0x0,-0xa10(%ebp)`，紧随 s 构造）⇒ [separate attack]（0x89ac3a5）与
//     [repair price]（0x89ac44f）两循环改「局部槽 + 两步寻址写成员」= ORIG 同形；
//     并修正块级槽相位 −4（`-0xd4` → **`-0xd8`** = ORIG）。
//     反例（勿重试）：[separate rate]（ORIG 0x89ac400 用 -0xa60）若改用函数级
//     `float fv` 承接 ⇒ 4.4 因该**使用点**重排函数级槽区，s 由 -0xa0c → -0xa08
//     （65 处引用全体漂移）⇒ 逐行相似度 84.3%→67.9%、ndiff 1056→2152 **反劣**，
//     已回退；该段形态差归 N82 槽相位族。
// 剩余 26 块判词（ORIG 地址区间 → 类型 → 依据）：
//  · 0x89a8421..0x89a85c1（[level linear ability] 入口）：ORIG 两个 string 局部
//    （-0xa18 构造 0x89a842a/析构 0x89a85b8 = **构造析构但从未使用**的遗留局部，
//    P328④；-0xa1c = 循环变量 t）+ 完整 EH landing/回滚段；我方 4.4 单 string(-0xa14)
//    + 不同 EH 布局 ⇒ N91/N104 封顶（补第二个未使用 string 需重排整段 EH，风险 > 收益）。
//  · 0x89abfe7..0x89ac04a 注册链插入段与下个 case 标签的相对位置：指令集合一致，
//    仅 EH 段长度造成位置错位（difflib 伪差）⇒ N104。
//  · 0x89ab66a/0x89ac72b/0x89ac808（3 处 push_back 后）：我方多
//    `~STAvatarAbilityVariation` ×2 + `~EquipmentAuraAppendageData` ×4 调用（+正常
//    路径与 EH landing 各一）—— ORIG 两个 D1 符号 size=0x5（空体 push/mov/pop/ret，
//    nm 实证）且本函数区间内调用数 0；4.1 判 trivial、4.4 判 non-trivial（P71）。
//    空体 + 成员全 POD ⇒ 调用与否不可观察等价；删用户空析构会丢 D1/D2 与
//    new_allocator::destroy 发射链（A/B 实测 CEquipItem identical 191→190）⇒ 维持。
//  · 0x89ac9e8..0x89aca18：ORIG 先算 `float flag=(s=="%")?1.0f:0.0f`(-0x20) 再
//    ScanInt，比较用 x87 `fucompp/fnstsw/sahf`；我方求值序相反且用整数比较 ⇒
//    N85 封顶（4.4 对 float!=0 发 fucomip/fstp，源码层不可择）。
//  · 0x89acab6/0x89acbca/0x89acbde：float 比较物化 + EH 段布局 ⇒ N85/N104。
//  · 0x89acbe6..0x89acd1e（[rebirth] 子标签循环）：ORIG `ScanType` 条件在环头
//    （`xor$1;test;je;` + 失败 jmp）+ 尾 nop 岛；4.4 恒「体在前、尾测 jne」⇒
//    跨版本循环 lowering（P25/N104）。
//  · 0x89ace9c/0x89aceb8：L_chain_end 的 jmp/nop 中继条数（我方多 3 组 = 6 条）
//    ⇒ N112 对齐填充 + 尾岛（N121）。
//  · 0x89a77fb/0x89acbde 等 nop：对齐填充（§6.1 豁免）。
//  · 0x89ac400..0x89ac433（[separate rate]）：ORIG 局部槽 -0xa60 + 两步寻址，
//    我方直传成员地址 ⇒ N82 槽相位族（⑫ 反例：改写反劣，已回退）。
//  · 0x89ac11c/0x89ac16b/0x89ac25d/0x89ac2af：两循环写 +0x10/+0xc 的语句顺序相反
//    （ORIG 先 +0x10）⇒ P324 叶块序族，改动连锁槽位，封 N。
//  · 0x89ac33e..0x89ac456：ORIG 用 for 循环写数组（回边 + setle 尾测），我方 4.4
//    展开为直写 ⇒ P165「循环抽象 vs 物理展开」封 N。
//  —— 已闭合（本轮）：缺 `movl $0,-0xa10`（⑫ 修）+ 块级槽相位 −4（⑫ 修）+
//     8 处 ScanStr 冗余检查（⑩ **真语义差异**）+ [separate attack] 形态（⑫）。
// 验收（check_tu_routed fresh，ImpBig2 后）：identical=446 / ae=1011 / near=3 /
//   diff=13 / missing_in_game=312——与本轮改动前**逐值相同**（零回退）；
//   p167 调用点：ORIG 609 vs 我方 615（差 6 = 2×STAvatar D1 + 4×Appendage D1 = P71
//   豁免帧）；MISSING 侧全部为 C2/D2 别名对（P93 同址，非缺口）。
// ============================================================================
bool importEquipmentParameterInfo(EquipmentParameterInfo* info,
                                  std::string& path, const char* dir,
                                  int& a, int& b)
{
    // [ORIG 结构还原] 0x89a75ea：与 readAction/readCondtion 同形 —— 函数级静态
    // 查表 + Duff 式惰性注册链（106 项），每 case 头一个 guard+insert+goto 链，
    // 链尾（ORIG 0x58d1）leave+置位跳回重查；建表后 body 走共享尾。
    std::string s;
    // [推断·P328④] ORIG 0x89a7604 `movl $0x0,-0xa10(%ebp)`（紧随 s 构造之后）
    // 实证函数级共享标量临时：被 [separate attack]（0x89ac3a5 `lea -0xa10` →
    // `_Z7ScanIntPi`）与 [repair price]（0x89ac44f 同槽）复用。
    int v = 0;
    int iv = 0;
    float fv;
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    int id;
 relaunch:
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(path.c_str());
        id = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();
        if (__inited)
        {
            __init_lock.leave();
            goto relaunch;
        }
        id = 0;
    }
    switch (id)
    {
    case 0:                     // P163：ORIG 0x89a7842 树含未列 case 0
    case 0x176d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[HP MAX]", 0x176d));
            goto L_1770;
        }
    {
        info->m_hpMax= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1770:
    L_1770:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[HP MAX rate]", 0x1770));
            goto L_1773;
        }
    {
        info->m_hpMaxRate= ScanFloat(1, (bool*)0); goto L_chain_end;
    }
    case 0x1773:
    L_1773:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[MP MAX]", 0x1773));
            goto L_1776;
        }
    {
        info->m_mpMax= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1776:
    L_1776:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[MP MAX rate]", 0x1776));
            goto L_1779;
        }
    {
        info->m_mpMaxRate= ScanFloat(1, (bool*)0); goto L_chain_end;
    }
    case 0x1779:
    L_1779:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[physical attack]", 0x1779));
            goto L_177c;
        }
    {
        info->m_physicalAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x177c:
    L_177c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[physical defense]", 0x177c));
            goto L_177f;
        }
    {
        info->m_physicalDefense= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x177f:
    L_177f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[magical attack]", 0x177f));
            goto L_1782;
        }
    {
        info->m_magicalAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1782:
    L_1782:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[magical defense]", 0x1782));
            goto L_1785;
        }
    {
        info->m_magicalDefense= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1785:
    L_1785:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[if]", 0x1785));
            goto L_1788;
        }
    {
        // ORIG 0x89a8249：i 无栈槽（eax→edx 直传）⇒ 原码是实参内联 `a++`（P0）。
        scanIf(info, a++); goto L_chain_end;
    }
    case 0x1788:
    L_1788:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[then]", 0x1788));
            goto L_1795;
        }
    {
        // ORIG 0x89a82d8 `jg <then>`：else 直落、then 为跳转目标 ⇒ 正向包裹（P222/P59）。
        if (a <= b)
        {
        a = -a;
        b = -b;
        return false;
        }
        // ORIG 0x89a8303：i 无栈槽（%esi 跨 string 构造保活）⇒ 实参内联 `b++`（P0）；
        // 0x89a83b1 `test %al,%al; je <成功尾>`（esi=!ret，X 族负极性）。
        if (!scanThen(dir, info, b++)) return false;
        goto L_chain_end;
    }
    case 0x1795:
    L_1795:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[multiple then]", 0x1795));
            goto L_17a5;
        }
    {
        // ORIG 0x89a83c1：ScanType 流读到 EOF/失败或 "[/multiple then]" 时
        // b(=*param_5)+1 并成功返回；读到 "[then]" 时以当前 b 调 scanThen
        //（b 不递增），失败返回 false；其它 token 忽略继续读。
        std::string t;
        for (;;)
        {
        if (!ScanType(t, true))
        {
        b = b + 1;
        goto L_chain_end;
        }
        if (t == "[/multiple then]")
        {
        b = b + 1;
        goto L_chain_end;
        }
        if (t == "[then]")
        {
        if (!scanThen(dir, info, b)) return false;
        }
        }
    }
    case 0x17a5:
    L_17a5:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[equipment physical attack]", 0x17a5));
            goto L_17a9;
        }
    {
        info->getEquipmentPhysicalAttack(0)= ScanInt((bool*)0);
        info->getEquipmentPhysicalAttack(1)= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17a9:
    L_17a9:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[equipment physical defense]", 0x17a9));
            goto L_17ad;
        }
    {
        info->getEquipmentPhysicalDefense(0)= ScanInt((bool*)0);
        info->getEquipmentPhysicalDefense(1)= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17ad:
    L_17ad:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[equipment magical attack]", 0x17ad));
            goto L_17b1;
        }
    {
        info->getEquipmentMagicalAttack(0)= ScanInt((bool*)0);
        info->getEquipmentMagicalAttack(1)= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17b1:
    L_17b1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[equipment magical defense]", 0x17b1));
            goto L_17b6;
        }
    {
        info->getEquipmentMagicalDefense(0)= ScanInt((bool*)0);
        info->getEquipmentMagicalDefense(1)= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17b6:
    L_17b6:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[additional effect index]", 0x17b6));
            goto L_17bb;
        }
    {
        // [b28d-solo-EquipmentParameterInfo] ORIG 0x89a88f8 调 push_backEOi（右值引用版：
        // `call ScanInt; mov %eax,-0x8a8; lea -0x8a8; call EOi`）⇒ 实参是右值表达式；
        // 旧实现经具名 iv 传左值 ⇒ 选 push_backRKi（p167 MISSING EOi ×1）。
        { info->m_vec194.push_back(ScanInt((bool*)0)); } goto L_chain_end;
    }
    case 0x17bb:
    L_17bb:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[recovery status]", 0x17bb));
            goto L_17cc;
        }
    {
        // ORIG 0x89a8902：清空 +0x240 vector<vector<float>>，ScanInt 次数 resize；
        // 循环 ScanFloat（bool 成功出参）：v 追加到第 (i % n) 个子向量并递增 i。
        info->m_vec240.clear();
        int n = ScanInt((bool*)0);
        info->m_vec240.resize(n);
        // ORIG 0x89a89da：ok(-0xa1d) 声明先于 i(-0xf8)；0x89a89fc 头部
        // `movzbl ok; xor $1; test; je <exit>` + 0x89a8a49 回跳头部 ⇒ for(;;)+break
        // 复现 4.1 循环旋转（P164/P283）。
        bool ok = false;
        int i = 0;
        float v2;
        for (;;)
        {
        v2 = ScanFloat(&ok);
        if (!ok) break;
        info->m_vec240[i % n].push_back(v2);
        ++i;
        }
        goto L_chain_end;
    }
    case 0x17cc:
    L_17cc:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[separate attack]", 0x17cc));
            goto L_17d0;
        }
    {
        info->m_separateAttack[0]= ScanInt((bool*)0);
        info->m_separateAttack[1]= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17d0:
    L_17d0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[separate rate]", 0x17d0));
            goto L_17e6;
        }
    {
        // ORIG 0x89a8aea：两个 ScanFloat 无检查写 +0x390/+0x394（float 位模式）。
        info->m_separateRate[0] = ScanFloat((bool*)0);
        info->m_separateRate[1] = ScanFloat((bool*)0);
        goto L_chain_end;
    }
    case 0x17e6:
    L_17e6:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[physical absolute damage]", 0x17e6));
            goto L_17e9;
        }
    {
        info->m_physicalAbsoluteDamage= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17e9:
    L_17e9:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[physical damage reduce]", 0x17e9));
            goto L_17ec;
        }
    {
        info->m_physicalDamageReduce= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17ec:
    L_17ec:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[physical absolute defense]", 0x17ec));
            goto L_17ef;
        }
    {
        info->m_physicalAbsoluteDefense= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x17ef:
    L_17ef:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[magical absolute damage]", 0x17ef));
            goto L_17f2;
        }
    {
        info->m_magicalAbsoluteDamage= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17f2:
    L_17f2:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[magical damage reduce]", 0x17f2));
            goto L_17f5;
        }
    {
        info->m_magicalDamageReduce= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17f5:
    L_17f5:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[magical absolute defense]", 0x17f5));
            goto L_17f8;
        }
    {
        info->m_magicalAbsoluteDefense= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x17f8:
    L_17f8:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[fire resistance]", 0x17f8));
            goto L_17fb;
        }
    {
        info->m_fireResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17fb:
    L_17fb:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[water resistance]", 0x17fb));
            goto L_17fe;
        }
    {
        info->m_waterResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x17fe:
    L_17fe:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[dark resistance]", 0x17fe));
            goto L_1801;
        }
    {
        info->m_darkResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1801:
    L_1801:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[light resistance]", 0x1801));
            goto L_1804;
        }
    {
        info->m_lightResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1804:
    L_1804:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[all elemental resistance]", 0x1804));
            goto L_1807;
        }
    {
        info->m_allElementalResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1807:
    L_1807:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[slow resistance]", 0x1807));
            goto L_180a;
        }
    {
        info->m_slowResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x180a:
    L_180a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[freeze resistance]", 0x180a));
            goto L_180d;
        }
    {
        info->m_freezeResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x180d:
    L_180d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[poison resistance]", 0x180d));
            goto L_1810;
        }
    {
        info->m_poisonResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1810:
    L_1810:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stun resistance]", 0x1810));
            goto L_1813;
        }
    {
        info->m_stunResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1813:
    L_1813:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[curse resistance]", 0x1813));
            goto L_1816;
        }
    {
        info->m_curseResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1816:
    L_1816:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[blind resistance]", 0x1816));
            goto L_1819;
        }
    {
        info->m_blindResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1819:
    L_1819:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[lightning resistance]", 0x1819));
            goto L_181c;
        }
    {
        info->m_lightningResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x181c:
    L_181c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stone resistance]", 0x181c));
            goto L_181f;
        }
    {
        info->m_stoneResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x181f:
    L_181f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[sleep resistance]", 0x181f));
            goto L_1822;
        }
    {
        info->m_sleepResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1822:
    L_1822:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[burn resistance]", 0x1822));
            goto L_1825;
        }
    {
        info->m_burnResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1825:
    L_1825:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[weapon break resistance]", 0x1825));
            goto L_1828;
        }
    {
        info->m_weaponBreakResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1828:
    L_1828:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[bleeding resistance]", 0x1828));
            goto L_182b;
        }
    {
        info->m_bleedingResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x182b:
    L_182b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[all activestatus resistance]", 0x182b));
            goto L_182e;
        }
    {
        info->m_allActiveStatusResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x182e:
    L_182e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[piercing resistance]", 0x182e));
            goto L_1831;
        }
    {
        info->m_piercingResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1831:
    L_1831:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stuck resistance]", 0x1831));
            goto L_1834;
        }
    {
        info->m_stuckResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1834:
    L_1834:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[confuse resistance]", 0x1834));
            goto L_1837;
        }
    {
        info->m_confuseResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1837:
    L_1837:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[hold resistance]", 0x1837));
            goto L_183a;
        }
    {
        info->m_holdResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x183a:
    L_183a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[armor break resistance]", 0x183a));
            goto L_183d;
        }
    {
        info->m_armorBreakResistance= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x183d:
    L_183d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[inventory limit]", 0x183d));
            goto L_1840;
        }
    {
        info->m_inventoryLimit= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1840:
    L_1840:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[HP regen speed]", 0x1840));
            goto L_1843;
        }
    {
        info->m_hpRegenSpeed= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1843:
    L_1843:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[MP regen speed]", 0x1843));
            goto L_1846;
        }
    {
        info->m_mpRegenSpeed= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1846:
    L_1846:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[MP regen speed rate]", 0x1846));
            goto L_1849;
        }
    {
        info->m_mpRegenSpeedRate= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1849:
    L_1849:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[move speed]", 0x1849));
            goto L_184c;
        }
    {
        info->m_moveSpeed= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x184c:
    L_184c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[x move dash speed]", 0x184c));
            goto L_184f;
        }
    {
        info->m_xMoveDashSpeed= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x184f:
    L_184f:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[y move dash speed]", 0x184f));
            goto L_1852;
        }
    {
        info->m_yMoveDashSpeed= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1852:
    L_1852:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attack speed]", 0x1852));
            goto L_1855;
        }
    {
        info->m_attackSpeed= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1855:
    L_1855:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[cast speed]", 0x1855));
            goto L_1858;
        }
    {
        info->m_castSpeed= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1858:
    L_1858:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[hit recovery]", 0x1858));
            goto L_185b;
        }
    {
        info->m_hitRecovery= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x185b:
    L_185b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[jump power]", 0x185b));
            goto L_185e;
        }
    {
        info->m_jumpPower= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x185e:
    L_185e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[elemental property]", 0x185e));
            goto L_1862;
        }
    {
        // ORIG 0x89a9e6b：`call ScanStr` 后直接 `lea;call c_str;call getElement`（无 test/je）
        // ⇒ 原码无成功检查（P140/P349）。本轮删除我方多加的返回值检查。
        ScanStr(&s); info->m_elementalProperty = getElement(s.c_str()); goto L_chain_end;
    }
    case 0x1862:
    L_1862:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status]", 0x1862));
            goto L_189e;
        }
    {
        ScanStr(&s); info->m_activeStatusType = getActiveStatusType(s);
        info->m_activeStatusValue= ScanFloat((bool*)0);
        info->m_activeStatusLevel= ScanInt((bool*)0);
        info->m_activeStatusFlag = (ScanInt((bool*)0) != 0);
        info->m_activeStatusValue2= ScanInt((bool*)0);
        // ORIG 0x89aa...（+0x158 附加参数）：状态名逐条 UNROLL，push_back 直接内联
        // ScanInt((bool*)0) 返回值（右值 → push_back(int&&)）；blind/slow/haste 各 2、
        // stone/confuse/armor break/weapon break/poison/lightning/bleeding 各 1、
        // burn 显式 4 次、bless/curse 用 for(<4) 循环（各 1 调用点），共 19 个 push_back。
        if (s == "[blind]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[slow]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[haste]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[stone]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[confuse]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[armor break]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[weapon break]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[poison]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[lightning]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[bleeding]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[burn]")
        { info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0));
        info->m_vec158.push_back(ScanInt((bool*)0)); info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[bless]")
        { for (int k = 0; k < 4; ++k) info->m_vec158.push_back(ScanInt((bool*)0)); }
        else if (s == "[curse]")
        { for (int k = 0; k < 4; ++k) info->m_vec158.push_back(ScanInt((bool*)0)); }
        goto L_chain_end;
    }
    case 0x189e:
    L_189e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[attacked active status]", 0x189e));
            goto L_18d7;
        }
    {
        // ORIG 0x899e...（0x189e 执行体）：状态附加 vector +0x17c 按状态名子键
        // 写入固定个数 int：blind/slow/haste→2，stone/confuse/armor break/
        // weapon break/poison/lightning/bleeding→1，burn/bless/curse→4，
        // 其它状态名不写。此处仅消费对应数量 ScanInt（ORIG 无成功检查）。
        ScanStr(&s); info->m_attackedActiveStatusType = getActiveStatusType(s);
        info->m_attackedActiveStatusValue= ScanFloat((bool*)0);
        info->m_attackedActiveStatusLevel= ScanInt((bool*)0);
        info->m_attackedActiveStatusFlag = (ScanInt((bool*)0) != 0);
        info->m_attackedActiveStatusValue2= ScanInt((bool*)0);
        // ORIG 0x899e...（+0x17c 附加参数）：状态名逐条 UNROLL，push_back 内联
        // ScanInt((bool*)0)（右值 → push_back(int&&)）；blind/slow/haste 各 2、
        // stone/confuse/armor break/weapon break/poison/lightning/bleeding 各 1、
        // burn/bless/curse 各用 for(<4) 循环（各 1 调用点），共 16 个 push_back。
        if (s == "[blind]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[slow]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[haste]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[stone]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[confuse]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[armor break]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[weapon break]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[poison]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[lightning]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[bleeding]")
        { info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[burn]")
        { for (int k = 0; k < 4; ++k) info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[bless]")
        { for (int k = 0; k < 4; ++k) info->m_vec17c.push_back(ScanInt((bool*)0)); }
        else if (s == "[curse]")
        { for (int k = 0; k < 4; ++k) info->m_vec17c.push_back(ScanInt((bool*)0)); }
        goto L_chain_end;
    }
    case 0x18d7:
    L_18d7:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add absolute damage]", 0x18d7));
            goto L_18e0;
        }
    {
        // ORIG 0x89aaacd/0x89aaae5：两段独立比较+直写成员（P47）。
        ScanStr(&s);
        {
        if (s == "[physical]") info->m_addAbsoluteDamageType = 0;
        else if (s == "[magical]") info->m_addAbsoluteDamageType = 1;
        }
        info->m_addAbsoluteDamageValue= ScanFloat((bool*)0);
        info->m_addAbsoluteDamageExtra= ScanInt((bool*)0);
        goto L_chain_end;
    }
    case 0x18e0:
    L_18e0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[add absolute defense]", 0x18e0));
            goto L_18e9;
        }
    {
        ScanStr(&s);
        {
        if (s == "[physical]") info->m_addAbsoluteDefenseType = 2;
        else if (s == "[magical]") info->m_addAbsoluteDefenseType = 3;
        }
        info->m_addAbsoluteDefenseValue= ScanFloat((bool*)0);
        info->m_addAbsoluteDefenseExtra= ScanInt((bool*)0);
        goto L_chain_end;
    }
    case 0x18e9:
    L_18e9:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[item aura]", 0x18e9));
            goto L_1941;
        }
    {
        // ORIG 0x899a...（0x18e9 执行体）：ScanStr 状态名→m_field0；
        // ScanStr "%"→m_field4（0x01）；ScanFloat→m_float8；ScanInt→m_fieldc，
        // 全部无成功检查；构造完成后 push 到 +0x188 vector。
        ItemAuraInfo aura;
        ScanStr(&s);
        aura.m_field0 = getChangeStatusType(s);
        ScanStr(&s);
        // ORIG 0x89aacdc：`call operator==; mov %al,-0xa30(%ebp)` —— 直接字节存、
        // 无 `mov $1/mov $0` 物化 ⇒ 原码为直接赋值（非三目）。
        aura.m_field4 = (s == "%");
        aura.m_float8 = ScanFloat((bool*)0);
        aura.m_fieldc = ScanInt((bool*)0);
        info->m_aura188.push_back(aura);
        goto L_chain_end;
    }
    case 0x1941:
    L_1941:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill data up]", 0x1941));
            goto L_1944;
        }
    {
        importSkillDataUp(info->m_skill1b8); goto L_chain_end;
    }
    case 0x1944:
    L_1944:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[push aside]", 0x1944));
            goto L_1947;
        }
    {
        info->m_pushAside= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1947:
    L_1947:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[lift up]", 0x1947));
            goto L_194a;
        }
    {
        info->m_liftUp= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x194a:
    L_194a:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[physical critical hit]", 0x194a));
            goto L_194d;
        }
    {
        info->m_physicalCriticalHit= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x194d:
    L_194d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[magical critical hit]", 0x194d));
            goto L_1950;
        }
    {
        info->m_magicalCriticalHit= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1950:
    L_1950:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[stuck]", 0x1950));
            goto L_1953;
        }
    {
        info->m_stuck= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1953:
    L_1953:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[ignore defense]", 0x1953));
            goto L_1956;
        }
    {
        info->m_ignoreDefense = (ScanInt((bool*)0) != 0); goto L_chain_end;
    }
    case 0x1956:
    L_1956:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[human damage rate]", 0x1956));
            goto L_1959;
        }
    {
        info->m_humanDamageRate= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1959:
    L_1959:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[fort damage rate]", 0x1959));
            goto L_195c;
        }
    {
        info->m_fortDamageRate= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x195c:
    L_195c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[monster damage rate]", 0x195c));
            goto L_1969;
        }
    {
        info->m_monsterDamageRate= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1969:
    L_1969:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[HP drain]", 0x1969));
            goto L_196c;
        }
    {
        info->m_hpDrain = (ScanInt((bool*)0) != 0); goto L_chain_end;
    }
    case 0x196c:
    L_196c:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[MP drain]", 0x196c));
            goto L_1970;
        }
    {
        info->m_mpDrain = (ScanInt((bool*)0) != 0); goto L_chain_end;
    }
    case 0x1970:
    L_1970:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[skill levelup]", 0x1970));
            goto L_1981;
        }
    {
        info->m_levelUpSkills[0].clear();
        // [b28d-solo-EquipmentParameterInfo] ORIG 0x89ab334/0x89ab350：`LevelUpSkill lu;`
        // 在循环**外**构造一次（C1），循环体内先 `lu.clear()` 再填字段；旧实现把 lu 放在
        // 循环内 ⇒ 每轮 C1/D1 且缺 clear 调用（p167 MISSING LevelUpSkill::clear ×1）。
        LevelUpSkill lu;
        while (ScanStr(&s))
        {
        lu.clear();
        // ORIG 0x89ab366：true 臂直写 `movl $0xb,-0xa40`、else 臂 call 后写槽
        // ⇒ 原码是 if/else 两段赋值，非三目（P47）。
        if (s == "[common]") { lu.m_job = 0xb; }
        else { lu.m_job = getCharacterJob(s.c_str()); }
        lu.m_field4 = ScanInt((bool*)0);
        lu.m_field8 = ScanInt((bool*)0);
        info->m_levelUpSkills[0].push_back(lu);
        }
        goto L_chain_end;
    }
    case 0x1981:
    L_1981:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[avatar select ability]", 0x1981));
            goto L_19c0;
        }
    {
        // ORIG 0x089ab451：先清空 m_avatar264，再按 47 项能力名表（0x93f4840）
        // 匹配解析；命中则填 STAvatarAbilityVariation（m_field0=表下标、
        // m_field4='+'→0 / '*'→1 标志、m_field8=float 数值；[SKILL_LEVEL](43)
        // 特殊：job 标签 + 两 int），size<=99 时 push_back；未命中项 push 默认值。
        info->m_avatar264.clear();
        // [OurOnly33 符号名轴 2026-09-11] 47 项 avatar 能力名表已合并为文件级
        // AVTVARIATION_TAG_NAME（ORIG `093f4840 D AVTVARIATION_TAG_NAME`，见文件头定义）。
        while (ScanStr(&s))
        {
        STAvatarAbilityVariation var;
        int i = 0;
        while (i <= 0x2e)
        {
        if (s == AVTVARIATION_TAG_NAME[i])
        {
        var.m_field0 = i;
        // ORIG 0x89ab4a4 `mov -0xa58,%eax; cmp $0x2b,%eax`：比较对象是
        // var.m_field0（成员槽往返），非循环变量 i。
        if (var.m_field0 == 0x2b)
        {
        var.m_field4 = 0;
        ScanStr(&s);
        if (s == "[common]")
        {
        var.m_skill.m_job = 0xb;
        }
        else
        {
        int job = getCharacterJob(s.c_str());
        if (job == 0xb)
        {
        LogManager::logFormat(0, "../RDARScriptItem.cpp",
        __FUNCTION__, 0x199e,
        "ImportEquipmentScript - file %s, invalid job tag %s",
        dir, s.c_str());
        return false;
        }
        var.m_skill.m_job = job;
        }
        var.m_skill.m_field4 = ScanInt((bool*)0);
        var.m_skill.m_field8 = ScanInt((bool*)0);
        }
        else
        {
        ScanStr(&s);
        if (s == "+")
        {
        var.m_field4 = 0;
        }
        else if (s == "*")
        {
        var.m_field4 = 1;
        }
        var.m_field8 = ScanFloat((bool*)0);
        }
        break;
        }
        ++i;
        }
        if (info->m_avatar264.size() <= 0x63)
        info->m_avatar264.push_back(var);
        }
        goto L_chain_end;
    }
    case 0x19c0:
    L_19c0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[room list move speed rate]", 0x19c0));
            goto L_19c4;
        }
    {
        info->m_roomListMoveSpeedRate= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x19c4:
    L_19c4:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[anti evil]", 0x19c4));
            goto L_19c7;
        }
    {
        info->m_antiEvil= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x19c7:
    L_19c7:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[rigidity]", 0x19c7));
            goto L_19ca;
        }
    {
        info->m_rigidity= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x19ca:
    L_19ca:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[fire attack]", 0x19ca));
            goto L_19cd;
        }
    {
        info->m_fireAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x19cd:
    L_19cd:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[water attack]", 0x19cd));
            goto L_19d0;
        }
    {
        info->m_waterAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x19d0:
    L_19d0:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[dark attack]", 0x19d0));
            goto L_19d3;
        }
    {
        info->m_darkAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x19d3:
    L_19d3:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[light attack]", 0x19d3));
            goto L_19d6;
        }
    {
        info->m_lightAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x19d6:
    L_19d6:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status probability]", 0x19d6));
            goto L_19f4;
        }
    {
        // ORIG 0x89ab9c6（probability）/0x89abafc（level）：ScanStr 状态名转枚举，
        // ScanInt 无检查；枚举 0x13 写满 18 槽，0..0x11 写对应槽，0x12/其它不写。
        // [b28d-solo-EquipmentParameterInfo] 两个 id 必须**各自独立**成块：ORIG 是两个
        // 独立分支（probability RA28_ @0x89aba26、level RA22_ @0x89abb5c），各有一次
        // ScanStr+getActiveStatusToEnumValue；合并写 `if (id==0x19d6) … else …` 使该调用
        // 只剩 2 处（p167 MISSING getActiveStatusToEnumValue ×1）。
        ScanStr(&s);
        {
        int st = getActiveStatusToEnumValue(s);
        int val = ScanInt((bool*)0);
        if (st != 0x12)
        {
        if (st == 0x13)
        {
        for (int i = 0; i < 0x12; ++i)
        {
        info->m_activeStatusProbability[i] = val;
        }
        }
        else
        {
        // ORIG 0x89abaa3：`for(j=0;j<=0x11;++j) if(st==j) arr[j]=val` 循环查找形。
        for (int j = 0; j <= 0x11; ++j)
        {
        if (st == j) info->m_activeStatusProbability[j] = val;
        }
        }
        }
        }
        goto L_chain_end;
    }
    case 0x19f4:
    L_19f4:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status level]", 0x19f4));
            goto L_1a0e;
        }
    {
        // ORIG 0x89abb5c：level 分支，与 probability 同形但写 m_activeStatusLevelTable。
        ScanStr(&s);
        {
        int st = getActiveStatusToEnumValue(s);
        int val = ScanInt((bool*)0);
        if (st != 0x12)
        {
        if (st == 0x13)
        {
        for (int i = 0; i < 0x12; ++i)
        {
        info->m_activeStatusLevelTable[i] = val;
        }
        }
        else
        {
        // ORIG 0x89abbd9 同形循环。
        for (int j = 0; j <= 0x11; ++j)
        {
        if (st == j) info->m_activeStatusLevelTable[j] = val;
        }
        }
        }
        }
        goto L_chain_end;
    }
    case 0x1a0e:
    L_1a0e:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[active status control info]", 0x1a0e));
            goto L_1a5d;
        }
    {
        // ORIG 0x89abc32：ScanStr 状态名→枚举，随后 3 个 ScanInt（无检查）分别写
        // 概率槽/等级槽/控制槽；枚举 0x13 写满 18 槽，0..0x11 写对应槽并按其类型
        // 追加特判：0/4/5/0xa/0x11 读入的数值仅消费不入表（部分仍写 control 区）。
        ScanStr(&s);
        {
        int st = getActiveStatusToEnumValue(s);
        int a = ScanInt((bool*)0);
        int b = ScanInt((bool*)0);
        int c = ScanInt((bool*)0);
        if (st != 0x12)
        {
        if (st == 0x13)
        {
        for (int i = 0; i < 0x12; ++i)
        {
        info->m_activeStatusProbability[i] = a;
        info->m_activeStatusLevelTable[i] = b;
        info->m_activeStatusCtrl[i] = c;
        }
        }
        else if (st >= 0 && st < 0x12)
        {
        info->m_activeStatusProbability[st] = a;
        info->m_activeStatusLevelTable[st] = b;
        info->m_activeStatusCtrl[st] = c;
        if (st == 0)
        {
        // ORIG 0x89abdeb/0x89abdfd：结果落具名槽 -0xa4/-0xa0（P0 槽往返）。
        int v0 = ScanInt((bool*)0);
        int v1 = ScanInt((bool*)0);
        (void)v0; (void)v1;
        }
        else if (st == 2)
        {
        int v2 = ScanInt((bool*)0);
        info->m_activeStatusControlInfo[0] = v2;
        }
        else if (st == 4)
        {
        int v3 = ScanInt((bool*)0); (void)v3;
        }
        else if (st == 5)
        {
        int v4 = ScanInt((bool*)0);
        int v5 = ScanInt((bool*)0);
        (void)v4; (void)v5;
        }
        else if (st == 6)
        {
        int v6 = ScanInt((bool*)0);
        info->m_activeStatusControlInfo[1] = v6;
        }
        else if (st == 7)
        {
        int v7 = ScanInt((bool*)0);
        info->m_activeStatusControlInfo[2] = v7;
        }
        else if (st == 9)
        {
        int v8 = ScanInt((bool*)0);
        int v9 = ScanInt((bool*)0);
        int v10 = ScanInt((bool*)0);
        (void)v9; (void)v10;
        info->m_activeStatusControlInfo[3] = v8;
        }
        else if (st == 0xa)
        {
        int v11 = ScanInt((bool*)0); (void)v11;
        }
        else if (st == 0xb)
        {
        int v12 = ScanInt((bool*)0);
        info->m_activeStatusControlInfo[4] = v12;
        }
        else if (st == 0x11)
        {
        int v13 = ScanInt((bool*)0); (void)v13;
        }
        }
        }
        }
        goto L_chain_end;
    }
    case 0x1a5d:
    L_1a5d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[account bind linear ability]", 0x1a5d));
            goto L_1a60;
        }
    {
        // ORIG 0x89abffa/0x89abf9b（两关键字共享执行体）：ScanInt（bool 出参）
        // 必须成功写 m_growthLevelBegin，失败即返回 0；ScanInt 无检查写
        // m_growthLevelEnd；随后 while ScanType 内层子标签循环写入成长表，
        // 未知标签忽略继续；EOF 为正常结束。
        int lv = 0;
        if (!ScanInt(&lv)) return false;
        info->m_growthLevelBegin = lv;
        info->m_growthLevelEnd = ScanInt((bool*)0);
        std::string t;
        while (ScanType(t, true))
        {
        if (t == "[physical attack]")
        {
        for (int i = 0; i < 2; ++i) info->m_linPhyAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[physical defense]")
        {
        for (int i = 0; i < 2; ++i) info->m_linPhyDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[magical attack]")
        {
        for (int i = 0; i < 2; ++i) info->m_linMagAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[magical defense]")
        {
        for (int i = 0; i < 2; ++i) info->m_linMagDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment physical attack]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqPhyAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment physical defense]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqPhyDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment magical attack]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqMagAtk[i] = ScanInt((bool*)0);
        }
        else if (t == "[equipment magical defense]")
        {
        for (int i = 0; i < 4; ++i) info->m_linEqMagDef[i] = ScanInt((bool*)0);
        }
        else if (t == "[grade]")
        {
        // ORIG 0x89ac34a..0x89ac37d：`for (i=0; i<=1; ++i)`，循环体
        // `mov i,%esi; movl $0,(%esp); call ScanIntPb; add $0xf8,%ecx;
        //  mov %eax,(%edx,%ecx,4)`（两步寻址写 this+0x3e0+4i）。
        // 原写成两条 ScanInt 直写 +0x3e0/+0x40c ⇒ p167 静态调用点 +1（重载相同，
        // 仅循环 vs 展开），本轮按 ORIG 恢复循环形。
        for (int i = 0; i <= 1; ++i)
        info->m_growthGrade[i] = ScanInt((bool*)0);
        }
        else if (t == "[separate attack]")
        {
        // ORIG 0x89ac3a5..0x89ac3d6：`lea -0xa10(%ebp); call _Z7ScanIntPi`（函数级共享
        // 临时 v），随后两步寻址 `add $0xf8,%ecx; mov %edx,0x8(%eax,%ecx,4)`
        // 写 this+0x3e8+4i。原写成直传成员地址（无局部槽）⇒ 形态差。
        for (int i = 0; i < 4; ++i)
        {
        ScanInt(&v);
        info->m_linSepAtk[i] = v;
        }
        }
        else if (t == "[separate rate]")
        {
        // ORIG 0x89ac400..0x89ac433：`lea -0xa60(%ebp); call _Z9ScanFloatPf` + 两步寻址。
        // [ImpBig2] 曾按此改写为「使用函数级 `float fv`（-0xa60）」——A/B 实测**反劣**：
        // 4.4 因该使用点重排函数级槽区，把 s 由 -0xa0c → -0xa08（65 处引用全体漂移，
        // 逐行相似度 84.3% → 67.9%、ndiff 1056 → 2152）⇒ 回退为直传成员地址，
        // 该段形态差归 N82 槽相位族（不可翻，勿再试）。
        for (int i = 0; i < 4; ++i)
        {
        ScanFloat(&info->m_linSepRate[i]);
        }
        }
        else if (t == "[repair price]")
        {
        // ORIG 0x89ac44f..0x89ac48b：循环 2 次 `bool ScanInt(int*)`（`_Z7ScanIntPi`），
        // 经具名局部槽 -0x40(i) / -0xa10(v) 取值后写 m_growthRepair[i]
        // （`add $0x100,%ecx; mov %edx,0x8(%eax,%ecx,4)` = this+0x408+4i，两步寻址）。
        // 原写成 `int ScanInt(bool*)` 两条直写 +0x408/+0x40c ⇒ p167 EXTRA `_Z7ScanIntPb`×2
        // （重载选错），本轮修正为 int* 重载 + 局部槽形。
        for (int i = 0; i <= 1; ++i)
        {
        ScanInt(&v);
        info->m_growthRepair[i] = v;
        }
        }
        else if (t == "[/level linear ability]")
        {
        break;
        }
        }
        goto L_chain_end;
    }
    case 0x1a60:
    L_1a60:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[level linear ability]", 0x1a60));
            goto L_1b39;
        }
    case 0x1b39:
    L_1b39:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[all skill item]", 0x1b39));
            goto L_1b3d;
        }
    {
        if (!importAllSkillItem(info)) return false; goto L_chain_end;
    }
    case 0x1b3d:
    L_1b3d:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[item overpower desc]", 0x1b3d));
            goto L_1b41;
        }
    {
        if (!importItemOverpowerDesc(info)) return false; goto L_chain_end;
    }
    case 0x1b41:
    L_1b41:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[item overpower part]", 0x1b41));
            goto L_1b47;
        }
    {
        // ORIG 0x89ac64d：`mov 0x8(%ebp),%eax; movb $0x1,0x425(%eax)` →
        // info->+0x425（m_119.m_425）置 1，随后统一尾出口 jmp 0x89acebb。
        info->m_119.m_425 = 1;
        goto L_chain_end;
    }
    case 0x1b47:
    L_1b47:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[ice appendage]", 0x1b47));
            goto L_1b53;
        }
    {
        // ORIG 0x89ac6b9：3× ScanInt((bool*)0) 先存独立局部 -0x3c/-0x38/-0x34，
        // 再按序写元素 -0xa74..-0xa68：{0, i1, i2, i3}（movl $0 + 3 次拷贝，
        // 第 5 个 int ORIG 不写，push_back 仍拷 0x14：0x89c6e76 lea 0x14(%eax)），
        // push_back(&info->m_aura200) @0x89ac721。
        int i1 = ScanInt((bool*)0);
        int i2 = ScanInt((bool*)0);
        int i3 = ScanInt((bool*)0);
        EquipmentParameterInfo::EquipmentAuraAppendageData e;
        e.m_field0 = 0;
        e.m_field4 = i1;
        e.m_field8 = i2;
        e.m_fieldc = i3;
        info->m_aura200.push_back(e);
        goto L_chain_end;
    }
    case 0x1b53:
    L_1b53:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[curse appendage]", 0x1b53));
            goto L_1b65;
        }
    {
        // ORIG 0x89ac78b：4× ScanInt((bool*)0) 先存独立局部 -0x30/-0x2c/-0x28/-0x24，
        // 再按序写元素 -0xa88..-0xa78：{1, j1, j2, j3, j4}（5 int 全写），
        // push_back(&info->m_aura200) @0x89ac80b。
        int j1 = ScanInt((bool*)0);
        int j2 = ScanInt((bool*)0);
        int j3 = ScanInt((bool*)0);
        int j4 = ScanInt((bool*)0);
        EquipmentParameterInfo::EquipmentAuraAppendageData e;
        e.m_field0 = 1;
        e.m_field4 = j1;
        e.m_field8 = j2;
        e.m_fieldc = j3;
        e.m_field10 = j4;
        info->m_aura200.push_back(e);
        goto L_chain_end;
    }
    case 0x1b65:
    L_1b65:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[upgrade cost discount]", 0x1b65));
            goto L_1b68;
        }
    {
        // ORIG 0x89ac815：ScanFloat 无检查写 +0x444。
        info->m_upgradeCostDiscount = ScanFloat((bool*)0);
        goto L_chain_end;
    }
    case 0x1b68:
    L_1b68:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[upgrade prob increase]", 0x1b68));
            goto L_1b6b;
        }
    {
        // ORIG 0x89ac88c：ScanFloat 无检查写 +0x448。
        info->m_upgradeProbIncrease = ScanFloat((bool*)0);
        goto L_chain_end;
    }
    case 0x1b6b:
    L_1b6b:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[assault cost discount]", 0x1b6b));
            goto L_1b70;
        }
    {
        info->m_assaultCostDiscount= ScanFloat((bool*)0); goto L_chain_end;
    }
    case 0x1b70:
    L_1b70:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[decrease critical damage]", 0x1b70));
            goto L_1b83;
        }
    {
        // ORIG 0x89aca1d：ScanStr "%" 判定；ScanInt 无检查转 float；
        // 非 %：值位模式存 +0x210（汇编 mov 32 位）；%：+0x20c = 值 / 10.0。
        ScanStr(&s);
        float f = (float)ScanInt((bool*)0);
        if (s == "%")
        // ORIG 0x89aca3f `flds 0x8e1a56c` = **100.0f**（.rodata 字节 00 00 c8 42）；
        // 序列 `flds f(-0x1c); flds 100.0f; fdivrp %st,%st(1)` ⇒ 结果 f/100.0f。
        // 原写成 f/10.0f（我方 .rodata 0x1fdc = 00 00 20 41 = 10.0f）⇒ **真数值 bug**，本轮修正。
        info->m_decreaseCriticalDamage = f / 100.0f;
        else
        info->m_210 = f;
        goto L_chain_end;
    }
    case 0x1b83:
    L_1b83:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[hardcoding parameter]", 0x1b83));
            goto L_1b90;
        }
    {
        // ORIG 0x89acac3..0x89acb0d：`bool ok`(-0xa8d) 与 `int v`(-0xa8c) 两个独立槽；
        // 形为 `ok = false; for(;;){ v = ScanInt(&ok); if (ok) { push_back(v); continue; } break; }`
        // （`*ok=true` 表示解析成功 ⇒ 继续；失败即落到出口 `jmp <L_chain_end>`）。
        // 原写成 `while (ScanInt(&v))`（`_Z7ScanIntPi`）⇒ p167 EXTRA `_Z7ScanIntPi`×1
        // （重载选型错），本轮按 ORIG 恢复 `int ScanInt(bool*)` + ok 出参槽形。
        bool ok = false;
        int v;
        for (;;)
        {
        v = ScanInt(&ok);
        if (ok)
        {
        info->m_vec214.push_back(v);
        continue;
        }
        break;
        }
        goto L_chain_end;
    }
    case 0x1b90:
    L_1b90:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[all elemental attack]", 0x1b90));
            goto L_1b96;
        }
    {
        info->m_allElementalAttack= ScanInt((bool*)0); goto L_chain_end;
    }
    case 0x1b96:
    L_1b96:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[rebirth]", 0x1b96));
            goto L_1bbb;
        }
    {
        std::string t;
        while (ScanType(t, true))
        {
        if (t == "[probability]")      { info->m_rebirth450.m_field0= ScanInt((bool*)0); }
        else if (t == "[consume item]"){ info->m_rebirth450.m_pair4.first= ScanInt((bool*)0);
        info->m_rebirth450.m_pair4.second= ScanInt((bool*)0); }
        else if (t == "[hp recovery]") { info->m_rebirth450.m_fieldc= ScanInt((bool*)0); }
        else if (t == "[cooltime]")    { info->m_rebirth450.m_field14= ScanInt((bool*)0); }
        else if (t == "[mp recovery]") { info->m_rebirth450.m_field10= ScanInt((bool*)0); }
        else break;
        }
        goto L_chain_end;
    }
    case 0x1bbb:
    L_1bbb:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[potion physical attack]", 0x1bbb));
            goto L_1bbd;
        }
    case 0x1bbd:
    L_1bbd:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[potion magical attack]", 0x1bbd));
            goto L_1bc1;
        }
    {
        // ORIG 0x89acd24（0x1bbb [potion physical attack]，串 @0x8e181ad，
        // pair IRA25 注册 @0x89acd5b）/0x89acd83（0x1bbd [potion magical attack]，
        // 串 @0x8e181c6，pair IRA24 注册 @0x89acdba）：分派目标即注册块本身，
        // 块首 `movzbl 0x94fe044; xor $0x1; test; je 89aceb7/89aceba` —— 运行期
        // __inited 已置位，je 直接落统一尾出口 0x89acebb（scanRandomOptionValue
        // + return true）。ORIG 对该二 id 无任何解析实体，仅注册 + 命中不处理。
        goto L_chain_end;
    }
    case 0x1bc1:
    L_1bc1:
        if (!__inited)
        {
            __lookup.insert(std::pair<const char* const, int>("[variable stat]", 0x1bc1));
            goto L_unknown;         // ORIG 0x89ace5a `jmp 0x89ace7e`（共享未知块）
        }
    {
        // ORIG 0x89ace3f：VariableStat 局部 ctor（0x89c2dfc）后
        // while (scanVariableStat(v))（0x88bdb27）成功即 info->m_var474（+0x474）
        // push_back（0x89ace65）直接逐条追加；结束 jmp 尾出口 0x89acebb。
        VariableStat v;
        while (scanVariableStat(v)) info->m_var474.push_back(v);
        goto L_chain_end;
    }
    default:
        // ORIG 0x89ace7e（共享块）：未知 token 在**已初始化**时返回 false；
        // 建表首轮（__inited==0）则落 chain_end 继续重查。原实现 break 直达
        // chain_end ⇒ 已初始化 + 未知 token 会错误返回 true（真语义缺口）。
        goto L_unknown;
    }
 L_unknown:                          // ORIG 0x89ace7e（default 与 [variable stat] 共享）
    if (__inited) return false;
 L_chain_end: ;
    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
        goto relaunch;
    }

done:
    // ORIG 0x89acf17：成功路径对非 [if]/[then] token 经 info->m_434 子指针
    // 调 scanRandomOptionValue（空指针时其自身直接返回）。
    if (path != "[if]" && path != "[then]")
        scanRandomOptionValue(info->m_434, path);
    return true;
}