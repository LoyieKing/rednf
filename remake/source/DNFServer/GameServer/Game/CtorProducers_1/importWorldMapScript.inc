bool importWorldMapScript(WorldMapScript* script, const char* path)
{
    // [b28d-solo-CtorProducers_1 判词·P234/P164/P317/P171/P0，残余 N82]
    // ORIG 0x08ab0388（ndiff 62→12、gap -20→-2）：本轮修
    // ①`std::pair<int,int>(tmp, second)` 第二参右值化（`static_cast<int&&>`，P234）；
    // ②外层 while → `for(;;){ if(!ScanType(...)) break; }` 复现头部
    //   `call; xor $0x1; test; jne 尾`（P164）；
    // ③`script->m_field10 = ScanInt(NULL)` 内联实参（P0，消 tmp 往返 3 条）；
    // ④isType 声明移到 wd.m_dungeonIndex 赋值之后（P171 照抄序）；
    // ⑤删 first/secondPoint 零初始化（P171）；⑥WorldMapScript.h 删
    //   WorldMapDungeon 用户空析构（P317，消 2×out-of-line D1 call+EH 段；
    //   D1/D2 W 符号仍发射、尺寸 5 与 ORIG 同——R2 已广播）。
    // 残余：`second = ScanInt()` 经复用槽 -0x40 的 2 条中转 + minimap 槽
    // -0x44/-0x48 vs ORIG -0x40/-0x44（4.4 块内标量赋值恒经复用槽；register/
    // 函数头声明/初始化式三形实测均无效或劣化）⇒ N82 封顶。
    // ORIG 0x08ab0388。
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory, path))
        return false;
    script->clear();

    // 函数级局部序与宽度实证：0x08ab03bf `lea -0x24; call std::string::string()`
    // （token **先**构造）→ 0x08ab03ca `movl $0x0,-0x28`（int 暂存）→
    // 0x08ab03d1 `movb $0x0,-0x29`（bool flag）→ 0x08ab03d5 `jmp` 条件头
    // 0x08ab03d8 `movl $1,0x4; lea token; call ScanType; xor $1; test; jne 尾`。
    // ⇒ 外层是 while(ScanType(...))，且 tmp/flag 是**跨所有子块复用的函数级**
    // 局部（不是每块各一个），故各块写成 `tmp = ScanInt(&flag); if (!flag) break;`
    // （P-H：中间变量必须提到函数级并与 ORIG 同数量，否则槽号全漂）。
    std::string token;
    int tmp = 0;
    bool flag = false;
    // [b28d-solo-CtorProducers_1 修复·P164] ORIG 0x08ab03d5 `jmp` 到头部条件块
    // 0x08ab03d8（`call ScanType; xor $0x1; test; jne 尾`）；4.4 对 while 恒旋转到
    // 尾部 ⇒ 改 `for(;;){ if(!ScanType(...)) break; … }` 复现头部取反物化块。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (token == "[map image]")
        {
            ScanStr(&script->m_mapImage);
            // [b28d-solo-CtorProducers_1 修复·P0] ORIG 0x08ab0449..0x08ab0451
            // `call ScanInt; mov 0x8(%ebp),%edx; mov %eax,0x10(%edx)` ⇒ 返回值
            // 直存成员，**无** tmp 槽往返（旧形多 3 条）。
            script->m_field10 = ScanInt((bool*)0);
        }
        else if (token == "[ui path]")
        {
            ScanStr(&script->m_uiPath);
        }
        else if (token == "[dungeon]")
        {
            // 0x08ab04a9..0x08ab055d：wd 基址 -0x38；`movb $0,-0x39` 是**独立 bool**
            // （在 wd 之前一字节），ScanTypeOrInt 实参 = (&b, token, &flag, true)
            // （0x08ab04f2 四实参），返回存 -0x28；`[in progress]` 命中写
            // **wd.m_flag=1**（0x08ab052c `movl $1,-0x30`），随后 `ScanInt(NULL)`
            // 覆盖 -0x28；最后 `mov -0x28,%eax; mov %eax,-0x34` ⇒ wd.m_state=tmp。
            script->m_dungeons.clear();
            for (;;)
            {
                tmp = ScanInt(&flag);
                if (!flag)
                    break;
                // [b28d-solo-CtorProducers_1 修复·P171 照抄序] ORIG 0x08ab04c7
                // 先构造 wd（-0x38）→ 0x08ab04d2 写 wd.m_dungeonIndex →
                // 0x08ab04d8 `movb $0x0,-0x39`（isType 初始化）⇒ isType 声明在
                // wd 之后（槽 -0x39 紧邻 wd 上方由编译器排布，非声明序）。
                WorldMapDungeon wd;
                wd.m_dungeonIndex = tmp;
                bool isType = false;
                // mangled _Z13ScanTypeOrIntRbRSsS_b = (bool&, string&, bool&, bool)，
                // 0x08ab04f2 传的是 `lea -0x39`（&isType）与 0x08ab04e7 的
                // `lea -0x29`（&flag）两个地址。
                tmp = ScanTypeOrInt(isType, token, flag, true);
                if (!flag)
                    break;
                if (isType)
                {
                    if (token == "[in progress]")
                        wd.m_flag = 1;
                    tmp = ScanInt((bool*)0);
                }
                wd.m_state = tmp;
                script->m_dungeons.push_back(wd);
            }
        }
        else if (token == "[hell dungeon]")
        {
            // 0x08ab057e..0x08ab0598：tmp = ScanInt(NULL)；`test;setne %dl`
            // 一次物化后 `mov %dl,0x20(%eax)` ⇒ 赋 bool 表达式（技巧 9）。
            tmp = ScanInt((bool*)0);
            script->m_isHellDungeon = (tmp != 0);
        }
        else if (token == "[hell quest]")
        {
            for (;;)
            {
                // 0x08ab05b7..0x08ab05eb：push_back 的实参是 `lea -0x28` ⇒
                // 传的是函数级 tmp 的**地址**（不是新局部 v）。
                tmp = ScanInt(&flag);
                if (!flag)
                    break;
                script->m_hellQuests.push_back(tmp);
            }
        }
        else if (token == "[hell freepass item]")
        {
            for (;;)
            {
                tmp = ScanInt(&flag);
                if (!flag)
                    break;
                // [b28d-solo-CtorProducers_1 实验·P212] ORIG 0x08ab062a..0x08ab062f
                // `call ScanInt; mov %eax,-0x18` 直存；块内声明致 4.4 中转 -0x40
                // （函数头声明实测槽全漂 ndiff 12→80，回退块内）。
                // [b28d-solo-CtorProducers_1 判词·N82] 残余 2 条中转 + minimap 槽
                // -0x44/-0x48 vs ORIG -0x40/-0x44（4.4 块内标量赋值恒经复用槽；
                // register/函数头声明/初始化式三形实测均无效或劣化）⇒ 封顶。
                // [b28d-S435 实验·三形同值] 块内「声明+赋值」/块内初始化式/分支块级
                // 三形实测 ndiff 均 12（4.4 恒经 -0x40 中转）⇒ 保持原形，P112 封顶。
                int second;
                second = ScanInt((bool*)0);   // ORIG 独立槽 -0x18
                // ORIG 0x08ab0646 `call _ZNSt4pairIiiEC1IRiiEEOT_OT0_`（pair<int,int>
                // 以两个 **int lvalue 引用** 实例化构造）+ 0x08ab065b
                // `push_back(EOS1_)` ⇒ 原码是嵌套 pair 临时构造，
                // 写 std::make_pair 会换成 _ZSt8make_pair 实例（P134）。
                // p167：ORIG 实例 `pair<int,int>::pair<int&,int>(int&, int&&)`
                // （第二参右值）⇒ `static_cast<int&&>(second)`（P234：纯语言构造，
                // 零 call）；旧两左值形产 `pair<int&,int&>` 实例差。
                script->m_freepassItems.push_back(
                    std::pair<int, int>(tmp, static_cast<int&&>(second)));
            }
        }
        else if (token == "[item condition]")
        {
            for (;;)
            {
                // 0x08ab0679..0x08ab06aa：先 ScanInt(&flag) 落 tmp，再无条件
                // ScanInt(NULL)（结果丢弃），之后才判 flag。
                tmp = ScanInt(&flag);
                ScanInt((bool*)0);
                if (!flag)
                    break;
                script->m_itemConditions.push_back(tmp);
            }
        }
        else if (token == "[minimap points]")
        {
            // 0x08ab06d6..0x08ab075c：外层 `tmp = ScanInt(&flag)` 后 while(flag)；
            // 内层 `for (k=0; k<2 && flag; ++k)`（0x08ab0715 `cmpl $1,-0xc; jg`
            // + 0x08ab071b `movzbl -0x29` 复合条件）；k==0 写 -0x44、否则写 -0x48
            // ⇒ 两个**独立具名 int**（不是数组，数组会走变址寻址）。
            tmp = ScanInt(&flag);
            while (flag)
            {
                // [b28d-solo-CtorProducers_1 修复·P171] ORIG 0x08ab06e6 直接进
                // for 头，**无** first/secondPoint 零初始化（多写即 +2 条）。
                int first;
                int secondPoint;
                for (int k = 0; k < 2 && flag; ++k)
                {
                    if (k == 0)
                        first = tmp;
                    else
                        secondPoint = tmp;
                    tmp = ScanInt(&flag);
                }
                script->m_minimapPoints.push_back(
                    std::pair<int, int>(first, secondPoint));
            }
        }
    }
    script->m_index =
        g_worldMapScriptFileList.FindIndexByFullScan(path);
    return true;
}