/*
 * logger.h
 *
 *  Created on: 2016�~9��23��
 *      Author: poa116
 */

#ifndef MSP432_DATA_LOGGER_V1_1_LOGGER_H_
#define MSP432_DATA_LOGGER_V1_1_LOGGER_H_


struct time_drift{
	signed long long now_time;
	int32_t drift_unit;
			};


typedef struct Logger{
				int status;  // 0 view mode, 1 Rec mode
				struct time_drift start_dirft;
				struct time_drift end_dirft;
				signed long long NOW_Time;
				int data_is_full;
				}data_loggerV2;

typedef struct gps_data{
		int status;  // 0

		int checkGps;
		int gps_right_time;

		signed long long gps_time;
		signed long long gps_time2;
}GPS_DATA;

#endif /* MSP432_DATA_LOGGER_V1_1_LOGGER_H_ */
