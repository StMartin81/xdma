# Create PBuilder Environment
Use a PBuilder environment to prevent that the source directory gets cluttered with generated files
```
$ pbuilder-dist bionic create
```

# Create Debian Package
The following commands will create the Debian package:
```
$ dpkg-buildpackage -S -us -uc
$ pbuilder-dist bionic build ../xdma_2018.3.50.dsc
```

# Install Driver
If you used the PBuilder environment the driver can be installed with the following command:
```
# dpkg -i ~/pbuilder/bionic_result/xdma-dkms_2018.3.50_all.deb
```

# Install Driver Manually
If for whatever reasons you want to install the driver manually you have to copy the sources first to `/usr/src/xdma-2018.3.50/` then you can add the driver with the following commands:
```
dkms add -m xdma -v 2018.3.50
dkms build -m xdma -v 2018.3.50
dkms install -m xdma -v 2018.3.50
```

The driver can then be removed with the following commands:
```
dkms remove -m xdma/2018.3.50 --all
```

# Using The Sysctl Interface To Wait For User Interrupts
Enable user IRQ 0:
```
const char user_irq_sysctl_path[] = "/proc/sys/dev/xdma0/enable_user_irq/0";
int fpga_fd = open(user_irq_sysctl_path, O_WRONLY | O_SYNC);
write(fpga_fd, (uint32_t)0, 4);
close(fpga_fd);
```

After the user IRQ 0 was enabled it is possible to wait for an IRQ event:
```
const char user_event_path[] = "/dev/xdma/card0/events0";
struct pollfd poll_fd;
uint32_t data; /* pseudo data for read */
ssize_t bytes_read;
int const timeout_ms = 500;

if (0 != access(user_event_path, F_OK)) {
  /* return error */
}
fpga_fd = open(user_event_path, O_RDONLY);


poll_fd = { .fd = fpga_fd, .events = POLLIN };
if (!(poll(&poll_fd, 1, timeout_ms) > 0)) {
  /* return error */
}

bytes_read = read(fpga_fd, &data, sizeof(data));
if (bytes_read != sizeof(data)) {
  /* return error */
}
```

When a user interrupt is enabled and an interrupt was received the function `user_irq_service` in `libxdma.x` will set a flag and it will call `wake_up_interruptible`. When trying to read from the event device the function `char_events_read` in `cdev_events.c` will be called which waits for the event (`wait_event_interruptible`). The `poll` command has an option to set an timeout to return if no value is available after a certain time. This will happen if no user interrupt was received during the timeout.
