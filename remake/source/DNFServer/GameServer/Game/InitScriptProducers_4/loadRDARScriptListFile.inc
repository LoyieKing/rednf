bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* map);

// ---- STScriptFileList（布局权威：STScriptFileList.cpp 同名本地类，
//      sizeof 0x18 = std::map<int,std::string>；ORIG ctor 0x088bacf8）----


// [L32 GI 调序] ORIG `_GLOBAL__I_DIFFICULTY_OF_DUNGEON_BONUS`
//   （CU=RDARMonsterAbilityFormula.cpp）⇒ 本 TU 首个强符号须为
//   DIFFICULTY_OF_DUNGEON_BONUS（静态初始化 float 表，无构造序影响；P327）。
//   ORIG D 0x93f4a00 [0x104]=13×5，.data 初值为首元 1.0f 其余 0。
float DIFFICULTY_OF_DUNGEON_BONUS[13][5]  = { { 1.0f } };

// ===================== 地图脚本域（InitMapScript）=====================

// ORIG B 0x94fe2a0 [0x100]
char g_szMapScriptBaseDirectory[0x100];
// [b14 归属迁移·Main 批准] B 0x94fe3a0 的 g_stMapScriptFileList **定义**已迁至
// OnlyOrigBatch14.cpp（ORIG 该 TU static_init 0x089e81d2 构造它 + atexit D1
// 0x80f1352，属 P186/P128）；本 TU 仅按 extern 引用（InitMapScript 消费）。
