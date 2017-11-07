#include "SoundAdjuster.h"

#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h> // IAudioSessionManager2
#include <endpointvolume.h> // IAudioMeterInformation

#include "Buffers.h"

#include <vector>
#include <future>

#define EXIT_ON_ERROR(hres) if (FAILED(hres)) {goto Exit;}
#define SAFE_RELEASE(var) if (var != NULL) { var->Release(); var = NULL;}

class SA_audioSessionNotification : public IAudioSessionNotification
{
public:

	std::vector<IAudioMeterInformation*> m_pMeterInformations;

	SA_audioSessionNotification()
	{

	}

	virtual HRESULT STDMETHODCALLTYPE OnSessionCreated( IAudioSessionControl *NewSession ) override
	{
		HRESULT hr;
		IAudioMeterInformation* iAudioMeterInformation;
		hr = NewSession->QueryInterface( &iAudioMeterInformation );

		if ( hr == S_OK && iAudioMeterInformation != NULL )
		{
			m_pMeterInformations.push_back( iAudioMeterInformation );
		}

		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,
		_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject ) override {
		return 0;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void ) override { return 0; }
	virtual ULONG STDMETHODCALLTYPE Release( void ) override { return 0; }
};

struct SA_context
{
    // Audio level caps defined by hardware
    float m_maxDB; // -4.6875f
    float m_minDB; // -95.2188f

    // Maximum cap to as m_maxDB is usually too loud
    float m_maxPeak; // 0.30f

    // Current target volume level
    float m_targetVolume; // 0.025f;

	std::future<void> m_loop;

    // Control whether sound is adjusted
	std::atomic<ADJUST_MODE> m_adjust;

    // Control whether sound is processed
    std::atomic<bool> m_running;

    // Storage for volume level data
    MovingAvgFilter<double> m_sampledMaf;
    CircularBuffer<double> m_averageBuf;

    // Windows Audio Session API classes for the actual sampling and adjusting
    IAudioEndpointVolume* m_pEndpointVolume;
    IAudioSessionEnumerator* m_pSessionEnumerator;
    SA_audioSessionNotification* m_pSessionNotification;
};

SA_context* SA_initialize()
{
    SA_context* sac = new SA_context;

    sac->m_maxDB = -4.6875f;
    sac->m_minDB = -95.2188f;
    sac->m_maxPeak = 0.3f;
    sac->m_targetVolume = 0.025f;
    sac->m_adjust = ADJUST_MODE::ADJUST;
    sac->m_running = true;

    CoInitializeEx( NULL, COINIT_MULTITHREADED );

    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance( __uuidof( MMDeviceEnumerator ), NULL, CLSCTX_ALL, __uuidof( IMMDeviceEnumerator ), (void**)&pEnumerator );

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint( eRender, eMultimedia, &pDevice );

	hr = pDevice->Activate( __uuidof( IAudioEndpointVolume ), CLSCTX_ALL, NULL, (void**)&sac->m_pEndpointVolume );

    IAudioSessionManager2* pSessionManager = nullptr;
    hr = pDevice->Activate( __uuidof( IAudioSessionManager2 ), CLSCTX_ALL, NULL, (void**)&pSessionManager );
    
    sac->m_pSessionNotification = new SA_audioSessionNotification();
    hr = pSessionManager->RegisterSessionNotification( sac->m_pSessionNotification );
    
    hr = pSessionManager->GetSessionEnumerator( &sac->m_pSessionEnumerator );

    return sac;
}

void updateSessionMeters( SA_context* sac )
{
	HRESULT hr;
	IAudioSessionControl* iAudioSessionControl;
	IAudioMeterInformation* iAudioMeterInformation;

	int numSessions = 0;
	hr = sac->m_pSessionEnumerator->GetCount( &numSessions );

	sac->m_pSessionNotification->m_pMeterInformations.clear();

	for ( int i = 0; i < numSessions; i++ )
	{
		hr = sac->m_pSessionEnumerator->GetSession( i, &iAudioSessionControl );

		hr = iAudioSessionControl->QueryInterface( &iAudioMeterInformation );

		sac->m_pSessionNotification->m_pMeterInformations.push_back( iAudioMeterInformation );
	}

	return;
}

void SA_loop( SA_context* sac )
{
	float peakValHolder = 0.f;
	float peakValMax = -1.0;
	float peakValMaxScaled = 0.;
	float currentDb = 0.f;
	float currentScale = 0.f;
	float dScaled = 0.f;
	float dDb = 0.f;

	const float ln10 = (float)log( 10 );

	updateSessionMeters( sac );

	while ( sac->m_running )
	{
		peakValMax = -1.0;

		HRESULT hr;

		for ( int i = 0; i < (int)sac->m_pSessionNotification->m_pMeterInformations.size(); i++ )
		{
			hr = sac->m_pSessionNotification->m_pMeterInformations[i]->GetPeakValue( &peakValHolder );

			peakValMax = max( peakValMax, peakValHolder );
		}

		hr = sac->m_pEndpointVolume->GetMasterVolumeLevel( &currentDb );

		currentScale = pow( 10.f, currentDb / 20.f );
		peakValMaxScaled = peakValMax * currentScale;

		sac->m_sampledMaf.addValue( peakValMaxScaled );
		sac->m_averageBuf.addValue( sac->m_sampledMaf.getAverage() );

		dScaled = sac->m_targetVolume - peakValMaxScaled;
		dDb = ( 20.f / ( peakValMaxScaled * ln10 ) ) * dScaled;

		currentDb += dDb;

		currentDb = min( currentDb, sac->m_maxDB );
		currentDb = max( currentDb, sac->m_minDB );

		// Audio limits: min: -95.25, max: 0, step: 0.03125, #steps: 3048

		if ( sac->m_adjust == ADJUST_MODE::ADJUST )
		{
			hr = sac->m_pEndpointVolume->SetMasterVolumeLevel( currentDb, NULL );
		}

		Sleep( 1 );
	}

	return;
}

void SA_start( SA_context* sa )
{
	sa->m_loop = std::async( std::launch::async, &SA_loop, sa );
}

void SA_setAdjustMode( SA_context* sa, ADJUST_MODE mode )
{
	sa->m_adjust = mode;
}

void SA_stop( SA_context* sa )
{
	sa->m_running = false;
    delete sa;
}
