#ifndef _LIBSOC_PWM_H_
#define _LIBSOC_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct pwm
 * \brief representation of a single requested pwm
 * \param unsigned int pwm - pwm num
 * \param unsigned int pwm_chip - pwm chip num
 * \param int enabled_fd - file descriptor to pwm enable file
 * \param int duty_fd - file descriptor to pwm duty_cycle file
 * \param int period_fd - file descriptor to pwm period file
 * \param int shared - set if the request flag was shared and the pwm was
 *  exported on request
 */

typedef struct {
	unsigned int chip;
	unsigned int pwm;
	int enable_fd;
	int duty_fd;
	int period_fd;
	int shared;
} pwm;

/**
 * \enum pwm_enabled
 * \brief defined values for pwm enabled/disabled
 */

typedef enum {
	ENABLED_ERROR = -1,
	DISABLED = 0,
	ENABLED = 1,
} pwm_enabled;

/**
 * \enum pwm_polarity
 * \brief defined values for pwm polarity
 */

typedef enum {
	POLARITY_ERROR = -1,
	NORMAL = 0,
	INVERSED = 1,
} pwm_polarity;

/**
 * \enum shared_mode
 *
 * LS_SHARED - if the pwm is already exported then it will not unexport
 *             the PWM on free. If it is not exported, then it will
 *             unexport on free.
 *
 * LS_GREEDY - will succeed if the PWM is already exported, but will
 *             always unexport the PWM on free.
 *
 * LS_WEAK   - will fail if PWM is already exported, will always unexport
 *             on free.
 */

enum shared_mode {
	LS_SHARED,
	LS_GREEDY,
	LS_WEAK,
};

/**
 * \fn pwm* libsoc_pwm_request(unsigned int pwm_chip, unsigned int pwm_num)
 * \brief request a pwm to use
 * \param unsigned int pwm_chip - the chip number that controls the pwm you
    wish to use
 * \param unsigned int pwm_num - the pwm number number within your pwm chip
 * \param unsigned int mode - mode for opening PWM
 * \return pointer to pwm* on success NULL on fail
 *
 */

pwm* libsoc_pwm_request(unsigned int pwm_chip, unsigned int pwm_num,
  enum shared_mode mode);

/**
 * \fn int libsoc_pwm_free(pwm* pwm)
 * \brief free a previously requested pwm
 * \param pwm* pwm - valid pointer to a requested pwm
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_pwm_free(pwm *pwm);

/**
 * \fn int libsoc_pwm_set_enabled(pwm *pwm, pwm_enabled enabled)
 * \brief set PWM enabled/disabled
 * \param pwm* pwm - pointer to pwm struct on which to set enabled
 * \param pwm_enabled enabled - enumerated enabled status, ENABLED or DISABLED
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_pwm_set_enabled(pwm *pwm, pwm_enabled enabled);

/**
 * \fn libsoc_pwm_get_enabled(pwm *pwm)
 * \brief get the current enabled status of the PWM
 * \param pwm *pwm - pointer to pwm struct on which to get the enabled status
 * \return current PWM enabled status, ENABLED,DISABLED or ERROR_ENABLED
 */

pwm_enabled libsoc_pwm_get_enabled(pwm *pwm);

/**
 * \fn libsoc_pwm_set_polarity(pwm *pwm, pwm_polarity polarity)
 * \brief set the PWM polarity to normal or inverted
 * \param pwm *pwm - pointer to pwm struct on which to set the polarity
 * \param pwm_polarity = enumerated pwm_polarity NORMAL or INVERTED
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_pwm_set_polarity(pwm *pwm, pwm_polarity polarity);

/**
 * \fn libsoc_pwm_get_polarity(pwm *pwm)
 * \brief gets the current pwm polarity
 * \param pwm *pwm - pointer to pwm struct on which to get the polarity
 * \return pwm_polarity - NORMAL, INVERTED or ERROR_POLARITY
 */

pwm_polarity libsoc_pwm_get_polarity(pwm *pwm);

/**
 * \fn libsoc_pwm_set_duty_cycle(pwm *pwm, int duty)
 * \brief set the PWM duty cycle (active time of the PWM signal)
 * \param pwm *pwm - pointer to valid pwm struct
 * \param int duty - duty value in nanoseconds, must be less than period
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_pwm_set_duty_cycle(pwm *pwm, unsigned int duty);

/**
 * \fn libsoc_pwm_get_duty_cycle(pwm *pwm)
 * \brief gets the current pwm duty cycle
 * \param pwm *pwm - pointer to valid pwm struct
 * \return duty_cycle - integer, -1 on failure
 */

int libsoc_pwm_get_duty_cycle(pwm *pwm);

/**
 * \fn libsoc_pwm_set_period(pwm *pwm, unsigned int period)
 * \brief set the PWM period (sum of the active and inactive
 * time of the PWM)
 * \param pwm *pwm - pointer to valid pwm struct
 * \param int period - period value in nanoseconds
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_pwm_set_period(pwm *pwm, unsigned int period);

/**
 * \fn libsoc_pwm_get_period(pwm *pwm)
 * \brief gets the current pwm period
 * \param pwm *pwm - pointer to valid pwm struct
 * \return period - integer, -1 on failure
 */

int libsoc_pwm_get_period(pwm *pwm);

#ifdef __cplusplus
}
#endif
#endif
