#ifndef ALTITUDE_H_
#define ALTITUDE_H_

//static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)

void
ADCIntHandler(void);

void
initADC (void);

int32_t
computeAltitude(void);

void
resetAltitude(void);

int32_t
percentAltitude(void);

circBuf_t*
bufferLocation(void);

#endif /*ALTITUDE_H_*/
