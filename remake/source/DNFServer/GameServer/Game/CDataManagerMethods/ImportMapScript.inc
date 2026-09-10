bool ImportMapScript(STMapScript* script, const char* path);

// importSpPerLevelReferenceTable：ORIG T 0x08910505（脚本导入域实现）。
// stSpPerLevelTable 在 ORIG 中 W ctor/dtor 与空 vector<unsigned int> 一致
// （sizeof 0xc），本 TU 以派生视图获得内联生命周期与继承 operator[]，
// 引用形参产生与 ORIG 一致的 mangled 名 R17stSpPerLevelTable。
