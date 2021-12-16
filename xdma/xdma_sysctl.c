#include "xdma_sysctl.h"
#include "libxdma_api.h"
#include "xdma_cdev.h"

static int
enable_user_irq(struct ctl_table* ctl,
                int write,
                void __user* buffer,
                size_t* lenp,
                loff_t* ppos)
{
  int ret = 0;
  void* config_base_address;
  struct interrupt_regs* irq_regs;
  struct sysctl_data* data;
  struct xdma_dev* xdev;

  BUG_ON(!ctl);

  if (write) {
    data = (struct sysctl_data*)ctl->extra1;
    xdev = data->xdev;

    ret = proc_douintvec(ctl, write, buffer, lenp, ppos);
    if (ret)
      return ret;

    config_base_address = xdev->bar[xdev->config_bar_idx];
    irq_regs =
      (struct interrupt_regs*)(config_base_address + XDMA_OFS_INT_CTRL);

    /* Enable user interrupt */
    dbg_io("Enabling user IRQ %d\n", data->user_irq);
    write_register(0x1u << data->user_irq,
                   config_base_address,
                   (size_t)&irq_regs->user_int_enable_w1s -
                     (size_t)config_base_address);
  }

  return 0;
}

static const char*
set_device_name(const char* fmt, ...)
{
  const char* device_name;
  va_list vargs;

  va_start(vargs, fmt);
  device_name = kvasprintf_const(GFP_KERNEL, fmt, vargs);
  va_end(vargs);

  return device_name;
}

void
xdma_sysctl_free(struct sysctl_data* sysctl_data)
{
  if (sysctl_data->ctl_table_header)
    unregister_sysctl_table(sysctl_data->ctl_table_header);
  kfree_const(sysctl_data->ctl_path[1].procname);
  sysctl_data->ctl_path[1].procname = NULL;
}

int
xdma_sysctl_init(struct sysctl_data* sysctl_data, struct xdma_dev* xdev)
{
  int ret = 0;
  size_t ctl_path_size = sizeof(sysctl_data->ctl_path);
  size_t ctl_table_size = sizeof(sysctl_data->ctl_table);

  memset(sysctl_data->ctl_path, 0, ctl_path_size);
  memset(sysctl_data->ctl_table, 0, ctl_table_size);

  sysctl_data->ctl_path[0].procname = "dev";
  sysctl_data->ctl_path[1].procname =
    set_device_name(XDMA_NODE_NAME "%d", xdev->idx);
  if (!sysctl_data->ctl_path[1].procname) {
    dbg_init("Failed to initialized sysctl device name.\n");
    ret = -ENOMEM;
    goto device_name;
  }

  sysctl_data->ctl_table[0].procname = "enable_user_irq";
  sysctl_data->ctl_table[0].mode = 0222;
  sysctl_data->ctl_table[0].data = &sysctl_data->user_irq;
  sysctl_data->ctl_table[0].maxlen = sizeof(sysctl_data->user_irq);
  sysctl_data->ctl_table[0].extra1 = sysctl_data;
  sysctl_data->ctl_table[0].proc_handler = enable_user_irq;

  sysctl_data->xdev = xdev;

  sysctl_data->ctl_table_header =
    register_sysctl_paths(sysctl_data->ctl_path, sysctl_data->ctl_table);
  if (!sysctl_data->ctl_table_header) {
    dbg_init("Failed to initialized sysctl paths.\n");
    ret = -ENOMEM;
    goto register_sysctl_paths;
  }
  return ret;

register_sysctl_paths:
device_name:
  xdma_sysctl_free(sysctl_data);
  return ret;
}
