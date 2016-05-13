#ifndef RANGING_H_
#define RANGING_H_

#include "board.h"

#define SHORT_FRONT_DIST		1.0
#define SHORT_BACK_DIST			1.0
#define SHORT_FRONT_HEIGHT		1.0
#define SHORT_BACK_HEIGHT		1.0
#define SHORT_FRONT_LEFT_DIST	1.0
#define SHORT_FRONT_RIGHT_DIST	1.0
#define SHORT_BACK_LEFT_DIST	1.0
#define SHORT_BACK_RIGHT_DIST	1.0
#define SHORT_RIGHT_DIST_AVG	(0.5*(SHORT_FRONT_LEFT_DIST + SHORT_FRONT_RIGHT_DIST))
#define SHORT_RIGHT_AVG_INV		(1.0 / SHORT_RIGHT_DIST_AVG)
#define SHORT_FRONT_DIST_INV	(1.0 / SHORT_FRONT_DIST)
#define SHORT_BACK_DIST_INV		(1.0 / SHORT_BACK_DIST)
#define SHORT_AXIS_SUM_INV		(1.0 / (SHORT_FRONT_DIST + SHORT_BACK_DIST))

#define LONG_FRONT_DIST			1.0
#define LONG_BACK_DIST			1.0
#define LONG_FRONT_LEFT_DIST	1.0
#define LONG_FRONT_RIGHT_DIST	1.0
#define LONG_BACK_LEFT_DIST		1.0
#define LONG_BACK_RIGHT_DIST	1.0
#define LONG_FRONT_DIST_INV		(1.0 / LONG_FRONT_DIST)
#define LONG_BACK_DIST_INV		(1.0 / LONG_BACK_DIST)
#define LONG_AXIS_SUM_INV		(1.0 / (LONG_FRONT_DIST + LONG_BACK_DIST))

#define _LPC_ADC_ID 		LPC_ADC
#define _LPC_ADC_IRQ 		ADC_IRQn
#define LONG_FRONT_INITIAL	45.0	// cm
#define LONG_BACK_INITIAL	45.0	// cm
#define SHORT_FRONT_INITIAL	2.5		// cm
#define SHORT_BACK_INITIAL	2.5		// cm
#define ALPHA 				.6
#define BETA				(1 - ALPHA)

volatile uint8_t Burst_Mode_Flag, Interrupt_Continue_Flag;
volatile uint8_t channelTC, dmaChannelNum;
volatile uint8_t ADC_Interrupt_Done_Flag;
float ShortRangingMovingAverage[4];
float LongRangingMovingAverage[4];
uint32_t DMAbuffer;
uint16_t ShortRangingDataRaw[4];
uint16_t LongRangingDataRaw[4];
ADC_CLOCK_SETUP_T ADCSetup;

typedef struct{

  float sensor0;
  float sensor1;
  float sensor2;
  float sensor3;

} rangingData;

/* Starts at 0.34V, goes to 2.43V, increments in intervals of 0.01V */
static const float shortRangingDistanceLUT[] =
{    17.664, 17.139, 16.641, 16.169, 15.719, 15.292, 14.885, 14.499, 14.130, 13.780,
     13.445, 13.126, 12.822, 12.532, 12.254, 11.989, 11.735, 11.492, 11.260, 11.037,
     10.823, 10.618, 10.421, 10.232, 10.050, 9.876, 9.707, 9.545, 9.389, 9.238,
     9.092, 8.951, 8.815, 8.684, 8.557, 8.433, 8.314, 8.198, 8.085, 7.976,
     7.869, 7.766, 7.665, 7.567, 7.472, 7.379, 7.288, 7.200, 7.113, 7.029,
     6.946, 6.865, 6.786, 6.709, 6.633, 6.559, 6.486, 6.415, 6.344, 6.276,
     6.208, 6.142, 6.076, 6.012, 5.949, 5.887, 5.826, 5.766, 5.707, 5.648,
     5.591, 5.534, 5.478, 5.423, 5.369, 5.316, 5.263, 5.211, 5.159, 5.109,
     5.058, 5.009, 4.960, 4.912, 4.864, 4.817, 4.770, 4.724, 4.679, 4.634,
     4.589, 4.546, 4.502, 4.459, 4.417, 4.375, 4.333, 4.292, 4.251, 4.211,
     4.171, 4.131, 4.092, 4.054, 4.016, 3.978, 3.940, 3.903, 3.867, 3.830,
     3.794, 3.759, 3.723, 3.689, 3.654, 3.620, 3.586, 3.552, 3.519, 3.486,
     3.454, 3.421, 3.389, 3.358, 3.326, 3.295, 3.265, 3.234, 3.204, 3.174,
     3.145, 3.115, 3.086, 3.058, 3.029, 3.001, 2.973, 2.945, 2.918, 2.891,
     2.864, 2.837, 2.811, 2.785, 2.759, 2.733, 2.708, 2.683, 2.658, 2.633,
     2.609, 2.584, 2.560, 2.537, 2.513, 2.490, 2.467, 2.444, 2.421, 2.398,
     2.376, 2.354, 2.332, 2.311, 2.289, 2.268, 2.247, 2.226, 2.205, 2.185,
     2.165, 2.144, 2.125, 2.105, 2.085, 2.066, 2.047, 2.028, 2.009, 1.990,
     1.972, 1.954, 1.935, 1.917, 1.900, 1.882, 1.865, 1.847, 1.830, 1.813,
     1.796, 1.780, 1.763, 1.747, 1.731, 1.715, 1.699, 1.683, 1.667, 1.652,
     1.636, 1.621, 1.606, 1.591, 1.577, 1.562, 1.547, 1.533, 1.519, 1.505 };

