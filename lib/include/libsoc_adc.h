#ifndef _LIBSOC_ADC_H_
#define _LIBSOC_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

static const char ADC_DEVICE_FORMAT[][64] = {
	"/sys/devices/12d10000.adc/iio:device%d",  // ARTIK 1020
	"/sys/devices/platform/c0000000.soc/c0053000.adc/iio:device%d", // 710
	"/sys/devices/126c0000.adc/iio:device%d"  // ARTIK 520
};

static const char ADC_READ_FORMAT[][32] = {
	"/in_voltage%d_raw"
};

/**
 * \struct adc 
 * \brief representation of a single requested adc
 * \param unsigned int adc - adc num
 * \param unsigned int adc_chip - adc chip num
 * \param int fd - file descriptor to adc file
 */

typedef struct {
	unsigned int chip;
	unsigned int adc;
	int fd;
} adc;

/**
 * \fn adc* libsoc_adc_request(unsigned int adc_chip, unsigned int adc_num)
 * \brief request an adc to use
 * \param unsigned int adc_chip - the chip number that controls the adc you
    wish to use
 * \param unsigned int adc_num - the adc number number within your adc chip
 * \return pointer to adc* on success NULL on fail
 *
 */

adc* libsoc_adc_request(unsigned int adc_chip, unsigned int adc_num);

/**
 * \fn int libsoc_adc_get_value(adc *adc)
 * \brief read value from ADC
 * \param pointer to adc*
 * \return int positive value read on success -1 on fail
 *
 */

int libsoc_adc_get_value (adc *adc);

/**
 * \fn int libsoc_adc_free(adc* adc)
 * \brief free a previously requested adc
 * \param adc* adc - valid pointer to a requested adc
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int libsoc_adc_free(adc *adc);

#ifdef __cplusplus
}
#endif
#endif
/*
 * vim: tabstop=8 expandtab shiftwidth=2 softtabstop=2
 */
