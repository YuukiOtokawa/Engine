#include "ComputeBuffer.h"

#include "RenderCore.h"

ComputeBuffer::ComputeBuffer() {
}

ComputeBuffer::~ComputeBuffer() {
    m_pSRV->Release();
    m_pUAV->Release();
    m_pSRVStructureBuffer->Release();
    m_pUAVStructureBuffer->Release();
}

void ComputeBuffer::CreateSRVStructureBuffer(UINT size, UINT count) {
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bd.ByteWidth = size * count;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = size;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    RenderCore::GetInstance()->GetDevice()->CreateBuffer(&bd,NULL,&m_pSRVStructureBuffer);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.Format=DXGI_FORMAT_UNKNOWN;
    srvDesc.BufferEx.NumElements = count;
    RenderCore::GetInstance()->GetDevice()->CreateShaderResourceView(m_pSRVStructureBuffer,&srvDesc,&m_pSRV);
}

void ComputeBuffer::CreateUAVStructureBuffer(UINT size, UINT count) {
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
    bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bd.ByteWidth = size * count;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = size;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    RenderCore::GetInstance()->GetDevice()->CreateBuffer(&bd,NULL,&m_pUAVStructureBuffer);
    
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc,sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Format=DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.NumElements = count;
    RenderCore::GetInstance()->GetDevice()->CreateUnorderedAccessView(m_pUAVStructureBuffer,&uavDesc,&m_pUAV);
}
