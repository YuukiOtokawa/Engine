#pragma once
#include "Script.h"
#include "GameManager.h"
#include "Audio.h"

#include "kiss_fft.h"

class MeshField;
class TitlePool :
    public Script
{
    const int FFT_SIZE = 1024;

    kiss_fft_cfg cfg;
    std::vector<float> window;
    std::vector<kiss_fft_cpx> timeData;
    std::vector<kiss_fft_cpx> freqData;

    std::vector<float> magnitudes = std::vector<float>(FFT_SIZE / 2, 0.0f);

    Audio* audioComp = nullptr;
    MeshField* meshFieldComp = nullptr;

    float logScaleValue = 10.0f;
public:
    TitlePool() {
        REGISTER_PROPERTY(logScaleValue, "Log Scale Value");
    }
    void Start() override;
    void Update() override;
    void DrawCustomGUI() override;

    const char* GetScriptName() const override;

};

