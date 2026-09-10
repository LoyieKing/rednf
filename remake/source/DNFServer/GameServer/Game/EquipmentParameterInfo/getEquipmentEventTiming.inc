int getEquipmentEventTiming(const std::string& s);     // 0x89c2a81（OnlyOrigBatch11.cpp，"jump attack"→0 否则 1）
// ORIG 0x088be60：职业枚举 → 作业标签名（STMapScript.cpp 全局 T）。
const char* getCharacterJobName(ENUM_CHARACTERJOB job);
