// EAX.H -- DirectSound Environmental Audio Extensions

#ifndef EAX_H_INCLUDED
#define EAX_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

 
// EAX (listener) reverb property set {4a4e6fc1-c341-11d1-b73a-444553540000}
DEFINE_GUID(DSPROPSETID_EAX_ReverbProperties, 
    0x4a4e6fc1,
    0xc341,
    0x11d1,
    0xb7, 0x3a, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

 
typedef enum 
{
    DSPROPERTY_EAX_ALL,                // all reverb properties
    DSPROPERTY_EAX_ENVIRONMENT,        // standard environment no.
    DSPROPERTY_EAX_VOLUME,             // loudness of the reverb
    DSPROPERTY_EAX_DECAYTIME,          // how long the reverb lasts
    DSPROPERTY_EAX_DAMPING             // the high frequencies decay faster
} DSPROPERTY_EAX_REVERBPROPERTY;


// use this structure for get/set all properties...
typedef struct 
{
    unsigned long environment;          // 0 to EAX_ENVIRONMENT_COUNT-1
    float fVolume;                      // 0 to 1
    float fDecayTime_sec;               // seconds, 0.1 to 100
    float fDamping;                     // 0 to 1
} EAX_REVERBPROPERTIES;

 
enum
{
    EAX_ENVIRONMENT_GENERIC,                // factory default
    EAX_ENVIRONMENT_PADDEDCELL,
    EAX_ENVIRONMENT_ROOM,              // standard environments
    EAX_ENVIRONMENT_BATHROOM,
    EAX_ENVIRONMENT_LIVINGROOM,
    EAX_ENVIRONMENT_STONEROOM,
    EAX_ENVIRONMENT_AUDITORIUM,
    EAX_ENVIRONMENT_CONCERTHALL,
    EAX_ENVIRONMENT_CAVE,
    EAX_ENVIRONMENT_ARENA,
    EAX_ENVIRONMENT_HANGAR,
    EAX_ENVIRONMENT_CARPETEDHALLWAY,
    EAX_ENVIRONMENT_HALLWAY,
    EAX_ENVIRONMENT_STONECORRIDOR,
    EAX_ENVIRONMENT_ALLEY,
    EAX_ENVIRONMENT_FOREST,
    EAX_ENVIRONMENT_CITY,
    EAX_ENVIRONMENT_MOUNTAINS,
    EAX_ENVIRONMENT_QUARRY,
    EAX_ENVIRONMENT_PLAIN,
    EAX_ENVIRONMENT_PARKINGLOT,
    EAX_ENVIRONMENT_SEWERPIPE,
    EAX_ENVIRONMENT_UNDERWATER,
    EAX_ENVIRONMENT_DRUGGED,
    EAX_ENVIRONMENT_DIZZY,
    EAX_ENVIRONMENT_PSYCHOTIC,

    EAX_ENVIRONMENT_COUNT           // total number of environments
};


 
// EAX buffer reverb property set {4a4e6fc0-c341-11d1-b73a-444553540000}
DEFINE_GUID(DSPROPSETID_EAXBUFFER_ReverbProperties, 
    0x4a4e6fc0,
    0xc341,
    0x11d1,
    0xb7, 0x3a, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

 
typedef enum 
{
    DSPROPERTY_EAXBUFFER_ALL,           // all reverb buffer properties
    DSPROPERTY_EAXBUFFER_REVERBMIX      // the wet source amount
} DSPROPERTY_EAXBUFFER_REVERBPROPERTY;

 
#define EAX_REVERBMIX_USEDISTANCE -1.0F // out of normal range
                                         // signifies the reverb engine should
                                         // calculate it's own reverb mix value
                                         // based on distance

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
