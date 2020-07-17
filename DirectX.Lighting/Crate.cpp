#include "Crate.h"
#include <DirectXMath.h>
#include "GeometryGenerator.h"
#include <DirectXColors.h>
#include "DDSTextureLoader.h"
#include "ShaderData.h"

Crate::Crate(Renderer* renderer) : m_Renderer(renderer)
{
}

bool Crate::Load()
{
    Geometry::CreateBox(1.0f, 1.0f, 1.0f, &m_MeshData);

    // Create vertex buffer
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = (UINT)(sizeof(Vertex) * m_MeshData.vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vInitData = {};
    vInitData.pSysMem = &m_MeshData.vertices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &m_VertexBuffer));

    // Create index buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = (UINT)(sizeof(UINT) * m_MeshData.indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iInitData = {};
    iInitData.pSysMem = &m_MeshData.indices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&ibd, &iInitData, &m_IndexBuffer));
    
    // Load texture
    ID3D11Resource* resource = nullptr;
    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), L"Textures\\crate_diffuse.dds", &resource, &m_DiffuseTexture));
    resource->Release();

	return true;
}

void Crate::Render(Camera* camera)
{
    // Bind the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Bind the index buffer
    m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set topology
    m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set world buffer
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    ConstantBuffer cb;
    cb.mWorld = DirectX::XMMatrixTranspose(world);
    cb.mView = DirectX::XMMatrixTranspose(camera->GetView());
    cb.mProjection = DirectX::XMMatrixTranspose(camera->GetProjection());
    cb.mWorldInverse = DirectX::XMMatrixInverse(nullptr, world);

    ID3D11Buffer* worldConstantBuffer = m_Renderer->GetWorldConstantBuffer();
    m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &worldConstantBuffer);
    m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &worldConstantBuffer);
    m_Renderer->GetDeviceContext()->UpdateSubresource(worldConstantBuffer, 0, nullptr, &cb, 0, 0);

    m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_DiffuseTexture);

    // Set Light buffer
    DirectionalLight directionalLight = {};
    directionalLight.mCameraPos = camera->GetPosition();
    directionalLight.mDiffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    directionalLight.mAmbient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f);
    directionalLight.mSpecular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 32.0f);
    directionalLight.mDirection = DirectX::XMFLOAT4(0.0f, 0.5f, -0.5f, 1.0f);

    PointLight pointLight = {};
    pointLight.mCameraPos = camera->GetPosition();
    pointLight.mDiffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pointLight.mAmbient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f);
    pointLight.mSpecular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 32.0f);
    pointLight.mLightPos = DirectX::XMFLOAT4(0.0f, 0.5f, 2.0f, 1.0f);

    LightBuffer lightBuffer;
    lightBuffer.mDirectionalLight = directionalLight;
    lightBuffer.mPointLight = pointLight;

    ID3D11Buffer* lightConsantBuffer = m_Renderer->GetLightConstantBuffer();
    m_Renderer->GetDeviceContext()->PSSetConstantBuffers(1, 1, &lightConsantBuffer);
    m_Renderer->GetDeviceContext()->UpdateSubresource(lightConsantBuffer, 0, nullptr, &lightBuffer, 0, 0);

    // Render geometry
    m_Renderer->GetDeviceContext()->DrawIndexed((UINT)m_MeshData.indices.size(), 0, 0);
}