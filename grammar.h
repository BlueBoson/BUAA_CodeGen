#pragma once

enum class GrammarType {
	PROGRAM, // 1.程序
	CONST_DESCRIPT, // 2.常量说明
	VAR_DESCRIPT, // 3.变量说明
	RET_FUNC_DEF, // 4.有返回值函数定义
	VOID_FUNC_DEF, // 5.无返回值函数定义
	MAIN_FUNC, // 6.主函数
	CONST_DEF, // 7.常量定义
	VAR_DEF, // 8.变量定义
	DECLARE_HEAD, // 9.声明头部
	ARG_LIST, // 10.参数表
	COMP_STATE, // 11.复合语句
	INTEGER, // 12.整数
	UNSIGNED_INT, // 13.无符号整数
	STATE_SEQ, // 14.语句列
	STATEMENT, // 15.语句
	COND_STATE, // 16.条件语句
	LOOP_STATE, // 17.循环语句
	RET_FUNC_CALL, // 18.有返回值函数调用语句
	VOID_FUNC_CALL, // 19.无返回值函数调用语句
	ASSIGN_STATE, // 20.赋值语句
	READ_STATE, // 21.读语句
	WRITE_STATE, // 22.写语句
	RETURN_STATE, // 23.返回语句
	CONDITION, // 24.条件
	EXPRESSION, // 25.表达式
	STEP, // 26.步长
	VALUE_LIST, // 27.值参数表
	ITEM, // 28.项
	FACTOR, // 29.因子
	STRING, // 30.字符串
};