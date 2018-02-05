## Introduction

---

### What is libsoc

libsoc is a C library to interface with common peripherals found in
System on Chips (SoC) through generic Linux Kernel interfaces.

It is aimed at new Linux users, and intends to be a stepping stone to
enable a user to get started quickly. It is optimised for reliability
rather than speed. While the library should be fast, no guarantees are
made on its determinism and it should not be used in time critical
routines.

### Why libsoc

libsoc was born due to the influx of new embedded Linux boards becoming
available cheap, and to hobbyists. There are currently numerous
libraries that do interfacing with common SoC peripherals, but they all
tend to be centred around a particular board or SoC. This leads to board
specific hacks and in some cases direct bypassing of the Linux Kernel.

libsoc aims to be generic and compatible with any SoC that has drivers
exposing the correct interfaces for the defined peripheral.

## Support

---

### SoC Support

If your SoC exposes its I/O subsystems using the generic Linux kernel
interfaces then yes, libsoc supports your SoC!

### Language Support

The library is written in C and has a native C API. A subset of the API
is also supported with Python bindings.

#### [C API](http://jackmitch.github.io/libsoc/c/gpio)

- Manual GPIO Manipulation through sysfs (Value, Edge, Direction, Exporting)
- Blocking GPIO Interrupts with timeout
- Non-blocking GPIO Interrupts with callback mechanism (pthread based)
- SPI transfers using spidev
- I2C transfers using ioctls
- PWM support through sysfs (Linux 3.12+)
- Automatic board probing via installed config files

#### [Python Bindings](http://jackmitch.github.io/libsoc/python)

- Manual GPIO Manipulation through sysfs (Value, Edge, Direction, Exporting)
- Blocking GPIO Interrupts with timeout
- Non-blocking GPIO Interrupts with callback mechanism
- I2C transfers

## Building

---

libsoc is built and installed using the autotools build system. This means it
follows the traditional linux methodology of autoreconf, configure, make.

First, check if your distro packages libsoc. If it does it is easiest to install
your distro's version of libsoc through its provided package manager. Depending
on your distro this may be `apt-get`, `dnf`, `yum`, `pacman`, etc. Consult your
distro documentation for more info.

### Manually Building

First, make sure you have the prerequisites installed. Under Debian Jessie this
means:

 * `autoconf`
 * `build-essential`
 * `libtool-bin`
 * `pkg-config`
 * `python-dev` *
 * `python3-dev` **

Under Fedora this would instead be:

 * `autoconf`
 * `automake`
 * `libtool`
 * `python2-devel` *
 * `python3-devel` **

\* Optional, only required if Python 2 bindings are desired.<br>
\*\* Optional, only required if Python 3 bindings are desired.

Then, clone libsoc from its git repository.

	git clone https://github.com/jackmitch/libsoc.git libsoc.git

Enter the libsoc.git directory

	cd libsoc.git

Run `autoreconf` to generate the libsoc configure scripts

	autoreconf -i

Configure the libsoc library with the required features

```text
./configure

	[--disable-debug]
	[--enable-python=<path|version>]
	[--enable-board=<board>]
	[--with-board-configs]
```

```text
--disable-debug

disables the debug code, turn off the debug to
get the fastest operation but at the cost of any
debug print outs. Omitting this flag will leave
debug enabled.
```

```text
--enable-python=<path|version>

enable Python language bindings to libsoc API.
Value can be empty for autodetect, the value 2
or 3 to search the PATH environment variable
for python2 or python3, or an absolute path to
a python binary.
```

```text
--enable-board=<board>

install a specific board config file to
$(sysdir)/libsoc.conf. This enables the use of
the board pin name lookup functions and
automatic board config probing. Supported boards
can be found under ./contrib/board_files. If
--with-board-configs is also used, a symlink
will be created instead of copying a single file.
```

```text
--with-board-configs

install all the contributed board configuration
files to $PREFIX/share/libsoc.
```

Compile the code using make

```text
make
```

Install the library using make

```text
make install
```

## Documentation

Written documentation can be found at
[http://jackmitch.github.io/libsoc](http://jackmitch.github.io/libsoc).

Test cases can be found in the 
[test/](https://github.com/jackmitch/libsoc/tree/master/test) directory
from the root of the project.

All functions and types are also documented directly in the code in the
application header files under
[lib/include](https://github.com/jackmitch/libsoc/tree/master/lib/include).

## Licencing

libsoc is licenced under the LGPLv2.1, please see the COPYING file for
further details.

## Contributing

libsoc is open-source software and as such you are welcome to browse the
code and either add features, or fix bugs. Please submit pull requests,
and bugs to [libsoc@github](http://www.github.com/jackmitch/libsoc).
If you have any comments or questions, I can be contacted by email at the
address [jack@embed.me.uk](mailto:jack@embed.me.uk).
