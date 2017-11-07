#pragma once

typedef struct SA_context SAC;

enum ADJUST_MODE
{
	ADJUST = 0,
	NO_ADJUST
};

SAC* SA_initialize();

// Start processing non-blocking
void SA_start( SAC* sa );

// Set whether to adjust or not
void SA_setAdjustMode( SAC* sa, ADJUST_MODE mode );

// Stop processing audio
void SA_stop( SAC* sa );
