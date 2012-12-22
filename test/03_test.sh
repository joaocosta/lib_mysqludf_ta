#!/bin/sh

set -e

mysql -N -uroot -e 'SELECT datetime, ta_ema(close,21) FROM goog_86400' libmysqludf_tests > ema
diff ema test/known_values/ema
rm ema

mysql -N -uroot -e 'SELECT datetime, ta_max(close,21) FROM goog_86400' libmysqludf_tests > max
diff max test/known_values/max
rm max

mysql -N -uroot -e 'SELECT datetime, ta_min(close,21) FROM goog_86400' libmysqludf_tests > min
diff min test/known_values/min
rm min

mysql -N -uroot -e 'SELECT datetime, ta_previous(close,21) FROM goog_86400' libmysqludf_tests > previous
diff previous test/known_values/previous
rm previous

mysql -N -uroot -e 'SELECT datetime, ta_rsi(close,14) FROM goog_86400' libmysqludf_tests > rsi
diff rsi test/known_values/rsi
rm rsi

mysql -N -uroot -e 'SELECT datetime, ta_sma(close,21) FROM goog_86400' libmysqludf_tests > sma
diff sma test/known_values/sma
rm sma

mysql -N -uroot -e 'SELECT datetime, ta_stddevp(close,21) FROM goog_86400' libmysqludf_tests > stddevp
diff stddevp test/known_values/stddevp
rm stddevp

mysql -N -uroot -e 'SELECT datetime, ta_sum(close,21) FROM goog_86400' libmysqludf_tests > sum
diff sum test/known_values/sum
rm sum

mysql -N -uroot -e 'SELECT datetime, ta_tr(high,low,close) FROM goog_86400' libmysqludf_tests > tr
diff tr test/known_values/tr
rm tr
