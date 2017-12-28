#pragma once

typedef struct SA_context SAC;

enum ADJUST_MODE
{
	ADJUST = 0,
	NO_ADJUST
};

typedef enum
{
	SA_OK = 0,
	SA_FAILED
} SAcode;

SAC* SA_initialize();

// Start processing non-blocking
void SA_start( SAC* sa );

// Set whether to adjust or not
void SA_setAdjustMode( SAC* sa, ADJUST_MODE mode );

// Get average attenuated peak
// Returns SA_OK for correct operation
SAcode SA_getAverage( SAC* sa, float& avg );

// Set target volume
SAcode SA_setTargetVolume( SAC* sa, const float targetVolume );

// Stop processing audio
SAcode SA_stop( SAC* sa );
