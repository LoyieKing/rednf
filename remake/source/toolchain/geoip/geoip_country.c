/* ============================================================================
 * libGeoIP.so.1 —— 真实可用的 GeoIP Legacy **Country edition** 实现
 *
 * 为什么需要它：`df_game_r` 的台湾地区限制（RestrictGeolocation）经
 * GeoIP_new / GeoIP_country_code_by_addr / GeoIP_delete 走 libGeoIP；
 * 本工程原先链的是 `toolchains/geoip_stub/libGeoIP.so.1`（三个符号全返回
 * 0/NULL 的链接桩）⇒ 地理限制形同虚设。本实现是
 * https://github.com/llnut/dnf-compat-layer 里 `geoip-compat`（Rust）的
 * C 语言等价移植，语义逐条对齐：
 *   · 数据库格式：MaxMind GeoIP Legacy Country edition（二进制 trie）
 *   · 版本校验：从文件尾部向前找 `FF FF FF <edition>`（窗口 1KB，取最后一个）；
 *     edition=1 即 Country；**找不到标记时按 MaxMind 行为回退为 Country**
 *     （老 Country 库——如 CentOS7 容器里那份 1.1MB 的 GeoIP.dat——尾部只有版权
 *     文本、没有标记；MaxMind 1.4.8 对这类库同样走"默认 Country"分支）
 *   · trie 走位：32 位 IPv4 从高位到低位，记录 3 字节（小端拼接 24 位）；
 *     值 ≥ GEOIP_COUNTRY_BEGIN(0xFFFF00) 即国家编号（值 − 基址）
 *   · 国家码表：256 项静态表，"--"（含越界）⇒ 返回 NULL
 *   · 只接受 IPv4 字面量（不做 DNS 解析；前导 0 与 Rust 解析器同样被拒）
 *
 * 导出（与容器内已部署的 Rust 版逐符号一致）：
 *   GeoIP_new / GeoIP_open / GeoIP_country_code_by_addr /
 *   GeoIP_country_code_by_name / GeoIP_delete
 *
 * 默认数据库搜索路径（同上游 README）：
 *   /usr/local/share/GeoIP/GeoIP.dat, /usr/share/GeoIP/GeoIP.dat,
 *   /var/lib/GeoIP/GeoIP.dat, /data/GeoIP/GeoIP.dat
 * ==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GEOIP_COUNTRY_EDITION 1
#define GEOIP_COUNTRY_BEGIN   16776960UL      /* 0xFFFF00 */
#define GEOIP_RECORD_LEN      3

typedef struct GeoIP
{
    unsigned char* data;
    unsigned long  size;
    unsigned long  segments;
} GeoIP;

