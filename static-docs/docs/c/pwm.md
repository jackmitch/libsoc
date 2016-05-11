# PWM
---
## Data Types
---
### shared_mode

Determines the way in which libsoc handles exporting and unexporting
PWMs in the Linux subsystem.

* **LS_SHARED**

	if the pwm is already exported then it will not unexport
	the pwm on free. If it is not exported, then it will
	unexport on free.

* **LS_GREEDY**

	will succeed if the pwm is already exported, but will
	always unexport the pwm on free.

* **LS_WEAK**

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

---

### libsoc_pwm_set_enabled

```c
int libsoc_pwm_set_enabled(pwm *pwm, pwm_enabled enabled)
```

- *pwm\** **pwm**

	requested pwm that you wish to set enabled/disabled

- *pwm_enabled* **enabled**

---

### libsoc_pwm_get_enabled
---

```c
pwm_enabled libsoc_pwm_get_enabled(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the state of
---

### libsoc_pwm_set_polarity

```c
int libsoc_pwm_set_polarity(pwm *pwm, pwm_polarity polarity)
```

- *pwm\** **pwm**

	requested pwm that you wish to set the polarity of

- *pwm_polarity* **polarity**

---

### libsoc_pwm_get_polarity

```c
pwm_polarity libsoc_pwm_get_polarity(pwm *pwm)
```
---

- *pwm\** **pwm**

	requested pwm that you wish to get the polarity of

### libsoc_pwm_set_duty_cycle

```c
int libsoc_pwm_set_duty_cycle(pwm *pwm, unsigned int duty)
```

- *pwm\** **pwm**

	requested pwm that you wish to set the duty cycle of

- *unsigned int* **duty**

---

### libsoc_pwm_get_duty_cycle

```c
int libsoc_pwm_get_duty_cycle(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the duty cycle of

---

### libsoc_pwm_set_period

```c
int libsoc_pwm_set_period(pwm *pwm, unsigned int period)
```

- *pwm\** **pwm**

	requested pwm that you wish to set the period of

- *unsigned int* **period**

---

### libsoc_pwm_get_period

```c
int libsoc_pwm_get_period(pwm *pwm)
```

- *pwm\** **pwm**

	requested pwm that you wish to get the period of
