CPATH += ../lib/include
LIBRARY_PATH := ../lib/.libs $(LIBRARY_PATH)
export
test: adc_test adc_test.test adc_test.py.test \
	pwm_test pwm_test.test pwm_test.py.test
%: %.c
	gcc -o $@ $< -lsoc
# example: `make ARGS="0 2 shared normal 1000 500" pwm_test.test` on ARTIK 710
%.test: %
	./$< $(ARGS)
