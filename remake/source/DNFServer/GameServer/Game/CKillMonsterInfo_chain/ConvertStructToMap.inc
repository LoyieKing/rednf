// ---- ConvertStructToMap —— ORIG T 0x0854cb42 ------------------------------
// for 循环遍历 st 数组 count 项；mobIdx==0 提前返回 1。bAPC ? 经
// CAICharacterList::get 校验 : CDataManager::find_monster 校验；失败 logFormat
// 后 continue（ORIG jmp +0x158 循环增量实证）；通过则共用插入块
// insert(make_pair(mobIdx, diff))（make_pair 实参序 = 0x4(%esp) mobIdx /
// 0x8(%esp) diff 实证）。
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0854cb42：100/100
// 助记符/槽位逐条全等（check_tu_routed 路由口径），P167 CLEAR。清单旧标
// NEAR ndiff=9 系 .o 重定位地址显示差（P264：字符串/函数地址未解析），
// 非助记符差异；TU 口径归 AE。
// [L6S5 P139 + §6.5 串盲区] 旧版 logFormat 用 `__PRETTY_FUNCTION__` 展开成
//   "…(stMonsterInfo_t*, unsigned int, std::map<…>&, bool)"（0xd1 字节），
//   与 ORIG @0x08c9ec80 size **0x5b** 的 "…(stMonsterInfo_t*, size_t,
//   killMonsterMap&, bool)" 不符（日志契约不一致）。父泳道在 .h 加 typedef +
//   `size_t` 拼写后，此处改回宏 ⇒ 生成同名局部静态符号且两串逐字节 = ORIG。
bool CKillMonsterInfo::ConvertStructToMap(
    stMonsterInfo_t* pInfo, size_t count,
    killMonsterMap& outMap, bool bAPC)
{
    unsigned int mobIdx;
    unsigned char diff;
    unsigned int i;
    void* pMon;
    void* pAI;
    for (i = 0; i < count; ++i)
    {
        mobIdx = pInfo[i].mobIdx;
        if (mobIdx == 0)
            break;
        if (!bAPC)
        {
            pMon = G_CDataManager()->find_monster(mobIdx);
            if (pMon == 0)
            {
                // [L6S5 §6.5 + P139] ORIG 0854cb42 的 __PRETTY_FUNCTION__ 串
                //   （@0x08c9ec80 size 0x5b）= "bool CKillMonsterInfo::
                //   ConvertStructToMap(stMonsterInfo_t*, size_t, killMonsterMap&,
                //   bool)"。typedef/`size_t` 形参拼写由 CKillMonsterInfo.h 提供
                //   ⇒ 宏展开逐字节匹配（旧展开式 0xd1 系 unsigned int + 展开 map）。
                LogManager::logFormat(
                    1, "KillMonsterInfo.cpp",
                    __PRETTY_FUNCTION__, 0x25,
                    "ConvertStrToMap ERROR : mobIdx is not valid(%u)", mobIdx);
                continue;
            }
        }
        if (bAPC)
        {
            pAI = G_CDataManager()->m_aiCharacterList->get(mobIdx);
            if (pAI == 0)
            {
                LogManager::logFormat(
                    1, "KillMonsterInfo.cpp",
                    __PRETTY_FUNCTION__, 0x2e,
                    "ConvertStrToMap ERROR : mobIdx is not valid(%u)", mobIdx);
                continue;
            }
        }
        diff = pInfo[i].diff;
        outMap.insert(std::make_pair(mobIdx, diff));
    }
    return 1;
}