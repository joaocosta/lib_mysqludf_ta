#!/bin/bash

set -e

DB_SCHEMA=$(cat <<'EOF'
DROP DATABASE IF EXISTS libmysqludf_tests;
CREATE DATABASE libmysqludf_tests;

use libmysqludf_tests;

CREATE TABLE goog_86400 (
datetime DATETIME NOT NULL ,
open DECIMAL(9,4) NOT NULL ,
high DECIMAL(9,4) NOT NULL ,
low DECIMAL(9,4) NOT NULL ,
close DECIMAL(9,4) NOT NULL ,
PRIMARY KEY ( datetime )
) ENGINE = MYISAM ;
EOF
)

echo $DB_SCHEMA | mysql -uroot

mysqlimport --ignore --local --fields-terminated-by=',' --lines-terminated-by='\n' -s -uroot libmysqludf_tests test/goog_86400
