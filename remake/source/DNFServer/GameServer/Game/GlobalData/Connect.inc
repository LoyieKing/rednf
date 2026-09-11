// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080ec760
// [SweepGlobalData] 旧 gap 2 缺口修正：cMyTrace 两语句 → 单语句临时链
// （ORIG 0x080ec7ba..0x080ec864 五实参先于 ctor 求值，P37/P87/P329）后
// 117=117、call 序列 ratio=1.000（gap 0）；残余 = N82 槽位/寄存器。
bool CDBConnector::Connect(DBConnInfo infoMap)
{
    // ORIG 0x080ec760：按值遍历连接表；idx==0/5 跳过；cMyTrace 记录
    // "Open(type:%2d, %s/%d/%s/%s)"（.rodata 0x8b356a4）；Open 失败即 false。
    // [b28d-solo-GlobalData] p167 修：ORIG 全程逐字段重调 operator->（ptEv×7，
    // P157/P183 禁 idx/info 缓存）；cMyTrace 为 5 参重载 `C1EPKciibb`
    // （str, 0x4e, 9, true, true；旧 3 参形 MISSING 5 参 + EXTRA 3 参）。
    // [b28d-solo-GlobalData N·Main 裁决] STDBConnInfo 偏移两函数证据冲突：本件
    // ORIG 0x080ec760 段为 port@0x18/db@0x1c/user@0x46，而 QueryConnInfo
    // 0x080eca6c `add $0x42,%edx` 与现布局 m_db@0x42 直证吻合 ⇒ 判定本件实参序/
    // 字段名映射与我们不同（p167 CLEAR，语义等价），**维持 QueryConnInfo 直证
    // 布局**，不重排 CEnvironment.h 的 STDBConnInfo（风险远超收益）⇒ N 封顶。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        // [b28d-solo-GlobalData 判据①] ORIG 0x080ec790/0x080ec7a1/0x080ec7b4 极性：
        // `test;je T(真)` + `cmp $5;jne F` + `test %al;jne <++it>` ⇒ 原码是
        // **正极性守卫** `if (a==0 || a==5) continue;`（旧 `if(!(…)){body}` 形把
        // 两侧跳转落点互换：ours je→F/jne→body，ORIG je→T/jne→跳过体）。
        if (it->first == 0 || it->first == (ENUM_DB_HANDLE_IDX)5)
        {
            continue;
        }
        // ORIG 0x080ec7ba..0x080ec864（P37/P87/P329）：五个实参在 cMyTrace
        // ctor **之前**全部求值（m_host/-0x40、m_port/-0x3c、m_db/%edi、
        // m_user/%esi、first/%ebx），随后 ctor + operator() ⇒ 源形是单语句
        // 临时链 `cMyTrace(...)(...)`，不是具名对象两语句。
        // [K8GlobalData 2026-09-11 · §6.5 串盲区] ORIG .rodata 0x8b36340 =
        // 'bool CDBConnector::Connect(DBConnInfo)'（0x080ec760 段直证）——原写法
        // 裸 "CDBConnector::Connect" 与 ORIG 串首参不符（ORIG 侧为 __PRETTY_FUNCTION__，
        // 其 typedef 名 DBConnInfo 与本地 DBInfoMap 不同，故按 ORIG 字节写字面量）。
        cMyTrace(__PRETTY_FUNCTION__, 0x4e, 9, true, true)
            // [GlobalData-判据①] 实参序照 ORIG 0x080ec7c8(+0x46=db)/0x080ec7d9
            // (+0x1c=user)：ORIG 先算 arg5=it->second.m_db(+0x46)、再 arg4=
            // it->second.m_user(+0x1c) ⇒ 原码 arg4/arg5 为 (user, db)；旧写法
            // (db, user) 反序（R2L 先算 user+4）。注意 +0x04 基址来自 pair 的
            // second 偏移，与 CEnvironment.h 布局一致（QueryConnInfo 0x080ecb53
            // db@+0x42 直证）。
            ("Open(type:%2d, %s/%d/%s/%s)", (int)it->first, it->second.m_host,
             it->second.m_port, it->second.m_user, it->second.m_db);
        if (!Open(it->first, (ENUM_SERVER_GROUP)0, it->second))
        {
            return false;
        }
    }
    return true;
}