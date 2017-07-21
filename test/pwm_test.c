#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libsoc_pwm.h"
#include "libsoc_debug.h"

/**
 *
 * This pwm_test is intended to be run on beaglebone white hardware,
 * however, it will work on any board with a PWM sysfs implementation.
 *
 * The PWM sysfs interface was introduced in linux 3.12, if your kernel
 * is older than this you will need to update or backport the PWM sysfs
 * patch.
 *
 * PWM sysfs Documentation: https://www.kernel.org/doc/Documentation/pwm.txt
 *
 */

#define PWM_OUTPUT_CHIP 0
#define PWM_CHIP_OUTPUT 1

int main(int argc, char **argv)
{
  int ret = EXIT_SUCCESS, chip, output;
  long period, duty_cycle;
  chip = argc > 1 ? atoi(argv[1]) : PWM_OUTPUT_CHIP;
  output = argc > 2 ? atoi(argv[2]) : PWM_CHIP_OUTPUT;
  period = argc > 3 ? atol(argv[3]) : 10;
  duty_cycle = argc > 4 ? atol(argv[4]) : 5;
  libsoc_set_debug(1);
  fprintf(stderr, "Initializing PWM with parameters %d, %d, %d, %d\n",
          chip, output, period, duty_cycle);

  pwm *pwm = libsoc_pwm_request(chip, output, LS_PWM_SHARED);

  if (!pwm)
  {
    printf("Failed to get PWM\n");
    goto fail;
  }

  libsoc_pwm_set_enabled(pwm, ENABLED);

  int enabled = libsoc_pwm_get_enabled(pwm);

  if (!enabled)
  {
    printf("Failed enabling test\n");
    ret = EXIT_FAILURE;
    goto fail;
  }

  libsoc_pwm_set_enabled(pwm, DISABLED);

  enabled = libsoc_pwm_get_enabled(pwm);

  if (enabled)
  {
    printf("Failed disabling test\n");
    ret = EXIT_FAILURE;
    goto fail;
  }

  libsoc_pwm_set_period(pwm, period);

  int current_period = libsoc_pwm_get_period(pwm);

  if (current_period != period)
  {
    printf("Failed period test\n");
    goto fail;
  }

  libsoc_pwm_set_duty_cycle(pwm, duty_cycle);

  int current_duty = libsoc_pwm_get_duty_cycle(pwm);

  if (current_duty != duty_cycle)
  {
    printf("Failed duty test\n");
    goto fail;
  }

  // This command should fail as the duty should not
  // be allowed to be greater than the period
  libsoc_pwm_set_duty_cycle(pwm, period + duty_cycle);

  current_duty = libsoc_pwm_get_duty_cycle(pwm);

  if (current_duty != duty_cycle)
  {
    printf("Failed duty test, this may be a problem with the kernel driver, continuing...\n");
  }
  else
  {
    printf("Driver correctly responded to setting duty higher than period, with error\n");
  }

  libsoc_pwm_set_polarity(pwm, INVERSED);

  int polarity = libsoc_pwm_get_polarity(pwm);

  if (polarity != INVERSED)
  {
    printf("Failed polarity test, this may be an error, or it might not be supported by your driver\n");
  }

  libsoc_pwm_set_polarity(pwm, NORMAL);

  polarity = libsoc_pwm_get_polarity(pwm);

  if (polarity != NORMAL)
  {
    printf("Failed polarity test, this may be an error, or it might not be supported by your driver\n");
  }

  fail:

  libsoc_pwm_free(pwm);

  return ret;
}
