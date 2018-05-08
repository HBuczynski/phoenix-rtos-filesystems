/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Jffs2 FileSystem - system specific information.
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "../os-phoenix.h"


#include "mtd.h"


int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
			     u_char *buf)
{

	*retlen = 0;

	if (!len)
		return 0;

	return 0;
}


int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	      const u_char *buf)
{
	if (!len)
		return 0;


	return 0;
}


int mtd_writev(struct mtd_info *mtd, const struct kvec *vecs,
	       unsigned long count, loff_t to, size_t *retlen)
{
	unsigned long i;
	size_t writelen = 0;
	int ret = 0;

	*retlen = 0;

	for (i = 0; i < count; i++) {
		ret = mtd_write(mtd, to, vecs[i].iov_len, &writelen, vecs[i].iov_base);
		*retlen = writelen;

		if (ret || writelen != vecs[i].iov_len)
			return ret;

		to += writelen;
	}
	return ret;
}


int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	return 0;
}


int mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops)
{
	return 0;
}


// not supported in nand
int mtd_point(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
			      void **virt, resource_size_t *phys)
{
	return -EOPNOTSUPP;
}


int mtd_unpoint(struct mtd_info *mtd, loff_t from, size_t len)
{
	return -EOPNOTSUPP;
}


int mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	return 0;
}


int mtd_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	return 0;
}


void *mtd_kmalloc_up_to(const struct mtd_info *mtd, size_t *size)
{
	void *ret;

	if (*size < mtd->writesize)
		*size = mtd->writesize;

	while (*size > mtd->writesize) {
		ret = malloc(*size);

		if (ret != NULL)
			return ret;

		*size >>= 1;
		*size += mtd->writesize - 1;
		*size &= mtd->writesize - 1;
	}

	return malloc((size_t)size);
}


int mtd_block_isbad(struct mtd_info *mtd, loff_t ofs)
{
	return 0;
}


struct dentry *mount_mtd(struct file_system_type *fs_type, int flags, 
		const char *dev_name, void *data,
		int (*fill_super)(struct super_block *, void *, int))
{
	struct mtd_info *mtd;
	flashdrv_dma_t *dma;

	flashdrv_init();

	dma = flashdrv_dmanew();

	if (dma == NULL)
		return NULL;

	mtd = malloc(sizeof(struct mtd_info));

	mtd->name = "micron";
	mtd->dma = dma;
	mtd->type = MTD_NANDFLASH;
	mtd->erasesize = 64 * 4096;
	mtd->writesize = 4096;
	mtd->flags = MTD_WRITEABLE;
	mtd->size = 64 * 4096 * 4096;
	mtd->oobsize = 224;
	mtd->oobavail = 16;

	struct super_block *sb = malloc(sizeof(struct super_block));
	sb->s_mtd = mtd;

	fill_super(sb, NULL, 0);
	return sb->s_root;
}


void kill_mtd_super(struct super_block *sb)
{
}