// ORIG T _ZN14village_object17CVillageObjectMgr17unregister_objectENS_4ZoneEPNS_7IObjectE @0x86b70d0
// objdump 逐指令转写（无独立 md 报告，自行导出并标记）：map find→end 判定→std::find→erase。
// [b28d-solo-CPrivateStoreMgr] 返回类型 int→bool（P292 档①，mangled 不含返回类型）。
bool village_object::CVillageObjectMgr::unregister_object(village_object::Zone zone,
                                                          village_object::IObject* object)
{
    std::map<village_object::Zone, std::list<village_object::IObject*> >::iterator it =
        m_objects.find(zone);
    if (it == m_objects.end())
        return 0;
    std::list<village_object::IObject*>& objects = it->second;   // ORIG 用 operator->（pt）
    std::list<village_object::IObject*>::iterator found =
        std::find(objects.begin(), objects.end(), object);
    if (found == objects.end())
        return 0;
    objects.erase(found);
    return 1;
}