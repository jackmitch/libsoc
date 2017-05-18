# GPIO

---

## Data Types
---
### gpio_mode

Determines the way in which libsoc handles exporting and unexporting
GPIOs in the Linux subsystem.

* **LS_GPIO_SHARED**

	if the gpio is already exported then it will not unexport
	the GPIO on free. If it is not exported, then it will
	unexport on free.

* **LS_GPIO_GREEDY**

	will succeed if the GPIO is already exported, but will 
	always unexport the GPIO on free.

* **LS_GPIO_WEAK**

	will fail if GPIO is already exported, will always unexport
	on free.

---

### gpio_direction

Used for setting and reading the direction of the GPIO pin.

* **DIRECTION_ERROR**

	Returned when the GPIO direction could not be read
 
* **INPUT**

	GPIO input mode, it's input value can be read

* **OUTPUT**

	GPIO output mode, it's output value can be written

---

### gpio_level

Used for setting and reading the level of the GPIO pin.

* **LEVEL_ERROR**

	Returned when the GPIO level could not be read

* **LOW**

	GPIO value low, usually logic 0 on the pin, but could be 1 if
	the gpio is inverted.

* **HIGH**

	GPIO value high, usually logic 1 on the pin, but could be 0 if
	the gpio is inverted.

---

### gpio_edge

Used for setting and reading the edge of the GPIO pin.

* **EDGE_ERROR**

	Returned when the GPIO edge could not be read

* **RISING**

	GPIO interrupt is triggered on a rising edge, e.g. logic level
	0 followed by 1

* **FALLING**

	GPIO interrupt is triggered on a falling edge, e.g. logic level
	1 followed by 0

* **NONE**

	GPIO interrupt edge is not set

* **BOTH**

	GPIO interrupt is triggered on both a falling and rising edge

---

### gpio_int_ret

Return type for blocked GPIO interrupts.

* **LS_INT_ERROR**

	Error condition

* **LS_INT_TRIGGERED**

	Interrupt was triggered

* **LS_INT_TIMEOUT**

	GPIO poll timedout

## Functions
---
### libsoc_gpio_request

```C
gpio * libsoc_gpio_request(unsigned int gpio_id, gpio_mode mode)
```

- *unsigned int* **gpio_id**

	the Linux ID number for the GPIO you wish to use

