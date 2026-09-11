// ORIG 0x088bc068：装载名字表文件（name id 对），插入 STNameList；
// 插入失败（重复键）时记日志；返回列表是否非空（推断：ORIG 尾部 begin()==end() 判断）。
// [b28d-lane16 DIFF] ORIG 088bc068：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
bool loadRDARNameFile(STNameList* list, const char* dir, const char* file)
{
    if (!loadRDARScriptFile(dir, file))
        return false;
    list->Clear();

    // [L3] P309 头内声明序按 ORIG 槽浅→深重排：ORIG dead(-0xc) 最浅、ok(-0x2d)、
    // name(-0x34) 最深 ⇒ 4.4 头内「先声明浅」故 dead→ok→name。
    // [L10-2] 判据⑫ widthscan：ORIG `clr_b=0`（无带初始化器的 bool 局部）⇒
    // 按 P171 照抄去掉 `= false`；ok 由 ScanInt 写入后读取，语义等价。
    bool ok;
    std::string name;
    // [L3] ORIG 0x088bc0a9 `movl $0x1,-0xc(%ebp)`，全函数无读点 = 原码遗留的
    // 死初始化具名 int 局部（P171/P232 照抄原则）；补回以复现 ORIG 槽图。
    int deadFlag = 1;
    for (;;)
    {
        if (!ScanStr(&name))
            break;
        int id = ScanInt(&ok);
        if (!ok)
            break;
        // [b28d-lane16 P192] ORIG 0x088bc068 直接调 StringUnorderedMap::insert(PKc,RKi)
        // 并用返回值 .second 判重复（ORIG nm 无 STNameList::Insert/IsEmpty 符号）；
        // 原 GetID+Insert 两查形多 EXTRA c_str/GetID 并缺 insert 调用。
        StringUnorderedMap<CharStringHashTrait, int>::InsertResult r =
            list->m_map.insert(name.c_str(), id);
        if (!r.second)
        {
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 1000,
                "ITEM NAME FILE LIST LOAD FAIL! [%s%s][%s]", dir, file,
                name.c_str());
        }
    }
    // ORIG 尾部 begin()==end() 直发（无 STNameList::IsEmpty 符号；p167 用 eq 非 ne）。
    return !(list->m_map.begin() == list->m_map.end());
}