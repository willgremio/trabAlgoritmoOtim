#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define PF(str, ...) printf(str, ##__VA_ARGS__)

// error messages level, always enabled
#define PF_ERR(str, ...) \
	PF("[ERROR] func: %s, line: %u, msg: "str"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

// debug messages level
#define TRACE_DBG_GLOBAL
#if defined(TRACE_DBG_LOCAL) || defined(TRACE_DBG_GLOBAL)
#define PF_DBG(str, ...) \
	PF("[DEBUG] func: %s, msg: "str"\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define PF_DBG(str, ...)
#endif

/* information messages level */
#define TRACE_INF_GLOBAL
#ifdef TRACE_INF_GLOBAL
#define PF_INF(str, ...) PF(str"\n", ##__VA_ARGS__)
#else
#define PF_INF(str, ...)
#endif

#endif // _DEBUG_H_

