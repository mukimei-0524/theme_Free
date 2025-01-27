#include "SkinnedMesh.h"
#include "misc.h"

#include <memory>
#include <sstream>
#include <functional>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "Shader.h"
#include "Texture.h"

using namespace DirectX;

// コンストラクタ
SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{
	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("cereal");

	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials, animation_clips);

		createComObjects(device, fbx_filename);
	}
}

// オブジェクト生成
void SkinnedMesh::createComObjects(ID3D11Device* device, const char* fbx_filename)
{
	for (mesh& mesh : meshes)
	{
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = mesh.vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,
			mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = mesh.indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,
			mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

#if 0
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT}
	};

	// TODO	改良の余地あり
	ShaderManager::instance()->createVsFromCso(device, ".\\Shader\\phong_shader_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\phong_shader_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	//ShaderManager::instance()->createVsFromCso(device, ".\\Shader\\SkinnedMeshVS.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	//ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\SkinnedMeshPS.cso", pixel_shader.ReleaseAndGetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
	{
		for (size_t texture_index = 0; texture_index < 2; ++texture_index)
		{
			if (iterator->second.texture_filenames[texture_index].size() > 0)
			{
				std::filesystem::path path(fbx_filename);
				path.replace_filename(iterator->second.texture_filenames[texture_index]);
				D3D11_TEXTURE2D_DESC texture2d_desc;
				TextureManager::instance()->loadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);
			}
			else
			{
				TextureManager::instance()->makeDummyTexture(device, iterator->second.shader_resource_views[texture_index].GetAddressOf(), 0xFFFFFFFF, 16);
			}
		}
	}
}

// 描画処理
void SkinnedMesh::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
	const animation::keyframe* keyframe)
{
	for (const mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediate_context->IASetInputLayout(input_layout.Get());
		immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

		constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));
			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (int bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);
			}
		}
		else
		{
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.bone_transforms[bone_index] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
			}
		}

		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };
			XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
			immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
			immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());
			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
}