/* MaxMind GeoIP Legacy 国家码表（索引 = 国家编号；0 与 255 为占位 "--"）。 */
static const char* const geoip_country_codes[256] = {
    "--", "AP", "EU", "AD", "AE", "AF", "AG", "AI",
    "AL", "AM", "CW", "AO", "AQ", "AR", "AS", "AT",
    "AU", "AW", "AZ", "BA", "BB", "BD", "BE", "BF",
    "BG", "BH", "BI", "BJ", "BM", "BN", "BO", "BR",
    "BS", "BT", "BV", "BW", "BY", "BZ", "CA", "CC",
    "CD", "CF", "CG", "CH", "CI", "CK", "CL", "CM",
    "CN", "CO", "CR", "CU", "CV", "CX", "CY", "CZ",
    "DE", "DJ", "DK", "DM", "DO", "DZ", "EC", "EE",
    "EG", "EH", "ER", "ES", "ET", "FI", "FJ", "FK",
    "FM", "FO", "FR", "SX", "GA", "GB", "GD", "GE",
    "GF", "GH", "GI", "GL", "GM", "GN", "GP", "GQ",
    "GR", "GS", "GT", "GU", "GW", "GY", "HK", "HM",
    "HN", "HR", "HT", "HU", "ID", "IE", "IL", "IN",
    "IO", "IQ", "IR", "IS", "IT", "JM", "JO", "JP",
    "KE", "KG", "KH", "KI", "KM", "KN", "KP", "KR",
    "KW", "KY", "KZ", "LA", "LB", "LC", "LI", "LK",
    "LR", "LS", "LT", "LU", "LV", "LY", "MA", "MC",
    "MD", "MG", "MH", "MK", "ML", "MM", "MN", "MO",
    "MP", "MQ", "MR", "MS", "MT", "MU", "MV", "MW",
    "MX", "MY", "MZ", "NA", "NC", "NE", "NF", "NG",
    "NI", "NL", "NO", "NP", "NR", "NU", "NZ", "OM",
    "PA", "PE", "PF", "PG", "PH", "PK", "PL", "PM",
    "PN", "PR", "PS", "PT", "PW", "PY", "QA", "RE",
    "RO", "RU", "RW", "SA", "SB", "SC", "SD", "SE",
    "SG", "SH", "SI", "SJ", "SK", "SL", "SM", "SN",
    "SO", "SR", "ST", "SV", "SY", "SZ", "TC", "TD",
    "TF", "TG", "TH", "TJ", "TK", "TM", "TN", "TO",
    "TL", "TR", "TT", "TV", "TW", "TZ", "UA", "UG",
    "UM", "US", "UY", "UZ", "VA", "VC", "VE", "VG",
    "VI", "VN", "VU", "WF", "WS", "YE", "YT", "RS",
    "ZA", "ZM", "ME", "ZW", "A1", "A2", "O1", "AX",
    "GG", "IM", "JE", "BL", "MF", "BQ", "SS", "--",
};

static const char* const geoip_dat_paths[] = {
    "/usr/local/share/GeoIP/GeoIP.dat",
    "/usr/share/GeoIP/GeoIP.dat",
    "/var/lib/GeoIP/GeoIP.dat",
    "/data/GeoIP/GeoIP.dat",
};

/* 读整个文件；失败返回 NULL（*out_size 不写出）。 */
static unsigned char* geoip_read_file(const char* path, unsigned long* out_size)
{
    FILE* f = fopen(path, "rb");
    long  n;
    unsigned char* buf;

    if (f == 0)
        return 0;
    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return 0;
    }
    n = ftell(f);
    if (n <= 0)
    {
        fclose(f);
        return 0;
    }
    if (fseek(f, 0, SEEK_SET) != 0)
    {
        fclose(f);
        return 0;
    }
    buf = (unsigned char*)malloc((size_t)n);
    if (buf == 0)
    {
        fclose(f);
        return 0;
    }
    if (fread(buf, 1, (size_t)n, f) != (size_t)n)
    {
        free(buf);
        fclose(f);
        return 0;
    }
    fclose(f);
    *out_size = (unsigned long)n;
    return buf;
}

/* 校验 edition 并建句柄；data 的所有权转交句柄。 */
static GeoIP* geoip_parse(unsigned char* data, unsigned long size)
{
    long start;
    long i;
    long marker = -1;
    GeoIP* gi;

    if (data == 0 || size < 10)
        return 0;

    /* 尾部 1KB 内**从后往前**找 marker，取最后一个（真 marker 在最后，
     * 数据段里也可能出现 FF FF FF，从后往前可优先命中真 marker）。 */
    start = size > 1024 ? (long)size - 1024 : 0;
    for (i = (long)size - 4; i >= start; i--)
    {
        if (data[i] == 0xFF && data[i + 1] == 0xFF && data[i + 2] == 0xFF)
        {
            marker = i;
            break;
        }
    }
    if (marker >= 0)
    {
        /* 有 marker ⇒ 按 edition 判定：只支持 Country（1），其余（City/Org/…）
         * 明确拒绝，避免把非 Country 库当 Country 解析出错误国家码。 */
        if (data[marker + 3] != GEOIP_COUNTRY_EDITION)
            return 0;
    }
    /* marker 缺失（老 Country 库）或 edition=1（标准 Country 库）都按 Country 处理：
     * 前者即 MaxMind 1.4.8 `_setup_segments` 的默认分支（默认 Country edition +
     * GEOIP_COUNTRY_BEGIN），后者与上游 dnf-compat-layer 的 geoip-compat 一致。 */

    gi = (GeoIP*)malloc(sizeof(GeoIP));
    if (gi == 0)
        return 0;
    gi->data = data;
    gi->size = size;
    gi->segments = GEOIP_COUNTRY_BEGIN;
    return gi;
}

