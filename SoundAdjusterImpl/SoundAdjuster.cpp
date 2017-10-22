#include "SoundAdjuster.h"

#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h> // IAudioSessionManager2
#include <endpointvolume.h> // IAudioMeterInformation

#include "Buffers.h"

#include <vector>
#include <thread>

#define EXIT_ON_ERROR(hres) if (FAILED(hres)) {goto Exit;}
#define SAFE_RELEASE(var) if (var != NULL) { var->Release(); var = NULL;}

struct SA_context
{
    ~SA_context()
    {
        SAFE_RELEASE( m_pEndpointVolume );
        SAFE_RELEASE( m_pSessionEnumerator );
        SAFE_RELEASE( m_pSessionNotification );
    }

    // Audio level caps defined by hardware
    float m_maxDB; // -4.6875f
    float m_minDB; // -95.2188f

    // Maximum cap to as m_maxDB is usually too loud
    float m_maxPeak; // 0.30f

    // Current target volume level
    float m_targetVolume; // 0.025f;

    // Control whether sound is adjusted
    bool m_adjust;

    // Control whether sound is processed
    bool m_running;

    // Storage for volume level data
    MovingAvgFilter<double> m_sampledMaf;
    CircularBuffer<double> m_averageBuf;

    // Windows Audio Session API classes for the actual sampling and adjusting
    IAudioEndpointVolume* m_pEndpointVolume;
    IAudioSessionEnumerator* m_pSessionEnumerator;
    IAudioSessionNotification* m_pSessionNotification;
    std::vector<IAudioMeterInformation*> m_pMeterInformations;
};

class SA_audioSessionNotification : public IAudioSessionNotification
{
public:

    SA_audioSessionNotification( SA_context* sac )
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

SA_context* SA_initialize()
{
    SA_context* sac = new SA_context;

    sac->m_maxDB = -4.6875f;
    sac->m_minDB = -95.2188f;
    sac->m_maxPeak = 0.3f;
    sac->m_targetVolume = 0.025f;
    sac->m_adjust = false;
    sac->m_running = false;

    CoInitializeEx( NULL, COINIT_MULTITHREADED );

    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance( __uuidof( MMDeviceEnumerator ), NULL, CLSCTX_ALL, __uuidof( IMMDeviceEnumerator ), (void**)&pEnumerator );
    EXIT_ON_ERROR( hr );

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint( eRender, eMultimedia, &pDevice );
    EXIT_ON_ERROR( hr );

    hr = pDevice->Activate( __uuidof( IAudioEndpointVolume ), CLSCTX_ALL, NULL, (void**)&sac->m_pEndpointVolume );
    EXIT_ON_ERROR( hr );

    IAudioSessionManager2* pSessionManager = nullptr;
    hr = pDevice->Activate( __uuidof( IAudioSessionManager2 ), CLSCTX_ALL, NULL, (void**)&pSessionManager );
    EXIT_ON_ERROR( hr );
    
    sac->m_pSessionNotification = new SA_audioSessionNotification( sac );
    hr = pSessionManager->RegisterSessionNotification( sac->m_pSessionNotification );
    EXIT_ON_ERROR( hr );
    
    hr = pSessionManager->GetSessionEnumerator( &sac->m_pSessionEnumerator );
    EXIT_ON_ERROR( hr );
    
Exit:
    SAFE_RELEASE( pEnumerator );
    SAFE_RELEASE( pDevice );
    SAFE_RELEASE( sac->m_pEndpointVolume );
    SAFE_RELEASE( pSessionManager );
    SAFE_RELEASE( sac->m_pSessionNotification );
    SAFE_RELEASE( sac->m_pSessionEnumerator );

    return sac;
}

void SA_start( SA_context* sa )
{
    std::thread t( &SA_loop, sa );
    t.detach();
}

void SA_stop( SA_context* sa )
{
    delete sa;
}

void updateSessionMeters( SA_context* sac )
{
    HRESULT hr;
    IAudioSessionControl* iAudioSessionControl;
    IAudioMeterInformation* iAudioMeterInformation;
    
    int numSessions = 0;
    hr = sac->m_pSessionEnumerator->GetCount( &numSessions );
    if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }
    
    sac->m_pSessionNotification->m_iAudioMeterInformations.clear();
    
    for ( int i = 0; i < numSessions; i++ )
    {
        hr = sac->m_pSessionEnumerator->GetSession( i, &iAudioSessionControl );
        m_resources.push_back( iAudioSessionControl );
        if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }
        
        hr = iAudioSessionControl->QueryInterface( &iAudioMeterInformation );
        m_resources.push_back( iAudioMeterInformation );
        if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }
        
        m_iAudioMeterInformations.push_back( iAudioMeterInformation );
    }
    return 0;
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

    updateSessionMeters();

    while ( m_running )
    {
        peakValMax = -1.0;

        HRESULT hr;

        for ( int i = 0; i < (int)m_iAudioMeterInformations.size(); i++ )
        {
            hr = m_iAudioMeterInformations[i]->GetPeakValue( &peakValHolder );
            if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }

            peakValMax = max( peakValMax, peakValHolder );
        }

        hr = m_pEndpointVolume->GetMasterVolumeLevel( &currentDb );
        if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }

        currentScale = pow( 10.f, currentDb / 20.f );
        peakValMaxScaled = peakValMax * currentScale;

        m_sampledMaf->addValue( peakValMaxScaled );
        m_averageBuf->addValue( m_sampledMaf->getAverage() );

        dScaled = m_targetVolume - peakValMaxScaled;
        dDb = ( 20.f / ( peakValMaxScaled * ln10 ) ) * dScaled;

        currentDb += dDb;

        currentDb = min( currentDb, m_maxDB );
        currentDb = max( currentDb, m_minDB );

        // Audio limits: min: -95.25, max: 0, step: 0.03125, #steps: 3048
        if ( m_adjust )
        {
            //hr = m_pEndpointVolume->SetMasterVolumeLevel( currentDb, NULL );
        }

        if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }

        Sleep( 1 );
    }

    if ( FAILED( hr ) ) { return Exit( m_resources, hr ); }

    //return hr;

Exit:
    SAFE_RELEASE( sac->m_pEndpointVolume );
    SAFE_RELEASE( sac->m_pSessionNotification );
    SAFE_RELEASE( sac->m_pSessionEnumerator );

    return;
}

