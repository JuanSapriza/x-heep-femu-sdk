#include "relativeEnergy.h"
#include "delineationConditioned.h"

//Previously-found short and long energies for RelEn signal generation
PI_L2 uint32_t lastShortEnergy = 0;
PI_L2 uint32_t lastLongEnergy = 0;
//circular ecg buffer and its pointers
PI_L2 int16_t ecgBuffer_re[LONG_WINDOW + 1];
PI_L2 uint16_t ecgBufferPointer = 0;
PI_L2 uint16_t lastEcgBufferPointer = 0;
PI_L2 uint16_t currentShortWindowPointer = 0;
PI_L2 uint16_t lastShortWindowPointer = 0;
PI_L2 uint16_t relEnBufferPointer = (LONG_WINDOW / 2);
PI_L2 int8_t start = 1;
PI_L2 uint16_t multiplier = 10000;

void clearAndResetRelEn() {
	//Previously-found short and long energies for RelEn signal generation
	lastShortEnergy = 0;
	lastLongEnergy = 0;

	//circular ecg buffer and its pointers
	ecgBufferPointer = 0;
	lastEcgBufferPointer = 0;
	currentShortWindowPointer = 0;
	lastShortWindowPointer = 0;
	relEnBufferPointer = (LONG_WINDOW / 2);
	start = 1;
}

//This is the main function where rel-En coefficients are generated
uint16_t getRelEnCoefficients(uint8_t fillingBuffer, int16_t sample) {

	//If this is the first window, start computing long and short energy sum

	if (fillingBuffer == 1) {
	 	
		if (ecgBufferPointer >= (LONG_WINDOW / 2 - SHORT_WINDOW_HALF) && ecgBufferPointer <= (LONG_WINDOW / 2 + SHORT_WINDOW_HALF)) {
			lastShortEnergy += (sample) * (sample);
		}

		lastLongEnergy += (sample) * (sample);
		

		if (ecgBufferPointer == LONG_WINDOW - 1) {
			lastEcgBufferPointer = 0;
			currentShortWindowPointer = LONG_WINDOW / 2 + SHORT_WINDOW_HALF + 1;
			lastShortWindowPointer = LONG_WINDOW / 2 - SHORT_WINDOW_HALF;
		}
 
		return 0;

	}
	else { //If not, compute Rel-En coefficient c(n)

 
		lastLongEnergy = lastLongEnergy + (ecgBuffer_re[ecgBufferPointer] * ecgBuffer_re[ecgBufferPointer]) - (ecgBuffer_re[lastEcgBufferPointer] * ecgBuffer_re[lastEcgBufferPointer]);

		lastShortEnergy = lastShortEnergy + (ecgBuffer_re[currentShortWindowPointer]*ecgBuffer_re[currentShortWindowPointer]) - (ecgBuffer_re[lastShortWindowPointer]*ecgBuffer_re[lastShortWindowPointer]);

		lastEcgBufferPointer++;
		if (lastEcgBufferPointer > LONG_WINDOW) {
			lastEcgBufferPointer = 0;
		}
		currentShortWindowPointer++;
		if (currentShortWindowPointer > LONG_WINDOW) {
			currentShortWindowPointer = 0;
		}
		lastShortWindowPointer++;
		if (lastShortWindowPointer > LONG_WINDOW) {
			lastShortWindowPointer = 0;
		}
 
		uint16_t result = 0;
 
		if (lastLongEnergy > multiplier) {
			result = lastShortEnergy / (lastLongEnergy / multiplier);
		}
		else {
			if(lastLongEnergy != 0){
				result = (lastShortEnergy * multiplier) / lastLongEnergy;
			} else {
				result = (lastShortEnergy * multiplier);
			}
		}

		return result;
 
	}

}

void relEn_w(int32_t *arg[]){

	int16_t *ecg_w = (int16_t*) arg[0];
	int16_t *out = (int16_t*) arg[1];
	int16_t rawRelEn = 0;

 
    for(int32_t j = 0; j < dim; j++)
    {	
		rawRelEn = 0; 

    	if (start == 0)
    	{ 
			ecgBuffer_re[ecgBufferPointer] = ecg_w[j];
  
			uint16_t relEnOutput = getRelEnCoefficients(start, ecg_w[j]);

			int16_t currentECG = ecgBuffer_re[relEnBufferPointer];
			rawRelEn = (relEnOutput * currentECG) / multiplier;
			ecgBufferPointer++;
			relEnBufferPointer++;
 
		} //The buffer is initially being filled
		else {
 		 
			ecgBuffer_re[ecgBufferPointer] = ecg_w[j];
 
			uint16_t relEnOutput = getRelEnCoefficients(start, ecg_w[j]);
				
			if (ecgBufferPointer == LONG_WINDOW - 1) {
				rawRelEn = (relEnOutput * ecgBuffer_re[relEnBufferPointer]) / multiplier;
				start = 0;
				relEnBufferPointer++;
			}
			ecgBufferPointer++;
		} 

		if (relEnBufferPointer > LONG_WINDOW) {
			relEnBufferPointer = 0;
		}
		if (ecgBufferPointer > LONG_WINDOW) {
			ecgBufferPointer = 0;
		}
		out[j] = rawRelEn;
	}
}
