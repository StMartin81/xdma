#ifndef __XDMA_SYSCTL_H__
#define __XDMA_SYSCTL_H__

#include "libxdma.h"
#include <linux/sysctl.h>

#define CTL_PATH_SIZE 3
#define CTL_TABLE_SIZE 2

struct sysctl_data
{
  struct ctl_path ctl_path[CTL_PATH_SIZE];
  struct ctl_table ctl_table[CTL_TABLE_SIZE];
  struct ctl_table_header* ctl_table_header;
  struct xdma_dev* xdev;
  unsigned int user_irq;
};

int
xdma_sysctl_init(struct sysctl_data* sysctl_data, struct xdma_dev* xdev);

void
xdma_sysctl_free(struct sysctl_data* sysctl_data);

#endif /* __XDMA_SYSCTL_H__ */
