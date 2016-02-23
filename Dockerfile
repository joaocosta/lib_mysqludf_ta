FROM mariadb
MAINTAINER Joao Costa <joaocosta@zonalivre.org>

RUN apt-get update && apt-get install -y \
        autoconf \
        automake \
        gcc \
        libmariadbclient-dev \
        libtool \
        make

WORKDIR /root

RUN ./autogen.sh && ./configure && make install
RUN cp setup/*_up.sql /docker-entrypoint-initdb.d/.

# docker run -v /my/datadir:/var/lib/mysql --name fxdata -e MYSQL_ROOT_PASSWORD=root -e MYSQL_DATABASE=fxcm -d lib_mysqludf_ta
# docker run -it --link fxdata:mysql --rm mariadb sh -c 'exec mysql -h"$MYSQL_PORT_3306_TCP_ADDR" -P"$MYSQL_PORT_3306_TCP_PORT" -uroot -p"$MYSQL_ENV_MYSQL_ROOT_PASSWORD"'
# https://hub.docker.com/_/mariadb/
