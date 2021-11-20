﻿/********************************************************************
	created:	2011/07/10
	created:	10:7:2011   23:34
	filename: 	d:\NewTable__0709\NewTable\Common\DataBase\FieldTypeDefine.h
	file path:	d:\NewTable__0709\NewTable\Common\DataBase
	file base:	FieldTypeDefine
	file ext:	h
	author:		Yang Wenge
	
	purpose:	
*********************************************************************/

#ifndef _INCLUDE_FIELDTYPEDEFINE_H_
#define _INCLUDE_FIELDTYPEDEFINE_H_

enum FIELD_TYPE
{
	FIELD_NULL = 0,
	FIELD_INT,
	FIELD_FLOAT,
	FIELD_DOUBLE,
	FIELD_STRING,
	FIELD_BOOL,
	FIELD_BYTE,
	FIELD_SHORT,
	FIELD_INT64,
	FIELD_INT_RELATION,
	FIELD_STRING_RELATION,
	FIELD_DATA,
	FIELD_TABLE,
	FIELD_RECORD,
	FIELD_NICEDATA,
	FIELD_EVENT,
	FIELD_SKIP_1,
	FIELD_SKIP_2,
	FIELD_UINT64,
	FIELD_CHAR_STRING,
	FIELD_NET_CONNECT,
//	FIELD_BINARY,
	FIELD_DB_TABLE,
    FIELD_ARRAYDATA,
    FIELD_DBNICE,					// DB或表格中使用的具有共用名称索引的数据, 特点是只保存名称索引ID与数据
	FIELD_CLOUD_NICE,			// 保存NiceData > UInt64 key 索引
	FIELD_SQLDATA,					// 即时保存或读取SQL数据, 只用于DB记录字段
	FIELD_TYPE_MAX
};

#define TYPE_INT_NAME				"INT"
#define TYPE_FLOAT_NAME				"FLOAT"
#define TYPE_STRING_NAME			"STRING"
#define TYPE_BOOL_NAME				"BOOL"
#define TYPE_BYTE_NAME				"BYTE"
#define TYPE_SHORT_NAME				"SHORT"
#define TYPE_INT64_NAME					"INT64"
#define TYPE_INT_RELATION_NAME		"INT_R"
#define TYPE_STRING_RELATION_NAME	"STRING_R"
#define TYPE_DATA_NAME				"DATA"
#define TYPE_TABLE_NAME				"TABLE"
#define TYPE_DB_TABLE_NAME				"DB_TABLE"
#define TYPE_RECORD_NAME			"RECORD"
#define TYPE_NICEDATA_NAME			"NICEDATA"
#define TYPE_ARRAYDATA_NAME			"ARRAYDATA"
#define TYPE_DBNICE_NAME			"DB_NICE"
#define TYPE_EVENT_NAME				"EVENT"
#define TYPE_UINT64_NAME			"UINT64"
#define TYPE_CHARSTRING_NAME		"CHARSTRING"
#define FIELD_NET_CONNECT			"NET_CONNECT"
//#define FIELD_BINARY_NAME			"BINARY"
#define TYPE_SQL_DATA					"SQL_DATA"

#define TYPE_DOUBLE_NAME			"DOUBLE"
#define TYPE_DBNICEKEY_NAME			"DB_NICE_KEY"

#endif //_INCLUDE_FIELDTYPEDEFINE_H_