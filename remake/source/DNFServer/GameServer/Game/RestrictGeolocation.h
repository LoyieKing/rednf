#ifndef DNF_GAME_RESTRICT_GEOLOCATION_H_
#define DNF_GAME_RESTRICT_GEOLOCATION_H_

// ============================================================================
// df_game_r 还原 —— RestrictGeolocation（localtaiwan/System/RestrictGeolocation.cpp）。
// 权威声明点（原 GlobalData.cpp L920 局部镜像删除，定义收敛于 RestrictGeolocation.cpp；
// Main 批准：W6_ARAD）。
//
// ORIG nm -S --defined-only df_game_r（0x0817097c..0x08170fed 区）：
//   C1 0x0817097c / D1 0x081709fe / initGeolocation 0x08170a46 /
//   closeGeolocation 0x08170a88 / clearAllowCountry 0x08170b16 /
//   isAllowCountryCode 0x0817109a / addAllowCountry 0x08170a9e /
//   getCountryCode 0x08170f3a / loadData 0x08170fec /
//   isAllow(string,string) 0x08170b2c / isAllow(string) 0x08170ecc
// ============================================================================

#include <string>
#include <set>

class RestrictGeolocation
{
public:
    void loadData();                    // ORIG T 0x08170fec
    void addAllowCountry(std::string country);   // ORIG T 0x08170a9e
    bool isAllow(std::string account, std::string ip);  // ORIG T 0x08170b2c
    bool isAllow(std::string ip);       // ORIG T 0x08170ecc
    bool isAllowCountryCode(std::string country); // ORIG T 0x0817109a
    void getCountryCode(char* out, unsigned int outLen, std::string ip);  // ORIG T 0x08170f3a
    bool initGeolocation();             // ORIG T 0x08170a46
    void closeGeolocation();            // ORIG T 0x08170a88
    void clearAllowCountry();           // ORIG T 0x08170b16

    RestrictGeolocation();              // ORIG C1 0x0817097c / C2 同体
    ~RestrictGeolocation();             // ORIG D1 0x081709fe

private:
    void* m_geoip;                      // +0x00 GeoIP 实例指针（GeoIP_new 返回值）
    // ORIG 成员为 boost::unordered_set<std::string>（+0x04，addAllowCountry/
    // clearAllowCountry 报告）；重建以 std::set 做语义等价【推断】。
    std::set<std::string> m_allowCountry;
};

#endif  // DNF_GAME_RESTRICT_GEOLOCATION_H_