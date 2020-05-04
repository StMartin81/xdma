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
