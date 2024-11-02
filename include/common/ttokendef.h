/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TD_COMMON_TOKEN_H_
#define _TD_COMMON_TOKEN_H_

#define TK_OR                   1
#define TK_AND                  2
#define TK_UNION                3
#define TK_ALL                  4
#define TK_MINUS                5
#define TK_EXCEPT               6
#define TK_INTERSECT            7
#define TK_NK_BITAND            8
#define TK_NK_BITOR             9
#define TK_NK_LSHIFT            10
#define TK_NK_RSHIFT            11
#define TK_NK_PLUS              12
#define TK_NK_MINUS             13
#define TK_NK_STAR              14
#define TK_NK_SLASH             15
#define TK_NK_REM               16
#define TK_NK_CONCAT            17
#define TK_CREATE               18
#define TK_ACCOUNT              19
#define TK_NK_ID                20
#define TK_PASS                 21
#define TK_NK_STRING            22
#define TK_ALTER                23
#define TK_PPS                  24
#define TK_TSERIES              25
#define TK_STORAGE              26
#define TK_STREAMS              27
#define TK_QTIME                28
#define TK_DBS                  29
#define TK_USERS                30
#define TK_CONNS                31
#define TK_STATE                32
#define TK_NK_COMMA             33
#define TK_HOST                 34
#define TK_IS_IMPORT            35
#define TK_NK_INTEGER           36
#define TK_CREATEDB             37
#define TK_USER                 38
#define TK_ENABLE               39
#define TK_SYSINFO              40
#define TK_ADD                  41
#define TK_DROP                 42
#define TK_GRANT                43
#define TK_ON                   44
#define TK_TO                   45
#define TK_REVOKE               46
#define TK_FROM                 47
#define TK_SUBSCRIBE            48
#define TK_READ                 49
#define TK_WRITE                50
#define TK_NK_DOT               51
#define TK_WITH                 52
#define TK_ENCRYPT_KEY          53
#define TK_ANODE                54
#define TK_UPDATE               55
#define TK_ANODES               56
#define TK_DNODE                57
#define TK_PORT                 58
#define TK_DNODES               59
#define TK_RESTORE              60
#define TK_NK_IPTOKEN           61
#define TK_FORCE                62
#define TK_UNSAFE               63
#define TK_CLUSTER              64
#define TK_LOCAL                65
#define TK_QNODE                66
#define TK_BNODE                67
#define TK_SNODE                68
#define TK_MNODE                69
#define TK_VNODE                70
#define TK_DATABASE             71
#define TK_USE                  72
#define TK_FLUSH                73
#define TK_TRIM                 74
#define TK_S3MIGRATE            75
#define TK_COMPACT              76
#define TK_IF                   77
#define TK_NOT                  78
#define TK_EXISTS               79
#define TK_BUFFER               80
#define TK_CACHEMODEL           81
#define TK_CACHESIZE            82
#define TK_COMP                 83
#define TK_DURATION             84
#define TK_NK_VARIABLE          85
#define TK_MAXROWS              86
#define TK_MINROWS              87
#define TK_KEEP                 88
#define TK_PAGES                89
#define TK_PAGESIZE             90
#define TK_TSDB_PAGESIZE        91
#define TK_PRECISION            92
#define TK_REPLICA              93
#define TK_VGROUPS              94
#define TK_SINGLE_STABLE        95
#define TK_RETENTIONS           96
#define TK_SCHEMALESS           97
#define TK_WAL_LEVEL            98
#define TK_WAL_FSYNC_PERIOD     99
#define TK_WAL_RETENTION_PERIOD 100
#define TK_WAL_RETENTION_SIZE   101
#define TK_WAL_ROLL_PERIOD      102
#define TK_WAL_SEGMENT_SIZE     103
#define TK_STT_TRIGGER          104
#define TK_TABLE_PREFIX         105
#define TK_TABLE_SUFFIX         106
#define TK_S3_CHUNKPAGES        107
#define TK_S3_KEEPLOCAL         108
#define TK_S3_COMPACT           109
#define TK_KEEP_TIME_OFFSET     110
#define TK_ENCRYPT_ALGORITHM    111
#define TK_NK_COLON             112
#define TK_BWLIMIT              113
#define TK_START                114
#define TK_TIMESTAMP            115
#define TK_END                  116
#define TK_TABLE                117
#define TK_NK_LP                118
#define TK_NK_RP                119
#define TK_USING                120
#define TK_FILE                 121
#define TK_STABLE               122
#define TK_COLUMN               123
#define TK_MODIFY               124
#define TK_RENAME               125
#define TK_TAG                  126
#define TK_SET                  127
#define TK_NK_EQ                128
#define TK_TAGS                 129
#define TK_BOOL                 130
#define TK_TINYINT              131
#define TK_SMALLINT             132
#define TK_INT                  133
#define TK_INTEGER              134
#define TK_BIGINT               135
#define TK_FLOAT                136
#define TK_DOUBLE               137
#define TK_BINARY               138
#define TK_NCHAR                139
#define TK_UNSIGNED             140
#define TK_JSON                 141
#define TK_VARCHAR              142
#define TK_MEDIUMBLOB           143
#define TK_BLOB                 144
#define TK_VARBINARY            145
#define TK_GEOMETRY             146
#define TK_DECIMAL              147
#define TK_COMMENT              148
#define TK_MAX_DELAY            149
#define TK_WATERMARK            150
#define TK_ROLLUP               151
#define TK_TTL                  152
#define TK_SMA                  153
#define TK_DELETE_MARK          154
#define TK_FIRST                155
#define TK_LAST                 156
#define TK_SHOW                 157
#define TK_FULL                 158
#define TK_PRIVILEGES           159
#define TK_DATABASES            160
#define TK_TABLES               161
#define TK_STABLES              162
#define TK_MNODES               163
#define TK_QNODES               164
#define TK_ARBGROUPS            165
#define TK_FUNCTIONS            166
#define TK_INDEXES              167
#define TK_ACCOUNTS             168
#define TK_APPS                 169
#define TK_CONNECTIONS          170
#define TK_LICENCES             171
#define TK_GRANTS               172
#define TK_LOGS                 173
#define TK_MACHINES             174
#define TK_ENCRYPTIONS          175
#define TK_QUERIES              176
#define TK_SCORES               177
#define TK_TOPICS               178
#define TK_VARIABLES            179
#define TK_BNODES               180
#define TK_SNODES               181
#define TK_TRANSACTIONS         182
#define TK_DISTRIBUTED          183
#define TK_CONSUMERS            184
#define TK_SUBSCRIPTIONS        185
#define TK_VNODES               186
#define TK_ALIVE                187
#define TK_VIEWS                188
#define TK_VIEW                 189
#define TK_COMPACTS             190
#define TK_DISK_INFO            191
#define TK_NORMAL               192
#define TK_CHILD                193
#define TK_LIKE                 194
#define TK_TBNAME               195
#define TK_QTAGS                196
#define TK_AS                   197
#define TK_SYSTEM               198
#define TK_TSMA                 199
#define TK_INTERVAL             200
#define TK_RECURSIVE            201
#define TK_TSMAS                202
#define TK_FUNCTION             203
#define TK_INDEX                204
#define TK_COUNT                205
#define TK_LAST_ROW             206
#define TK_META                 207
#define TK_ONLY                 208
#define TK_TOPIC                209
#define TK_CONSUMER             210
#define TK_GROUP                211
#define TK_DESC                 212
#define TK_DESCRIBE             213
#define TK_RESET                214
#define TK_QUERY                215
#define TK_CACHE                216
#define TK_EXPLAIN              217
#define TK_ANALYZE              218
#define TK_VERBOSE              219
#define TK_NK_BOOL              220
#define TK_RATIO                221
#define TK_NK_FLOAT             222
#define TK_OUTPUTTYPE           223
#define TK_AGGREGATE            224
#define TK_BUFSIZE              225
#define TK_LANGUAGE             226
#define TK_REPLACE              227
#define TK_STREAM               228
#define TK_INTO                 229
#define TK_PAUSE                230
#define TK_RESUME               231
#define TK_PRIMARY              232
#define TK_KEY                  233
#define TK_TRIGGER              234
#define TK_AT_ONCE              235
#define TK_WINDOW_CLOSE         236
#define TK_IGNORE               237
#define TK_EXPIRED              238
#define TK_FILL_HISTORY         239
#define TK_SUBTABLE             240
#define TK_UNTREATED            241
#define TK_KILL                 242
#define TK_CONNECTION           243
#define TK_TRANSACTION          244
#define TK_BALANCE              245
#define TK_VGROUP               246
#define TK_LEADER               247
#define TK_MERGE                248
#define TK_REDISTRIBUTE         249
#define TK_SPLIT                250
#define TK_DELETE               251
#define TK_INSERT               252
#define TK_NK_BIN               253
#define TK_NK_HEX               254
#define TK_NULL                 255
#define TK_NK_QUESTION          256
#define TK_NK_ALIAS             257
#define TK_NK_ARROW             258
#define TK_ROWTS                259
#define TK_QSTART               260
#define TK_QEND                 261
#define TK_QDURATION            262
#define TK_WSTART               263
#define TK_WEND                 264
#define TK_WDURATION            265
#define TK_IROWTS               266
#define TK_ISFILLED             267
#define TK_FLOW                 268
#define TK_FHIGH                269
#define TK_FROWTS               270
#define TK_CAST                 271
#define TK_POSITION             272
#define TK_IN                   273
#define TK_FOR                  274
#define TK_NOW                  275
#define TK_TODAY                276
#define TK_RAND                 277
#define TK_SUBSTR               278
#define TK_SUBSTRING            279
#define TK_BOTH                 280
#define TK_TRAILING             281
#define TK_LEADING              282
#define TK_TIMEZONE             283
#define TK_CLIENT_VERSION       284
#define TK_SERVER_VERSION       285
#define TK_SERVER_STATUS        286
#define TK_CURRENT_USER         287
#define TK_PI                   288
#define TK_CASE                 289
#define TK_WHEN                 290
#define TK_THEN                 291
#define TK_ELSE                 292
#define TK_BETWEEN              293
#define TK_IS                   294
#define TK_NK_LT                295
#define TK_NK_GT                296
#define TK_NK_LE                297
#define TK_NK_GE                298
#define TK_NK_NE                299
#define TK_MATCH                300
#define TK_NMATCH               301
#define TK_CONTAINS             302
#define TK_JOIN                 303
#define TK_INNER                304
#define TK_LEFT                 305
#define TK_RIGHT                306
#define TK_OUTER                307
#define TK_SEMI                 308
#define TK_ANTI                 309
#define TK_ASOF                 310
#define TK_WINDOW               311
#define TK_WINDOW_OFFSET        312
#define TK_JLIMIT               313
#define TK_SELECT               314
#define TK_NK_HINT              315
#define TK_DISTINCT             316
#define TK_WHERE                317
#define TK_PARTITION            318
#define TK_BY                   319
#define TK_SESSION              320
#define TK_STATE_WINDOW         321
#define TK_EVENT_WINDOW         322
#define TK_COUNT_WINDOW         323
#define TK_ANOMALY_WINDOW       324
#define TK_SLIDING              325
#define TK_FILL                 326
#define TK_VALUE                327
#define TK_VALUE_F              328
#define TK_NONE                 329
#define TK_PREV                 330
#define TK_NULL_F               331
#define TK_LINEAR               332
#define TK_NEXT                 333
#define TK_HAVING               334
#define TK_RANGE                335
#define TK_EVERY                336
#define TK_ORDER                337
#define TK_SLIMIT               338
#define TK_SOFFSET              339
#define TK_LIMIT                340
#define TK_OFFSET               341
#define TK_ASC                  342
#define TK_NULLS                343
#define TK_ABORT                344
#define TK_AFTER                345
#define TK_ATTACH               346
#define TK_BEFORE               347
#define TK_BEGIN                348
#define TK_BITAND               349
#define TK_BITNOT               350
#define TK_BITOR                351
#define TK_BLOCKS               352
#define TK_CHANGE               353
#define TK_COMMA                354
#define TK_CONCAT               355
#define TK_CONFLICT             356
#define TK_COPY                 357
#define TK_DEFERRED             358
#define TK_DELIMITERS           359
#define TK_DETACH               360
#define TK_DIVIDE               361
#define TK_DOT                  362
#define TK_EACH                 363
#define TK_FAIL                 364
#define TK_GLOB                 365
#define TK_ID                   366
#define TK_IMMEDIATE            367
#define TK_IMPORT               368
#define TK_INITIALLY            369
#define TK_INSTEAD              370
#define TK_ISNULL               371
#define TK_MODULES              372
#define TK_NK_BITNOT            373
#define TK_NK_SEMI              374
#define TK_NOTNULL              375
#define TK_OF                   376
#define TK_PLUS                 377
#define TK_PRIVILEGE            378
#define TK_RAISE                379
#define TK_RESTRICT             380
#define TK_ROW                  381
#define TK_STAR                 382
#define TK_STATEMENT            383
#define TK_STRICT               384
#define TK_STRING               385
#define TK_TIMES                386
#define TK_VALUES               387
#define TK_VARIABLE             388
#define TK_WAL                  389

#define TK_NK_SPACE   600
#define TK_NK_COMMENT 601
#define TK_NK_ILLEGAL 602
// #define TK_NK_HEX           603  // hex number  0x123
#define TK_NK_OCT 604  // oct number
// #define TK_NK_BIN           605  // bin format data 0b111
#define TK_BATCH_SCAN        606
#define TK_NO_BATCH_SCAN     607
#define TK_SORT_FOR_GROUP    608
#define TK_PARTITION_FIRST   609
#define TK_PARA_TABLES_SORT  610
#define TK_SMALLDATA_TS_SORT 611
#define TK_HASH_JOIN         612
#define TK_SKIP_TSMA         613

#define TK_NK_NIL 65535

#endif /*_TD_COMMON_TOKEN_H_*/
