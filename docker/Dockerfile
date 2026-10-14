# syntax=docker/dockerfile:1
# =====================================================================
# rednf：从源码构建 DNF 1031 服务端（remake）并组装运行镜像
#   docker compose -f docker/compose.yaml build
# 构建上下文 = 仓库根目录（需要已 git submodule update --init .origin_1031）
# =====================================================================
ARG DEBIAN_IMAGE=debian:trixie

# ---------- 阶段 1：构建 ----------
FROM ${DEBIAN_IMAGE} AS build
ARG DNF_MIRROR=https://vault.centos.org
ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl git cmake make g++ g++-multilib gcc-multilib \
        rpm2cpio cpio python3 libssl-dev:i386 xz-utils \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY toolchains/bootstrap.sh toolchains/bootstrap.sh
COPY toolchains/src toolchains/src
COPY .origin_1031/build/dnf_data/home/template/neople/game/libnxencryption.so \
     .origin_1031/build/dnf_data/home/template/neople/game/libnxencryption.so
RUN DNF_MIRROR=${DNF_MIRROR} bash toolchains/bootstrap.sh
# 登录网关 dnf-gate-server（llnut/dnf-login 发布包，校验 sha256；不进仓库）
ARG GATE_VERSION=0.2.1
RUN A=dnf-gate-server-${GATE_VERSION}-x86_64-unknown-linux-musl \
 && U=https://github.com/llnut/dnf-login/releases/download/${GATE_VERSION}/$A.tar.gz \
 && mkdir -p /out && curl -fsSL -o /tmp/g.tgz "$U" && curl -fsSL -o /tmp/g.sha "$U.sha256" \
 && [ "$(awk '{print $1}' /tmp/g.sha)" = "$(sha256sum /tmp/g.tgz | awk '{print $1}')" ] \
 && tar -xzf /tmp/g.tgz --strip-components=1 -C /out "$A/dnf-gate-server" && rm -f /tmp/g.tgz /tmp/g.sha
COPY remake remake
RUN cmake -S remake/source -B remake/build \
 && cmake --build remake/build -j"$(nproc)" \
 && gcc -m32 -O2 -shared -fPIC -o remake/build/libtss_shmfix.so remake/runtime/libtss_shmfix.c -ldl \
 && mkdir -p /out/bin && for s in auction bridge channel community coserver dbmw game guild manager monitor point relay statics stun; do \
        cp remake/build/$s/df_${s}_r /out/bin/; done \
 && cp remake/build/geoip/libGeoIP.so.1 remake/build/libtss_shmfix.so /out/bin/

# ---------- 阶段 2：运行 ----------
FROM ${DEBIAN_IMAGE} AS runtime
ENV DEBIAN_FRONTEND=noninteractive TZ=Asia/Shanghai
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --no-install-recommends \
        libc6:i386 libgcc-s1:i386 libstdc++6:i386 zlib1g:i386 libjemalloc2:i386 \
        bash procps psmisc socat netcat-openbsd mariadb-client tzdata ca-certificates openssl geoip-database \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /out/dnf-gate-server /usr/bin/dnf-gate-server
# 上游（llnut/dnf 清风 1031）运行时：配置模板、secsvr、TSS SDK、PVF 等
COPY .origin_1031/build/dnf_data/home/template /home/template
COPY .origin_1031/build/dnf_data/TeaEncrypt /usr/local/bin/TeaEncrypt
# remake 产物覆盖上游 ORIG ELF
COPY --from=build /out/bin /opt/remake/bin
COPY docker/runtime/ /opt/remake/runtime/
COPY docker/mysql/initdb/20-periodic-tables.sh /opt/remake/runtime/periodic_tables.sh
RUN chmod +x /usr/local/bin/TeaEncrypt /opt/remake/runtime/*.sh \
 && for s in auction bridge channel community coserver guild manager monitor point relay statics stun; do \
        cp /opt/remake/bin/df_${s}_r /home/template/neople/$s/; done \
 && for d in dbmw_guild dbmw_mnt dbmw_stat; do cp /opt/remake/bin/df_dbmw_r /home/template/neople/$d/; done \
 && cp /opt/remake/bin/df_game_r /opt/remake/bin/libGeoIP.so.1 /opt/remake/bin/libtss_shmfix.so /home/template/neople/game/
VOLUME ["/data"]
ENTRYPOINT ["/opt/remake/runtime/entrypoint.sh"]
