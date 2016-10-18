/*
 * logger.h
 *
 *  Created on: 2016¦~9¤ë23¤é
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
				time_drift start_dirft;
				time_drift end_dirft;
				signed long long NOW_Time;
				int data_is_full;

				};


#endif /* MSP432_DATA_LOGGER_V1_1_LOGGER_H_ */
