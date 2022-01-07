/**
 * @file cpputil.hh
 * A file taken from apsiproxy which has useful preprocessing tools.
 * 
 */

#ifndef CPPUTIL_H
#define CPPUTIL_H

/* Generally useful C preprocessor routines */
#define PP_STR(X) PP_STR2(X)
#define PP_STR2(X) #X
#define PP_CAT(X, ...) PP_CAT2(X, __VA_ARGS__)
#define PP_CAT2(X, ...) X ## __VA_ARGS__

#endif