static GeoIP* geoip_open_default(void)
{
    unsigned long n = sizeof(geoip_dat_paths) / sizeof(geoip_dat_paths[0]);
    unsigned long i;

    for (i = 0; i < n; i++)
    {
        unsigned long size = 0;
        unsigned char* data = geoip_read_file(geoip_dat_paths[i], &size);
        if (data == 0)
            continue;
        GeoIP* gi = geoip_parse(data, size);
        if (gi != 0)
            return gi;
        free(data);
    }
    return 0;
}

/* 返回国家编号；-1 = 数据不足/无结果（对应上游 Option::None）。 */
static long geoip_lookup_country_id(const GeoIP* gi, unsigned long ipnum)
{
    unsigned long offset = 0;
    int depth;

    for (depth = 31; depth >= 0; depth--)
    {
        unsigned long bit = (ipnum >> depth) & 1UL;
        unsigned long rec = offset * 2 * GEOIP_RECORD_LEN + bit * GEOIP_RECORD_LEN;
        unsigned long val;

        if (rec + GEOIP_RECORD_LEN > gi->size)
            return -1;
        val = (unsigned long)gi->data[rec]
              | ((unsigned long)gi->data[rec + 1] << 8)
              | ((unsigned long)gi->data[rec + 2] << 16);
        if (val >= gi->segments)
            return (long)(val - gi->segments);
        offset = val;
    }
    return -1;
}

/* 严格 IPv4 字面量解析（拒绝前导 0，与 Rust Ipv4Addr::from_str 行为一致）。 */
static int geoip_parse_ipv4(const char* s, unsigned long* out)
{
    unsigned long ip = 0;
    int part;
    const char* p = s;

    if (s == 0)
        return 0;
    for (part = 0; part < 4; part++)
    {
        unsigned long v = 0;
        int digits = 0;
        const char* first;

        if (*p < '0' || *p > '9')
            return 0;
        first = p;
        while (*p >= '0' && *p <= '9')
        {
            v = v * 10 + (unsigned long)(*p - '0');
            if (v > 255)
                return 0;
            p++;
            digits++;
        }
        if (digits > 1 && *first == '0')
            return 0;
        if (part < 3)
        {
            if (*p != '.')
                return 0;
            p++;
        }
        else if (*p != '\0')
        {
            return 0;
        }
        ip = (ip << 8) | v;
    }
    *out = ip;
    return 1;
}

/* ---------------------------- C ABI 导出 ---------------------------- */

GeoIP* GeoIP_new(int db_type)
{
    (void)db_type;                     /* 与上游一致：参数仅为 API 兼容 */
    return geoip_open_default();
}

GeoIP* GeoIP_open(const char* path, int flags)
{
    unsigned long size = 0;
    unsigned char* data;

    (void)flags;
    if (path == 0)
        return geoip_open_default();
    data = geoip_read_file(path, &size);
    if (data == 0)
        return 0;
    {
        GeoIP* gi = geoip_parse(data, size);
        if (gi == 0)
            free(data);
        return gi;
    }
}

const char* GeoIP_country_code_by_addr(GeoIP* gi, const char* addr)
{
    unsigned long ipnum;
    long country_id;

    if (gi == 0 || addr == 0)
        return 0;
    if (!geoip_parse_ipv4(addr, &ipnum))
        return 0;
    country_id = geoip_lookup_country_id(gi, ipnum);
    if (country_id < 0 || country_id > 255)
        return 0;
    if (strcmp(geoip_country_codes[country_id], "--") == 0)
        return 0;
    return geoip_country_codes[country_id];
}

const char* GeoIP_country_code_by_name(GeoIP* gi, const char* name)
{
    return GeoIP_country_code_by_addr(gi, name);
}

void GeoIP_delete(GeoIP* gi)
{
    if (gi != 0)
    {
        free(gi->data);
        free(gi);
    }
}
