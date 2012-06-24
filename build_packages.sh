#!/bin/sh

set -e

INSTALLDIR=/tmp/libmysqludf_build_install

rm -fR $INSTALLDIR
mkdir -p $INSTALLDIR

./configure
make
make install DESTDIR=$INSTALLDIR

fpm -t deb -d "mysql-server (>= 5.1)" --description "Trading technical analysis functions as MySQL UDFs" --url https://github.com/joaocosta/lib_mysqludf_ta/ -m "joaocosta@zonalivre.org" -s dir -n libmysqludf_ta -v 0.1 -C $INSTALLDIR usr
fpm -t rpm -d "mysql-server >= 5.1" --description "Trading technical analysis functions as MySQL UDFs" --url https://github.com/joaocosta/lib_mysqludf_ta/ -m "joaocosta@zonalivre.org" -s dir -n libmysqludf_ta -v 0.1 -C $INSTALLDIR usr

rm -fR $INSTALLDIR
