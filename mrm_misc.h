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
#define MRM_CAFE_BABE 0xCD // base 10 -> 205

/*
 * implementation of boolean
 *
 * false = 0
 * true = 1
 */
#define bool MRM_Bool
#define TRUE MRM_BOOL_TRUE
#define FALSE MRM_BOOL_FALSE
typedef enum MRM_Bool {
	MRM_BOOL_FALSE = 0,
	MRM_BOOL_TRUE,
} MRM_Bool;

#endif // !MRM_MISC_H
