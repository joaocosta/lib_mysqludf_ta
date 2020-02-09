WITH T AS (
SELECT * FROM EURUSD_86400 ORDER BY datetime ASC LIMIT 93
),
weekly AS (
  SELECT 
  datetime,
  CAST(date_format(date_sub(datetime, interval weekday(datetime)+1 DAY), '%Y-%m-%d 00:00:00') AS DATETIME) AS COMMON_DATETIME,
  max(high) OVER (PARTITION BY COMMON_DATETIME ORDER BY datetime) AS week_high,
  min(low) OVER (PARTITION BY COMMON_DATETIME ORDER BY datetime) AS week_low,
  LAST_VALUE(close) OVER (PARTITION BY COMMON_DATETIME ORDER BY datetime) AS week_close
  FROM T
)
SELECT
datetime,
COMMON_DATETIME,
ta_rsi_win(week_close, 14) OVER (PARTITION BY COMMON_DATETIME ORDER BY datetime) AS tr,
ta_tr_win(week_high, week_low, week_close) OVER (PARTITION BY COMMON_DATETIME ORDER BY datetime) AS tr,
ta_atr_win(week_high, week_low, week_close, 14) OVER (PARTITION BY COMMON_DATETIME ORDER BY datetime) AS atr
FROM weekly;
