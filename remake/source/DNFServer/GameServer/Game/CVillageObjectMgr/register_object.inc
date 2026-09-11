bool CVillageObjectMgr::register_object(Zone zone, IObject* object)
{
    // [M5E round6 判据 ①/② 已改] ORIG 0x086b6f58 三条直证：
    //  · 极性：+0x046 `call _ZNKSt17_Rb_tree_iteratorI...EneERKSA_`（ne）+ `je` 落
    //    插入分支 ⇒ ORIG 源码是**正极性** `if (it != m_objects.end())`（旧码 eqERKSA_
    //    + 交换分支）。
    //  · 具名列表指针：+0x055 ptEv 后 `add $0x4,%eax; mov %eax,-0xc(%ebp)`
    //    （`list* pList = &it->second;`），随后 +0x067/+0x087 两次
    //    `mov -0xc(%ebp),%eax; mov %eax,(%esp)` ⇒ 全程只 1 次 ptEv（旧码 2 次）。
    //  · 帧：ORIG `sub $0x50,%esp`（旧码 0x40，少 1 个 4B 具名局部 ⇒ 差 0x10）。
    // ORIG 0x86b6f58：无该 Zone 则新建列表插入，否则先 detect_collision 碰撞
    // 检测（命中返 0），通过才 push_back；成功返 1。
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it != m_objects.end())
    {
        std::list<IObject*>* pList = &it->second;
        if (collision_detection::detect_collision(*pList, object))
            return 0;
        pList->push_back(object);
    }
    else
    {
        std::list<IObject*> newList;
        newList.push_back(object);
        m_objects.insert(std::make_pair(zone, newList));
    }
    return 1;
}