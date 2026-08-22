#ifndef DNFLEXWRAPPER_H
#define DNFLEXWRAPPER_H

#include <string>

class MySQL;

bool loadRDARScriptFile(const char* directory, const char* path) asm("_Z18loadRDARScriptFilePKcS0_");
void unloadRDARScriptFile() asm("_Z20unloadRDARScriptFilev");
bool ScanType(std::string& token, bool consume) asm("_Z8ScanTypeRSsb");
bool ScanInt(int* value) asm("_Z7ScanIntPi");
int ScanInt(bool* ok) asm("_Z7ScanIntPb");
float ScanFloat(float* value) asm("_Z9ScanFloatPf");
float ScanFloat(bool* ok) asm("_Z9ScanFloatPb");
bool ScanStr(std::string* value) asm("_Z7ScanStrPSs");
void GetEscapeString(MySQL* db, const char* src, char* dst);

#endif
