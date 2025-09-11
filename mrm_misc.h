/*
 * this file has useful defines and cafebabe!
 */

#ifndef MRM_MISC_H
#define MRM_MISC_H

/*
 * more descriptive use of the keyword `static`
 */
#define internal static
#define global_variable static
#define local_persist static

/*
 * good placeholder value to use for unintialized data
 * especially mallocs and such
 */
#define CAFE_BABE 0xCD // base 10 -> 205

/*
 * implementation of boolean
 *
 * false = 0
 *
 * true = 1
 */
typedef enum MRM_Bool {
	MRM_BOOL_FALSE = 0,
	MRM_BOOL_TRUE,
} MRM_Bool;

#define Bool MRM_Bool
#define TRUE MRM_BOOL_TRUE
#define FALSE MRM_BOOL_FALSE

/*
 * implementation of error codes
 *
 * NOT_FOUND = -1
 *
 * OK = 0
 *
 * ERR = 1
 */
typedef enum MRM_ErrorCode {
	MRM_ERROR_CODE_NOT_FOUND = -1,
	MRM_ERROR_CODE_OK = 0,
	MRM_ERROR_CODE_ERR = 1,
} MRM_ErrorCode;

#define Error MRM_ErrorCode
#define NOT_FOUND MRM_ERROR_CODE_NOT_FOUND
#define OK MRM_ERROR_CODE_OK
#define ERR MRM_ERROR_CODE_ERR

// used for ignoring unused functions due to defines
#define unused __attribute__((__unused__))

// used to temporarily shut up clang for unused variable
#define ignore __attribute__((__unused__))

#endif // !MRM_MISC_H
