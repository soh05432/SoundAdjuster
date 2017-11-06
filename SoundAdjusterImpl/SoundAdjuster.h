#pragma once

typedef struct SA_context SA;

SA* SA_initialize();

// Non-blocking, uses std::async to run a separate thread for audio processing loop
void SA_start( SA* sa );

// 
void SA_stop( SA* sa );
