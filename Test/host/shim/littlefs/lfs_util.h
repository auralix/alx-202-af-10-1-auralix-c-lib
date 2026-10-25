/**
  ******************************************************************************
  * @file		lfs_util.h
  * @brief		Auralix C Library - Configuration Matrix Fake - littlefs
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the one thing of littlefs's utilities the alx code relies on -
  * that a build defining LFS_CONFIG gets that header in their place, which is
  * how the library's alxLfsConfig.h is compiled (Test/config/matrix.toml
  * defines LFS_CONFIG=alxLfsConfig.h, as a product with littlefs does).
  ******************************************************************************
  **/

#ifndef ALX_SHIM_LITTLEFS_LFS_UTIL_H
#define ALX_SHIM_LITTLEFS_LFS_UTIL_H

#ifdef LFS_CONFIG
#define LFS_STRINGIZE(x) LFS_STRINGIZE2(x)
#define LFS_STRINGIZE2(x) #x
#include LFS_STRINGIZE(LFS_CONFIG)
#endif

#endif	// ALX_SHIM_LITTLEFS_LFS_UTIL_H
