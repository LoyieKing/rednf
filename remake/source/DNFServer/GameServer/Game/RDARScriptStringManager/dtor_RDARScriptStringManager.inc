// b28d-21: IDENTICAL（91=91）— 迭代器直构（P50 反例复活）+ end 循环外缓存（P13）
// + 后置 `it++`（P28 增量侧 ppEi 判据）+ `delete *it` 无具名 d/无显式判空。
// ORIG D1 0x8aa4f04。
RDARScriptStringManager::~RDARScriptStringManager()
{
    // ORIG D1 0x8aa4f04：begin/end 迭代器循环逐元素 delete（与 release 同形），
    // 随后成员析构。end 循环外求值一次（P13），迭代器先声明后赋值（P50）。
    std::vector<StringDictionary*>::iterator it = dictionaries_.begin();
    std::vector<StringDictionary*>::iterator e = dictionaries_.end();
    for (; it != e; it++)   // ORIG 增量段 ppEi（后置，P28 推论）
    {
        delete *it;         // ORIG 无具名 d/无显式判空（delete 自带守卫）
    }
}