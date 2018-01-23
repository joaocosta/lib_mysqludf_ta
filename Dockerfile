# certain queries crash the mariadb server starting with 10.2+.  See examples/mariadb10.2.crash .
FROM mariadb:10.1
MAINTAINER Joao Costa <joaocosta@zonalivre.org>

RUN apt-get update && apt-get install -y \
        autoconf \
        automake \
        gcc \
        libmariadbclient-dev \
        libtool \
        make \
        && rm -rf /var/lib/apt/lists/*

ADD . /root/libmysqludfta

WORKDIR /root/libmysqludfta

RUN ./autogen.sh && ./configure && make install
RUN cp setup/*_up.sql /docker-entrypoint-initdb.d/.

# docker build  --tag mysqludf/tatest .
# docker run --name fxdata -v $HOME/mysql:/var/lib/mysql -v $HOME/fx/cfg/mariadb:/etc/mysql/conf.d --hostname=datatest -e MYSQL_ROOT_PASSWORD=root -e MYSQL_DATABASE=fxdata -d --name fxdatatest lib_mysqludf/tatest
# docker run -it --link fxdatatest:mysql --rm mariadb sh -c 'exec mysql -h"$MYSQL_PORT_3306_TCP_ADDR" -P"$MYSQL_PORT_3306_TCP_PORT" -uroot -p"$MYSQL_ENV_MYSQL_ROOT_PASSWORD"'
# https://hub.docker.com/_/mariadb/
