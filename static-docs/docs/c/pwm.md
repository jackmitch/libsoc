# PWM
---
## Data Types
---
### shared_mode

Determines the way in which libsoc handles exporting and unexporting
PWMs in the Linux subsystem.

* **LS_PWM_SHARED**

	if the pwm is already exported then it will not unexport
	the pwm on free. If it is not exported, then it will
	unexport on free.

* **LS_PWM_GREEDY**

	will succeed if the pwm is already exported, but will
	always unexport the pwm on free.

* **LS_PWM_WEAK**

	will fail if pwm is already exported, will always unexport
	on free.

### pwm_enabled

Used for setting and reading the enabled state of the PWM device

* **ENABLED_ERROR**

	Returned when the enabled state could not be read

* **DISABLED**

	PWM disabled mode, the pwm is not currently running

* **ENABLED**

	PWM enabled mode, the pwm is currently running

---

### pwm_polarity

Used for setting and reading the polarity of the PWM device

* **POLARITY_ERROR**

	Returned when the polarity could not be read

* **NORMAL**

	Polarity mode is normal

* **INVERSED**

	Polarity mode is inversed

## Functions
---
### libsoc_pwm_request

```c
pwm* libsoc_pwm_request(unsigned int pwm_chip, unsigned int pwm_num, enum shared_mode mode)
```

- *unsigned int* **pwm_chip**

	the pwm chip number which controls the pwm you wish to use

- *unsigned int* **pwm_num**

	the pwm number within your specified pwm chip

- *shared_mode* **mode**

	the mode in which libsoc handles the PWM file descriptor

Request a pwm by specifying its pwm chip number and the pwm number it is on that chip.
Set [shared_mode](#shared_mode) to handle how libsoc manages the file descriptor.
Returns a malloced pwm struct which will need to be freed by [libsoc_pwm_free](#libsoc_pwm_free)
when no longer needed.

---

### libsoc_pwm_free

```c
int libsoc_pwm_free(pwm *pwm)
```

- *pwm\** **pwm**

	the previously requested pwm that you wish to release

Free the memory associated with a previously requested PWM device and close the file
descriptors.

---

### libsoc_pwm_set_enabled

```c
int libsoc_pwm_set_enabled(pwm *pwm, pwm_enabled enabled)
```

- *pwm\** **pwm**

	requested pwm that you wish to set enabled/disabled

- *[pwm_enabled](#pwm_enabled)* **enabled**

	state you wish to set the pwm device to

Set the state of a PWM device, `ENABLED` to enable to the device, `DISABLED` to
disable the device.

---

### libsoc_pwm_get_enabled

```c
pwm_enabled libsoc_pwm_get_enabled(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the state of

Get the current [pwm_enabled](#pwm_enabled) state of a requested PWM device.

---

### libsoc_pwm_set_period

```c
int libsoc_pwm_set_period(pwm *pwm, unsigned int period)
```

- *pwm\** **pwm**

	requested pwm that you wish to set the period of

- *unsigned int* **period**

	the total period of the pwm signal in nanoseconds

Set the period of a PWM device, the value is specified in nanoseconds and is the sum
of the active and inactive time of the signal.

---

### libsoc_pwm_get_period

```c
int libsoc_pwm_get_period(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the period of

Get the currently set perioid of the PWM device in nanoseconds.

---

### libsoc_pwm_set_duty_cycle

```c
int libsoc_pwm_set_duty_cycle(pwm *pwm, unsigned int duty)
```

- *pwm\** **pwm**

	requested pwm that you wish to set the duty cycle of

- *unsigned int* **duty**

	the duty cycle in nanoseconds


Set the duty cycle of the specified PWM device in nanoseconds. The duty cycle is the
active time of the PWM signal and must be less than the period.

---

### libsoc_pwm_get_duty_cycle

```c
int libsoc_pwm_get_duty_cycle(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the duty cycle of

Get the currently set duty cycle of a requested PWM device in nanoseconds.

---

### libsoc_pwm_set_polarity

```c
int libsoc_pwm_set_polarity(pwm *pwm, pwm_polarity polarity)
```

- *pwm\** **pwm**

	requested pwm that you wish to set the polarity of

- *pwm_polarity* **polarity**

	polarity you wish to set the pwm device to

Set the polarity of a PWM device, `NORMAL` will give you the default polarity,
`INVERSED` will give you the inverse polarity of normal.

Polarity support is optional, ensure your PWM devices kernel driver is capable before
relying on this support. The value -1 will be returned on failure to set the polarity.

---

### libsoc_pwm_get_polarity

```c
pwm_polarity libsoc_pwm_get_polarity(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the polarity of

Get the current [pwm_polarity](#pwm_polarity) of a requested PWM device.

---

