/*
 * processing.h
 *
 *  Created on: 29 Jan 2022
 *      Author: wokad
 */

#ifndef CORE_INC_PROCESSING_H_
#define CORE_INC_PROCESSING_H_

typedef enum {
	TURNED_OFF, SLOW, FAST
} FanState;

extern void processingInit();
#endif /* CORE_INC_PROCESSING_H_ */
