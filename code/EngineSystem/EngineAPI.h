#pragma once


// TODO: [otokawa] 後でOTOKAWA_APIの名づけ変えようね
#ifdef OTOKAWA_ENGINE_EXPORTS
#define OTOKAWA_API __declspec(dllexport)
#else
#define OTOKAWA_API __declspec(dllimport)
#endif
