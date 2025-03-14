---
title: 预测算法
description: 预测算法
---

import fc_result from '../pic/fc.png';
import fc_result_figure from '../pic/fc-result.png';

时序数据预测处理以持续一个时间段的时序数据作为输入，预测接下来一个连续时间区间内时间序列数据趋势。用户可以指定输出的（预测）时间序列数据点的数量，因此其输出的结果行数不确定。为此，TDengine 使用新 SQL 函数 `FORECAST` 提供时序数据预测服务。基础数据（用于预测的历史时间序列数据）是该函数的输入，预测结果是该函数的输出。用户可以通过 `FORECAST` 函数调用 Anode 提供的预测算法提供的服务。

在后续章节中，使用时序数据表`foo`作为示例，介绍预测和异常检测算法的使用方式，`foo` 表的模式如下：

|列名称|类型|说明|
|---|---|---|
|ts| timestamp| 主时间戳列|
|i32| int32| 4字节整数，设备测量值 metric|

```bash
taos> select * from foo;
           ts            |      i32    |
========================================
 2020-01-01 00:00:12.681 |          13 |
 2020-01-01 00:00:13.727 |          14 |
 2020-01-01 00:00:14.378 |           8 |
 2020-01-01 00:00:15.774 |          10 |
 2020-01-01 00:00:16.170 |          16 |
 2020-01-01 00:00:17.558 |          26 |
 2020-01-01 00:00:18.938 |          32 |
 2020-01-01 00:00:19.308 |          27 |
```

### 语法
```SQL
FORECAST(column_expr, option_expr)

option_expr: {"
algo=expr1
[,wncheck=1|0]
[,conf=conf_val]
[,every=every_val]
[,rows=rows_val]
[,start=start_ts_val]
[,expr2]
"}

```
1. `column_expr`：预测的时序数据列，只支持数值类型列输入。
2. `options`：预测函数的参数。字符串类型，其中使用 K=V 方式调用算法及相关参数。采用逗号分隔的 K=V 字符串表示，其中的字符串不需要使用单引号、双引号、或转义号等符号，不能使用中文及其他宽字符。预测支持 `conf`, `every`, `rows`, `start`, `rows` 几个控制参数，其含义如下：

### 参数说明

|参数|含义|默认值|
|---|---|---|
|algo|预测分析使用的算法|holtwinters|
|wncheck|白噪声（white noise data）检查|默认值为 1，0 表示不进行检查|
|conf|预测数据的置信区间范围 ，取值范围 [0, 100]|95|
|every|预测数据的采样间隔|输入数据的采样间隔|
|start|预测结果的开始时间戳|输入数据最后一个时间戳加上一个采样间隔时间区间|
|rows|预测结果的记录数|10|

1. 预测查询结果新增三个伪列，具体如下：`_FROWTS`：预测结果的时间戳、`_FLOW`：置信区间下界、`_FHIGH`：置信区间上界, 对于没有置信区间的预测算法，其置信区间同预测结果
2. 更改参数 `START`：返回预测结果的起始时间，改变起始时间不会影响返回的预测数值，只影响起始时间。
3. `EVERY`：可以与输入数据的采样频率不同。采样频率只能低于或等于输入数据采样频率，不能**高于**输入数据的采样频率。
4. 对于某些不需要计算置信区间的算法，即使指定了置信区间，返回的结果中其上下界退化成为一个点。

### 示例

```SQL
--- 使用 arima 算法进行预测，预测结果是 10 条记录（默认值），数据进行白噪声检查，默认置信区间 95%. 
SELECT  _flow, _fhigh, _frowts, FORECAST(i32, "algo=arima")
FROM foo;

--- 使用 arima 算法进行预测，输入数据的是周期数据，每 10 个采样点是一个周期，返回置信区间是95%的上下边界，同时忽略白噪声检查
SELECT  _flow, _fhigh, _frowts, FORECAST(i32, "algo=arima,alpha=95,period=10,wncheck=0")
FROM foo;
```
```
taos> select _flow, _fhigh, _frowts, forecast(i32) from foo;
        _flow         |        _fhigh        |       _frowts           | forecast(i32) |
========================================================================================
           10.5286684 |           41.8038254 | 2020-01-01 00:01:35.000 |            26 |
          -21.9861946 |           83.3938904 | 2020-01-01 00:01:36.000 |            30 |
          -78.5686035 |          144.6729126 | 2020-01-01 00:01:37.000 |            33 |
         -154.9797363 |          230.3057709 | 2020-01-01 00:01:38.000 |            37 |
         -253.9852905 |          337.6083984 | 2020-01-01 00:01:39.000 |            41 |
         -375.7857971 |          466.4594727 | 2020-01-01 00:01:40.000 |            45 |
         -514.8043823 |          622.4426270 | 2020-01-01 00:01:41.000 |            53 |
         -680.6343994 |          796.2861328 | 2020-01-01 00:01:42.000 |            57 |
         -868.4956665 |          992.8603516 | 2020-01-01 00:01:43.000 |            62 |
        -1076.1566162 |         1214.4498291 | 2020-01-01 00:01:44.000 |            69 |
```


## 内置预测算法
- [arima](./02-arima.md)
- [holtwinters](./03-holtwinters.md)
- CES (Complex Exponential Smoothing) 
- Theta
- Prophet
- XGBoost
- LightGBM
- Multiple Seasonal-Trend decomposition using LOESS (MSTL)
- ETS (Error, Trend, Seasonal)
- Long Short-Term Memory (LSTM)
- Multilayer Perceptron (MLP)
- DeepAR
- N-BEATS
- N-HiTS
- PatchTST (Patch Time Series Transformer)
- Temporal Fusion Transformer
- TimesNet

## 算法有效性评估工具
TDgpt 提供预测分析算法有效性评估工具 `analytics_compare`，调用该工具并设置合适的参数，能够使用 TDengine 中的数据作为回测依据，评估不同预测算法或相同的预测算法在不同的参数或训练模型的下的预测有效性。预测有效性的评估使用 `MSE` 和 `MAE` 指标作为依据，后续还将增加 `MAPE`指标。

```ini
[forecast]
# 训练数据的周期，每个周期包含多少个输入点
period = 10

# 使用范围内最后 10 条记录作为预测结果
rows = 10

# 训练数据开始时间
start_time = 1949-01-01T00:00:00

# 训练数据结束时间
end_time = 1960-12-01T00:00:00

# 输出结果的起始时间
res_start_time = 1730000000000

# 是否绘制预测结果图
gen_figure = true
```

算法对比分析运行完成以后，生成 fc-results.xlsx 文件，其中包含了调用算法的预测分析误差、执行时间、调用参数等信息。如下图所示：

<img src={fc_result} width="760" alt="预测对比结果" />


如果设置了 `gen_figure` 为 true，分析结果中还会有绘制的分析预测结果图（如下图所示）。

<img src={fc_result_figure} width="760" alt="预测对比结果" />

