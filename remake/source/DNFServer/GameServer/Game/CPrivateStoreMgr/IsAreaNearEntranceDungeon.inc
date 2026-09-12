// ---- IsAreaNearEntranceDungeon（ORIG T 0x85c5082，报告 IsAreaNearEntranceDungeon.md L15-269 全文）----
// 开店区域校验：村庄 2 区 0 即入口区允许开店；区域类型 4 禁止；异常走 errno 日志垫。
// [b28d-24] DIFF 残53: P167 MISSING cMyTrace::operator()×1（少一条日志调用，真缺口）。ORIG 0x085c5082
// [b28d-solo-CPrivateStoreMgr] 残 79 行 = N111（ORIG 的 area==0 trace 块
// `0x085c5273..0x085c5299` 被 `jmp 0x298` 跳过、字节级复核无任何跳转指向该区
// ⇒ 4.1 无入口死错误块；补 trace 会让 area==0 时多打日志、改变行为，故不补，
// P167 该 MISSING 项按 N111 豁免）+ N82（5 个互斥 cMyTrace 对象 4.1 各占独立槽，
// 帧 0x90 vs 我们 0x70）。
// [L14] 本轮复核：165/173，6 块中 4 块仅为行号/地址立即数（AE 口径归一后豁免），
// 余 2 块 = 帧/槽（N82）+ add-vs-lea（N4）⇒ 封顶。
// [R18hp3 2026-09-13 §24.3 轴 ④ 落修] 上列「不补 trace」判词与源码不一致：源码实写
//   `if (area == 0) { cMyTrace(0x24c, "getArea call failed (area no:%d)", areaNo); }`
//   —— 该条件是**真实条件**（area 由 Village::getArea 返回值写 -0x18），运行期可达
//   ⇒ 我方在 getArea 返回 0 时多打一条日志，而 ORIG 该块被 0x85c526e `e9 a7 00 00 00`
//   恒跳过且零入边（热补死块）⇒ 运行期可观察差异。已按 P337 复刻恒假 volatile 守卫
//   （块内容/调用点原样保留、恒跳过），见函数尾注释。
bool CPrivateStore::IsAreaNearEntranceDungeon(CUser* user)
{
    // [b28d-solo] ORIG 0x85c508d..0x85c50a9 五条 movl $0 的槽序：-0x1c 死局部
    // （P171 只初始化从不读取）、area@-0x18、gameWorld@-0x14、village@-0x10、
    // areaNo@-0xc；声明序按 ORIG 槽深→浅排列复现 4.4 分配。
    int deadLocal = 0;         // -0x1c（ORIG 只写不读）
    Area* area = 0;            // -0x18
    GameWorld* gameWorld = 0;  // -0x14
    Village* village = 0;      // -0x10
    int areaNo = 0;            // -0xc
    (void)deadLocal;

    try
    {
        if (user == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x233, 5)("pUser == NULL");
            return false;
        }
        gameWorld = G_GameWorld();
        // [b28d-solo] P225：ORIG 0x85c50fe `cmp -0x14,0; sete al; test al,al;
        // je` 为 4.1 bool 物化形，4.4 的 if 恒直跳 ⇒ while 形复现。
        while (gameWorld == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x239, 5)("G_GameWorld call failed");
            return false;
        }
        // [b23S3C] P48 去缓存：ORIG get_area 结果 eax→%ebx 跨 getCurCharacVill
        // call 直落 0x8(%esp) 实参槽（0x0d5-0x0f8），无 curArea 槽 ⇒ 右值直传。
        if (!gameWorld->check_valid_area((int)user->getCurCharacVill(),
                                         user->get_area(false)))
        {
            // ORIG 复用 "G_GameWorld call failed" 文案（报告行 84 字面实证）。
            cMyTrace(__PRETTY_FUNCTION__, 0x23f, 5)("G_GameWorld call failed");
            return false;
        }
        village = gameWorld->getUserVillage(user);
        // [b28d-solo] P225：ORIG 0x85c51db `cmp -0x10,0; sete al; test; je`。
        while (village == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x246, 5)("getUserVillage call failed");
            return false;
        }
        areaNo = user->get_area(false);
        area = village->getArea(areaNo);
        // [b28d-solo] ORIG 0x85c5251 `cmp -0x18,0; sete al; test; jne <远端岛>`。
        // [L14] 实测四形：(a) 原 4×if/return —— 173/165（近端岛，gap +8）；
        // (b) `bool ok` + 单表达式短路 —— 168/165 且 ok 占 -0x20 槽使全帧平移；
        // (c) P222 正向包裹 + 死块置 return 之后 —— 151/165（4.4 对 return 后不可达
        // 代码 DCE，死块整段消失）；**(d) 采纳形** = C05① 尾 fail 块嵌套
        // （主体进正向 if + 尾 `return false`）+ 死块**保留为可达体**（若块内带
        // `return` 会被 4.4 DCE）⇒ **165 = 165，gap +8 → 0**。
        // 残余 = ORIG 0x085c5273..0x085c5285 的 7 条数据区字节被 objdump 线性解码
        // 成荒谬指令（`or %cl,0x489d445(%ebp)` 等），非源码可造 ⇒ 该件封顶为
        // 「gap=0 + 数据区解码差」（§6.2 三件套：gap 0 ✓ / p167 CLEAR ✓）。
        // [K2 2026-09-11 复验] fresh .o：gap 0（165=165）保持；残 53 行 = try/catch
        // 两 EH 尾块相对序（ORIG 正常路径尾块置最末 + `cmpl $0,-0x18; sete %al;
        // test %al,%al; jne` 槽比零物化 vs 4.4 恒直跳 `je`，N104/setcc-fold）+
        // 上述数据区线性解码 ⇒ 三件套（gap 0 / p167 CLEAR / 槽集）已过，维持封顶。
        if (area != 0 &&
            ((area->m_villageIndex == 2 && area->m_areaIndex == 0) || area->m_areaType != 4))
        {
            return true;
        }
        // [R18hp3 §24.3 ④ 修复] ORIG 0x85c5273..0x85c529e 是**热补死块**：
        //   0x85c526e raw=`e9 a7 00 00 00`（jmp 0x85c531a = mov $1,%ebx）无条件
        //   跳过该区，且字节级复核**零入边** ⇒ ORIG 运行期恒不可达（热补前该分支
        //   体为 trace + return 0，热补后 f4==2&&f0==0 直接 return 1）。
        //   旧形 `if (area == 0) { trace }` 是**真实条件**（area 由
        //   Village::getArea 返回值写 -0x18，运行期可 0）⇒ 该日志可达、ORIG 不打
        //   ⇒ 运行期可观察差异（多日志）。按 P337 复刻恒假 volatile 守卫：
        //   块内容（cMyTrace ctor+operator() 调用点）原样保留、运行期恒跳过。
        volatile bool deadGetAreaTrace = false;
        if (deadGetAreaTrace)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x24c, 5)("getArea call failed (area no:%d)", areaNo);
        }
        return false;
    }
    catch (...)
    {
        // ORIG 落地垫 0x085c52b2-0x085c5303：errno → strerror(errno) 作 %s。
        LogManager::logFormat(1, "PrivateStore.cpp", __PRETTY_FUNCTION__, 0x25c,
                              "[EXCEPTION IsAreaNearEntranceDungeon] - code = %s",
                              strerror(errno));
        return false;
    }
}