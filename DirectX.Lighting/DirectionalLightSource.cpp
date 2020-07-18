#include "DirectionalLightSource.h"
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "DDSTextureLoader.h"
#include "ShaderData.h"
#include <cmath>

DirectionalLightSource::DirectionalLightSource(Renderer* renderer) : m_Renderer(renderer)
{
}

bool DirectionalLightSource::Load()
{
    Vertex vertices[6];

    // Arrow shaft
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = 0;

    vertices[1].x = 0.8;
    vertices[1].y = 0.5;
    vertices[1].z = -0.5;

    // Arrow head
    auto lightDir = DirectX::XMFLOAT3(0.8f, 0.5f, -0.5f);
    auto lightDirVec = DirectX::XMLoadFloat3(&lightDir);

    lightDirVec = DirectX::XMVectorScale(lightDirVec, 0.8f);

    DirectX::XMFLOAT3 lightDirSub;
    DirectX::XMStoreFloat3(&lightDirSub, lightDirVec);

    vertices[2] = vertices[1];
    vertices[3].x = lightDirSub.x + 0.1f;
    vertices[3].y = lightDirSub.y;
    vertices[3].z = lightDirSub.z;

    vertices[4] = vertices[1];
    vertices[5].x = lightDirSub.x - 0.1f;
    vertices[5].y = lightDirSub.y;
    vertices[5].z = lightDirSub.z;

    // Create vertex buffer 
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = (UINT)(sizeof(Vertex) * 6);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vInitData = {};
    vInitData.pSysMem = &vertices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &m_VertexBuffer));

    // Load texture
    ID3D11Resource* resource = nullptr;
    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), L"Textures\\light_diffuse.dds", &resource, &m_DiffuseTexture));
    resource->Release();

    return true;
}

void DirectionalLightSource::Render(Camera* camera)
{
    // Bind the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Bind the index buffer
    m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set topology
    m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // Set world buffer
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

    world *= DirectX::XMMatrixTranslation(0.0f, 2.0f, 0.0f);

    ConstantBuffer cb;
    cb.mWorld = DirectX::XMMatrixTranspose(world);
    cb.mView = DirectX::XMMatrixTranspose(camera->GetView());
    cb.mProjection = DirectX::XMMatrixTranspose(camera->GetProjection());
    cb.mWorldInverse = DirectX::XMMatrixInverse(nullptr, world);

    Material material;
    material.mDiffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    material.mAmbient = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    material.mSpecular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    cb.mMaterial = material;

    ID3D11Buffer* worldConstantBuffer = m_Renderer->GetWorldConstantBuffer();
    m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &worldConstantBuffer);
    m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &worldConstantBuffer);
    m_Renderer->GetDeviceContext()->UpdateSubresource(worldConstantBuffer, 0, nullptr, &cb, 0, 0);

    m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_DiffuseTexture);

    // Render geometry
    m_Renderer->GetDeviceContext()->Draw(6, 0);
}