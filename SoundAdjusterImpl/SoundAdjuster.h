#pragma once

struct SA_context;
typedef SA_context SA;

SA* SA_initialize();

void SA_start( SA* sa );

void SA_stop( SA* sa );