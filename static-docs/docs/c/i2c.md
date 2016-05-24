# I2C
---
## Functions
---
### libsoc_i2c_init

```c
i2c* libsoc_i2c_init (uint8_t i2c_bus, uint8_t i2c_address)
```

- *uint8_t* **i2c_bus**

	the linux enumerated bus number

- *uint8_t* **i2c_address**

	the peripherals address on the i2c bus

Initialises a new i2c instance at the specificed address. Returns a
malloced i2c struct which must be freed with
[libsoc_i2c_free](#libsoc_i2c_free) when no longer needed.

Returns `NULL` on failure.

---

### libsoc_i2c_free

```c
int libsoc_i2c_free (i2c * i2c)
```

- *i2c\** **i2c**

	previously initialised i2c struct

Free the memory associated with a previously initialised i2c struct and release
the hold on the i2c address.

Returns `EXIT_SUCCESS` or `EXIT_FAILURE`

---

### libsoc_i2c_write

```c
int libsoc_i2c_write (i2c * i2c, uint8_t * buffer, uint16_t len)
```

- *i2c\** **i2c**

	previously initialised i2c struct

- *uint8_t\** **buffer**

	pointer to output data buffer

- *uint16_t* **len**

	length of data to write in bytes

Write `len` bytes starting from the `buffer` data pointer to the specified
i2c device.


Returns `EXIT_SUCCESS` or `EXIT_FAILURE`

---

### libsoc_i2c_read

```c
int libsoc_i2c_read (i2c * i2c, uint8_t * buffer, uint16_t len)
```

- *i2c\** **i2c**

	previously initialised i2c struct

- *uint8_t\** **buffer**

	pointer to input data buffer

- *uint16_t* **len**

	length of data to read in bytes

Read `len` bytes into the `buffer` data pointer from the specified
i2c device.

Returns `EXIT_SUCCESS` or `EXIT_FAILURE`

---

### libsoc_i2c_set_timeout

```c
int libsoc_i2c_set_timeout(i2c * i2c, int timeout)
```

- *i2c\** **i2c**

	previously initialised i2c struct

- *int* **timeout**

	i2c timeout in milliseconds/10

Set the timeout of a i2c device in milliseconds/10. For example if a timeout of
20ms is requred, `timeout` would be set to 2.

```c
	// 20ms timeout, 2*10ms
	libsoc_i2c_set_timeout(device, 2);
```

Returns `EXIT_SUCCESS` or `EXIT_FAILURE`

