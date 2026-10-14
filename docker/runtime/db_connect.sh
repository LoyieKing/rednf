#!/bin/bash
# 把 d_taiwan.db_connect 指向容器内 127.0.0.1:3306/3307 转发，并创建 game 账号
set -euo pipefail
G=$SERVER_GROUP; D=$SERVER_GROUP_DB; P=$DEC_GAME_PWD
mysql -h"$DB_HOST" -P"$DB_PORT" -uroot -p"$DNF_DB_ROOT_PASSWORD" <<SQL
CREATE USER IF NOT EXISTS 'game'@'%' IDENTIFIED BY '$DNF_DB_GAME_PASSWORD';
ALTER USER 'game'@'%' IDENTIFIED BY '$DNF_DB_GAME_PASSWORD';
GRANT ALL PRIVILEGES ON *.* TO 'game'@'%';
FLUSH PRIVILEGES;
USE d_taiwan;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3307', db_passwd='$P', db_name='d_taiwan'           WHERE db_server_group=$G AND db_type=1;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3307', db_passwd='$P', db_name='d_taiwan_secu'      WHERE db_server_group=$G AND db_type=10;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3307', db_passwd='$P', db_name='d_technical_report' WHERE db_server_group=$G AND db_type=15;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='d_guild'                    WHERE db_server_group=$G AND db_type=8;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_$D'                 WHERE db_server_group=$G AND db_type=2;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_${D}_2nd'           WHERE db_server_group=$G AND db_type=3;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_${D}_log'           WHERE db_server_group=$G AND db_type=4;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_${D}_web'           WHERE db_server_group=$G AND db_type=5;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_login'              WHERE db_server_group=$G AND db_type=6;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_prod'               WHERE db_server_group=$G AND db_type=7;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_game_event'         WHERE db_server_group=$G AND db_type=9;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_login_play'         WHERE db_server_group=$G AND db_type=11;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_${D}_auction_gold'  WHERE db_server_group=$G AND db_type=12;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_se_event'           WHERE db_server_group=$G AND db_type=13;
UPDATE db_connect SET db_ip='127.0.0.1', db_port='3306', db_passwd='$P', db_name='taiwan_billing'            WHERE db_server_group=$G AND db_type=14;
SQL
echo "[rednf] db_connect 已更新（server_group=$G, db=$D）"