- *[gpio_mode](#gpio_mode)* **mode**

	the mode in which libsoc handles the GPIO file descriptor

Request a GPIO by it's Linux ID number and set the [gpio_mode](#gpio_mode)
under which libsoc will hold the file descriptor. Returns a malloced gpio struct
which will need to be freed by [libsoc_gpio_free](#libsoc_gpio_free) when no
longer needed.

Returns `NULL` on failure.

---

### libsoc_gpio_free

```c
int libsoc_gpio_free(gpio * gpio)
```

- *gpioi\** **gpio**

	the previously requested gpio that you wish to release

Free the memory associated with a previously requested GPIO and close the file
descriptors.

Returns `EXIT_SUCCESS`/`EXIT_FAILURE`

---

### libsoc_gpio_set_direction

```c
int libsoc_gpio_set_direction(gpio * current_gpio, gpio_direction direction)
```

- *gpio\** **current_gpio**

	requested gpio that you wish to set the direction of

- *[gpio_direction](#gpio_direction)* **direction**

	direction you wish to set the gpio to

Set the direction of a GPIO, `INPUT` for reading a GPIO, `OUTPUT` for setting a
GPIO.

Returns `EXIT_SUCCESS`/`EXIT_FAILURE`

---
### libsoc_gpio_get_direction

```c
gpio_direction libsoc_gpio_get_direction(gpio * current_gpio)
```

- *gpio\** **current_gpio**

	requested gpio that you wish to get the direction of

Get the current [gpio_direction](#gpio_direction) of a requested GPIO.

Returns [gpio_direction](#gpio_direction), `DIRECTION_ERROR` on failure,
`INPUT`/`OUTPUT` on success.

---
### libsoc_gpio_set_level

```c
int libsoc_gpio_set_level(gpio * current_gpio, gpio_level level)
```

- *gpio \** **current_gpio**

	requested gpio you wish to set the level of

- *[gpio_level](#gpio_level)* **level**

	level you wish to set the gpio to

Set the output level of a requested GPIO to `HIGH` or `LOW`.


Returns `EXIT_SUCCESS`/`EXIT_FAILURE`

---
### libsoc_gpio_get_level

```c
gpio_level libsoc_gpio_get_level(gpio * current_gpio)
```

- *gpio \** **current_gpio**

	requested gpio you wish to get the level of

Get the current [gpio_level](#gpio_level) of a requested GPIO.

Returns [gpio_level](#gpio_level), `LEVEL_ERROR` on failure,
`LOW`/`HIGH` on success.

---
### libsoc_gpio_set_edge

```c
int libsoc_gpio_set_edge(gpio * current_gpio, gpio_edge edge)
```

- *gpio \** **current_gpio**

	requested gpio you wish to set the edge of

- *[gpio_edge](#gpio_edge)* **edge**

	edge you wish to set the gpio to

Set the edge type of a requested GPIO. The edge is the type of signal
change that will trigger interrupt detection. See [gpio_edge](#gpio_edge)
for explanations of the different types of edges and how they work.

Returns `EXIT_SUCCESS`/`EXIT_FAILURE`

---

### libsoc_gpio_get_edge

```c
gpio_edge libsoc_gpio_get_edge(gpio * current_gpio)
```

- *gpio \** **current_gpio**

	requested gpio you wish to get the edge of

Get the current [gpio_edge](#gpio_edge) of a requested GPIO.

Returns [gpio_edge](#gpio_edge), `EDGE_ERROR` on failure,
`RISING`/`FALLING`/`BOTH`/`NONE` on success.

---
### libsoc_gpio_wait_interrupt

```c
int libsoc_gpio_wait_interrupt(gpio * gpio, int timeout)
```

- *gpio \** **gpio**

	requested gpio you wish to wait for an interrupt on

- *int* **timeout**

	the number of seconds to wait for the interrupt, -1 for block indefinitly

Block for a set amount of seconds (or indefinitly) waiting for an interrupt on a
GPIO to occur. If you wish to use a non-blocking interrupt mechanism see
[libsoc_gpio_callback_interrupt](#libsoc_gpio_callback_interrupt).

Returns `LS_INT_ERROR` on failure, `LS_INT_TRIGGERED` on captured interrupt,
`LS_INT_TIMEOUT` if no interrupt was captured in the specified time.

---
### libsoc_gpio_callback_interrupt

```c
int libsoc_gpio_callback_interrupt(gpio * gpio, int (*callback_fn) (void *), void *arg)
```

- *gpio \** **gpio**

	requested gpio you wish to set an interrupt handler for

- *int (\*callback_fn) (void \*)* **function**

	name of the function to use as the interrupt handler. Must match the prototype
	`int* function(void* ptr)`

- *void \** **arg**

	void casted pointer you wish to be passed to your interrupt handler

Setup an interrupt handler on a GPIO. This will start a new pthread with an infinite poll
on the GPIO waiting for the interrupt. When an interrupt is detected the supplied function
will run in the thread, and then return to waiting for an interrupt.

Only one interrupt will be queued if it arrives while your supplied function is being run,
so it is possible to miss interrupts if they happen too fast.

Returns `EXIT_SUCCESS`/`EXIT_FAILURE`

---
### libsoc_gpio_callback_interrupt_cancel

```c
int libsoc_gpio_callback_interrupt_cancel(gpio * gpio)
```

- *gpio \** **gpio**

	the gpio on which to cancel a waiting interrupt handler

Cancel a previously set interrupt handler on a GPIO. This uses the pthread_cancel function,
so it may cancel mid way through your interrupt handler function.

Returns `EXIT_SUCCESS`/`EXIT_FAILURE`
