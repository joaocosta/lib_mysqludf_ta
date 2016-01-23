#!/bin/bash

set -e

exit 0
# Fetch historical data from finance.yahoo.com

URL="http://ichart.finance.yahoo.com/table.csv?s=GOOG&d=11&e=22&f=2012&g=d&a=7&b=19&c=2004&ignore=.csv";

curl -s "$URL" | \
grep -v "Date,Open,High,Low,Close,Volume" | \
perl -e '@a=<STDIN>;print join("", reverse @a)' | \
perl -ne '@a=split(","); pop @a; print join(",", @a), "\n"' > goog_86400
