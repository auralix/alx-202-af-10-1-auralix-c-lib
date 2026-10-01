/**
  ******************************************************************************
  * @file		lfs.h
  * @brief		Auralix C Library - Configuration Matrix Fake - littlefs
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the littlefs API as far as the alx code uses it, written from
  * that use, so the configuration matrix compiles every alx line under ALX_LFS
  * (Test/config/matrix.toml). Nothing here is the vendor's and nothing here
  * runs; the real one is the product's.
  *
  * The types are littlefs's own widths, so the alx code's conversions and
  * format strings are judged against them: sizes, offsets and blocks are
  * 32-bit unsigned, the signed results of read, write, seek, tell and size
  * 32-bit signed.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_LITTLEFS_LFS_H
#define ALX_SHIM_LITTLEFS_LFS_H

#include <stdint.h>
#include <stdbool.h>
#include "lfs_util.h"


//******************************************************************************
// Types
//******************************************************************************
typedef uint32_t lfs_size_t;
typedef uint32_t lfs_off_t;
typedef int32_t lfs_ssize_t;
typedef int32_t lfs_soff_t;
typedef uint32_t lfs_block_t;

#define LFS_NAME_MAX 255

enum lfs_error
{
	LFS_ERR_OK = 0,
	LFS_ERR_IO = -5,
	LFS_ERR_CORRUPT = -84,
	LFS_ERR_NOENT = -2,
	LFS_ERR_EXIST = -17,
	LFS_ERR_NOTDIR = -20,
	LFS_ERR_ISDIR = -21,
	LFS_ERR_NOTEMPTY = -39,
	LFS_ERR_BADF = -9,
	LFS_ERR_FBIG = -27,
	LFS_ERR_INVAL = -22,
	LFS_ERR_NOSPC = -28,
	LFS_ERR_NOMEM = -12
};

enum lfs_type
{
	LFS_TYPE_REG = 0x001,
	LFS_TYPE_DIR = 0x002
};

enum lfs_open_flags
{
	LFS_O_RDONLY = 1,
	LFS_O_WRONLY = 2,
	LFS_O_RDWR = 3,
	LFS_O_CREAT = 0x0100,
	LFS_O_EXCL = 0x0200,
	LFS_O_TRUNC = 0x0400,
	LFS_O_APPEND = 0x0800
};

enum lfs_whence_flags
{
	LFS_SEEK_SET = 0,
	LFS_SEEK_CUR = 1,
	LFS_SEEK_END = 2
};

struct lfs_config
{
	void* context;
	int (*read)(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
	int (*prog)(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
	int (*erase)(const struct lfs_config* c, lfs_block_t block);
	int (*sync)(const struct lfs_config* c);
#ifdef LFS_THREADSAFE
	int (*lock)(const struct lfs_config* c);
	int (*unlock)(const struct lfs_config* c);
#endif
	lfs_size_t read_size;
	lfs_size_t prog_size;
	lfs_size_t block_size;
	lfs_size_t block_count;
	int32_t block_cycles;
	lfs_size_t cache_size;
	lfs_size_t lookahead_size;
	void* read_buffer;
	void* prog_buffer;
	void* lookahead_buffer;
	lfs_size_t name_max;
	lfs_size_t file_max;
	lfs_size_t attr_max;
};

struct lfs_info
{
	uint8_t type;
	lfs_size_t size;
	char name[LFS_NAME_MAX + 1];
};

typedef struct lfs { const struct lfs_config* cfg; uint32_t opaque[32]; } lfs_t;
typedef struct lfs_file { uint32_t opaque[32]; } lfs_file_t;
typedef struct lfs_dir { uint32_t opaque[16]; } lfs_dir_t;


//******************************************************************************
// API
//******************************************************************************
int lfs_format(lfs_t* lfs, const struct lfs_config* config);
int lfs_mount(lfs_t* lfs, const struct lfs_config* config);
int lfs_unmount(lfs_t* lfs);
int lfs_remove(lfs_t* lfs, const char* path);
int lfs_rename(lfs_t* lfs, const char* oldpath, const char* newpath);
int lfs_mkdir(lfs_t* lfs, const char* path);
int lfs_file_open(lfs_t* lfs, lfs_file_t* file, const char* path, int flags);
int lfs_file_close(lfs_t* lfs, lfs_file_t* file);
lfs_ssize_t lfs_file_read(lfs_t* lfs, lfs_file_t* file, void* buffer, lfs_size_t size);
lfs_ssize_t lfs_file_write(lfs_t* lfs, lfs_file_t* file, const void* buffer, lfs_size_t size);
int lfs_file_sync(lfs_t* lfs, lfs_file_t* file);
lfs_soff_t lfs_file_seek(lfs_t* lfs, lfs_file_t* file, lfs_soff_t off, int whence);
lfs_soff_t lfs_file_tell(lfs_t* lfs, lfs_file_t* file);
int lfs_file_rewind(lfs_t* lfs, lfs_file_t* file);
lfs_soff_t lfs_file_size(lfs_t* lfs, lfs_file_t* file);
int lfs_file_truncate(lfs_t* lfs, lfs_file_t* file, lfs_off_t size);
int lfs_dir_open(lfs_t* lfs, lfs_dir_t* dir, const char* path);
int lfs_dir_close(lfs_t* lfs, lfs_dir_t* dir);
int lfs_dir_read(lfs_t* lfs, lfs_dir_t* dir, struct lfs_info* info);

#endif	// ALX_SHIM_LITTLEFS_LFS_H
