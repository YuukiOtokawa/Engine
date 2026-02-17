#pragma once

#include <d3d11.h>

class ComputeBuffer {
    ID3D11Buffer* m_pSRVStructureBuffer = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;

    ID3D11Buffer* m_pUAVStructureBuffer = nullptr;
    ID3D11UnorderedAccessView* m_pUAV = nullptr;
public:

    ComputeBuffer();
    ~ComputeBuffer();

    void SetBuffer();

    void CreateSRVStructureBuffer(UINT size, UINT count);
    void CreateUAVStructureBuffer(UINT size, UINT count);

    ID3D11Buffer* GetSRVStructureBuffer() {
        return m_pSRVStructureBuffer;
    }
    ID3D11Buffer* GetUAVStructureBuffer() {
        return m_pUAVStructureBuffer;
    }
    ID3D11ShaderResourceView* GetSRV() {
        return m_pSRV;
    }
    ID3D11UnorderedAccessView* GetUAV() {
        return m_pUAV;
    }
};
