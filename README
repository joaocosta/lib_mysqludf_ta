DESCRIPTION:

Implements technical analysis functions as MySQL UDFs.

Currently implemented functions are:

TA_SMA
TA_EMA
TA_RSI
TA_TR (True Range)
TA_SUM (Running sum, as opposed to aggregate sum provided by mysql)
TA_PREVIOUS

Other indicators which can be derived from those functions include:

MACD
Bollinger Bands
ADX

Possibly others


AVAILABILITY:

Source repository at:
http://github.com/joaocosta/lib_mysqludf_ta



COMPILING:

Windows:

A binary dll is shipped with this release.

Copy it to the MySQL plugin directory.
To find out what the MySQL plugin directory is run this in a MySQL prompt:

show variables like 'plugin_dir';


The following article describes how to compile MySQL UDFs in Windows:
http://rpbouman.blogspot.com/2007/09/creating-mysql-udfs-with-microsoft.html


Linux:

./configure
make
sudo make install

If this fails, try running autogen.sh and repeat the "configure && make && make install" sequence

For more information on compiling MySQL UDFs:
http://dev.mysql.com/doc/refman/5.1/en/udf-compiling.html
http://dev.mysql.com/doc/refman/5.1/en/adding-udf.html




Install the UDFs

From the MySQL prompt:

Windows

CREATE FUNCTION ta_ema RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_rsi RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_sma RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_sum RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_tr RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_max RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_min RETURNS REAL SONAME 'lib_mysqludf_ta.dll';
CREATE FUNCTION ta_previous RETURNS REAL SONAME 'lib_mysqludf_ta.dll';

Linux

CREATE FUNCTION ta_ema RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_rsi RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_sma RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_sum RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_tr RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_max RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_min RETURNS REAL SONAME 'lib_mysqludf_ta.so';
CREATE FUNCTION ta_previous RETURNS REAL SONAME 'lib_mysqludf_ta.so';

Test your installation

make check  #This assumes a local mysql instance is available, username root with no password


FUNCTIONS:
To try the examples below import the provided sampledb.sql into a MySQL database.

mysqladmin -u root create lib_ta
mysql -u root lib_ta < sampledb.sql
mysql -u root lib_ta
------------------------------------------------------------------------------------------------------

ta_ema - Exponential moving average

ta_ema(
    float data, 
    int period
)

data   - The data to average
period - Running period to calculate for

Example:
To calculate a 50 period EMA of closing prices:

SELECT datetime, ta_ema(close, 50)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_sma - Simple Moving Average ( aka Running average )

ta_sma(
    float data, 
    int period
)

data   - The data to average
period - Running period to calculate for

Example:
To calculate a 50 period SMA of closing prices:

SELECT datetime, ta_sma(close, 50)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_rsi - Relative Strength Index

ta_rsi(
    float data, 
    int period
)

data   - The data to calculate rsi for
period - Running period to calculate for

Example:
To calculate a 14 period RSI of closing prices:

SELECT datetime, ta_rsi(close, 14)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

To calculate a 10 period ta_ema of ta_rsi(14):

SELECT datetime, ta_ema(ta_rsi(close, 14), 10)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_stddevp - Running Standard Deviation

ta_stddevp(
    float data, 
    int period
)

data   - The data to calculate on
period - Running period to calculate for

Example:

This function is useful for calculating indicators such as Bollinger Bands.

To calculate the upper limit Bollinger Band of 2 standard deviations over a 21 period sma:
SELECT datetime, ( ta_sma(close,21) + 2*ta_stddevp(close,21) ) AS `BOL_UP`
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

To calculate the lower limit Bollinger Band of 2 standard deviations over a 21 period sma:
SELECT datetime, ( ta_sma(close,21) - 2*ta_stddevp(close,21) ) AS `BOL_DOWN`
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_sum - Running Sum ( as opposed to aggregate sum )

ta_sum(
    float data, 
    int period
)

data   - The data to average
period - Running period to calculate for

Example:
To calculate a 50 running sum of closing prices:

SELECT datetime, ta_sum(close, 50)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;





------------------------------------------------------------------------------------------------------

ta_tr - Calculate True Range

ta_tr(
    float high,
    float low,
    float close
)

high   - The Highest price for the period
low    - The Lowest price for the period
close  - The Closing price of the period

Example:
To calculate True Range

SELECT datetime, ta_tr(high, low, close)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_previous - Calculate True Range

ta_previous(
    float data,
    int period
)

data   - The data to lookback into
period - Number of periods to look back into


Example:
See if today's close is greater than yesterday's close
SELECT datetime, close > ta_previous(close,1)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_max - Running Max

ta_max(
    float data,
    int period
)

data   - The data to search the maximum for
period - Running period to calculate for


Example:
To return the maximum close over the last 50 periods:
SELECT datetime, ta_max(close, 50)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

ta_min - Running Min

ta_min(
    float data,
    int period
)

data   - The data to search the minimum for
period - Running period to calculate for


Example:
To return the minimum close over the last 50 periods:
SELECT datetime, ta_min(close, 50)
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

------------------------------------------------------------------------------------------------------

macd - Moving Average Convergence / Divergence

MACD is defined as the difference between two emas

SELECT datetime, ta_ema(close,12) - ta_ema(close,26) AS `MACD`
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;

The MACD signal line is defined as an EMA of MACD
SELECT datetime, ta_ema(ta_ema(close,12) - ta_ema(close,26), 9) AS `MACD_SIGNAL`
FROM ( SELECT * FROM EURUSD_86400 ORDER BY datetime ASC ) AS T;


------------------------------------------------------------------------------------------------------

USING WITH INTEGER DATA INSTEAD OF FLOATS:

Financial data tends to be stored as floats, however sometimes it might be useful to run these functions with integer data.  The quickest way to achieve this is to use MySQL CAST:

SELECT ta_ema(CAST(integer_data_field AS DECIMAL(65), 14) FROM TABLE;


CREATING NEW FUNCTIONS:

The easiest way would be to copy one of the existing .c files to a different name and modify its implementation.
Edit test/03_test.sh and add a suitable test to validate results of the new function using the provided dataset.
Edit src/db_* and add the new function there
Edit Makefile.am and add the new source file.
To generate a new configure script, run the provided autogen.sh ( this depends on automake, autoconf and libtool being installed ).



CONTACT:
João Costa <joaocosta@zonalivre.org>