/* Starts at 0.49V, goes to 2.73V, increments in intervals of 0.01V. */
static const float longRangingDistanceLUT[] =
{    136.944, 134.068, 131.290, 128.606, 126.013, 123.507, 121.085, 118.743, 116.479, 114.289,
     112.171, 110.121, 108.138, 106.218, 104.359, 102.559, 100.815, 99.126, 97.489, 95.903,
     94.364, 92.872, 91.425, 90.021, 88.659, 87.336, 86.051, 84.804, 83.592, 82.414,
     81.269, 80.156, 79.073, 78.020, 76.995, 75.997, 75.026, 74.080, 73.158, 72.260,
     71.384, 70.530, 69.697, 68.884, 68.091, 67.317, 66.561, 65.822, 65.101, 64.395,
     63.706, 63.031, 62.371, 61.726, 61.094, 60.475, 59.869, 59.275, 58.694, 58.123,
     57.564, 57.016, 56.478, 55.950, 55.432, 54.923, 54.423, 53.932, 53.450, 52.976,
     52.510, 52.051, 51.601, 51.157, 50.721, 50.291, 49.868, 49.452, 49.042, 48.638,
     48.239, 47.847, 47.460, 47.079, 46.703, 46.331, 45.965, 45.604, 45.248, 44.896,
     44.548, 44.205, 43.866, 43.532, 43.201, 42.874, 42.552, 42.232, 41.917, 41.605,
     41.297, 40.992, 40.690, 40.391, 40.096, 39.804, 39.515, 39.229, 38.946, 38.665,
     38.388, 38.113, 37.841, 37.571, 37.304, 37.040, 36.778, 36.519, 36.262, 36.007,
     35.754, 35.504, 35.256, 35.011, 34.767, 34.526, 34.286, 34.049, 33.814, 33.581,
     33.349, 33.120, 32.892, 32.667, 32.443, 32.221, 32.001, 31.782, 31.566, 31.351,
     31.137, 30.926, 30.716, 30.507, 30.301, 30.096, 29.892, 29.690, 29.490, 29.291,
     29.093, 28.897, 28.703, 28.510, 28.318, 28.128, 27.939, 27.751, 27.565, 27.381,
     27.197, 27.015, 26.834, 26.655, 26.477, 26.300, 26.124, 25.950, 25.777, 25.605,
     25.434, 25.264, 25.096, 24.929, 24.763, 24.598, 24.435, 24.272, 24.111, 23.951,
     23.791, 23.633, 23.476, 23.320, 23.166, 23.012, 22.859, 22.708, 22.557, 22.407,
     22.259, 22.111, 21.965, 21.819, 21.675, 21.531, 21.389, 21.247, 21.106, 20.967,
     20.828, 20.690, 20.553, 20.417, 20.282, 20.148, 20.015, 19.882, 19.751, 19.620,
     19.491, 19.362, 19.234, 19.107, 18.981, 18.855, 18.731, 18.607, 18.484, 18.362,
     18.241, 18.120, 18.000, 17.882, 17.764 };


float sin_inv(float x);
void computePositionAttitudeRanging();
rangingData getLongDistance();
void convertVoltage(uint16_t data, uint8_t sensor);
rangingData getShortDistance();
void ADC_IRQHandler();
void rangingSensorsInit();
void convertVoltageShort(uint8_t sensor);
void convertVoltageLong(uint8_t sensor);
void Ranging_Int_Measure();
void initADCChannel(uint8_t channel, uint8_t port, uint8_t pin, uint8_t func, float init_val);

#endif
