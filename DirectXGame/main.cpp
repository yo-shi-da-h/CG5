#include <Windows.h>
#include "KamataEngine.h"
#include "cassert"
#include "d3dcompiler.h"

using namespace KamataEngine;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"LE3D_17_ヨシダ_ハルト_CG5");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	

	while (true)
	{
		if (KamataEngine::Update()) {
			break;
		}
		
		int32_t w = dxCommon->GetBackBufferWidth();
		int32_t h = dxCommon->GetBackBufferHeight();
		DebugText::GetInstance()->ConsolePrintf(std::format("width: {}","height: {}", w, h).c_str());
		
		//DirectXCommonクラスが管理している、コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ID3D10Blob* signatureBlob = nullptr;
		ID3D10Blob* errorBlog = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlog);
		if (FAILED(hr)) {
			DebugText::GetInstance()->ConsolePrintf(reinterpret_cast<char*>(errorBlog->GetBufferPointer()));
			assert(false);
		}
		//バイナリを元に生成
		ID3D12RootSignature* rootSignature = nullptr;
		hr = dxCommon->GetDevice()->CreateRootSignature(
			0,
			signatureBlob->GetBufferPointer(),
			signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature)
		);
		assert(SUCCEEDED(hr));

		//InputLayoutの設定
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

		//BlendStateの設定
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		//ResterStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc = {};
		//裏面をカリングする
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//塗りつぶしモードをソリッドにする
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		ID3D10Blob* vsBlob = nullptr;//頂点シェーダーオブジェクト
		ID3D10Blob* psBlob = nullptr;//ピクセルシェーダーオブジェクト
		ID3D10Blob* errorBlob = nullptr;//エラーオブジェクト

		//頂点シェーダーの読み込みとコンパイル
		std::wstring vsPath = L"Resources/Shaders/TestVS.hlsl";
		hr = D3DCompileFromFile(vsPath.c_str(),//シェーダーファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"main", "vs_5_0", //エントリーポイントとシェーダーのモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用のフラグ
			0, &vsBlob, &errorBlob);

		if(FAILED(hr)){
			DebugText::GetInstance()->ConsolePrintf(std::system_category().message(hr).c_str());
			if(errorBlob) {
				DebugText::GetInstance()->ConsolePrintf(
					reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			}
		}
		//ピクセルシェーダーの読み込みとコンパイル
		std::wstring psPath = L"Resources/Shaders/TestPS.hlsl";
		hr = D3DCompileFromFile(psPath.c_str(),//シェーダーファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"main", "ps_5_0",//エントリーポイントとシェーダーのモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用のフラグ
			0, &psBlob, &errorBlob);
		if (FAILED(hr)) {
			DebugText::GetInstance()->ConsolePrintf(std::system_category().message(hr).c_str());
			if (errorBlob) {
				DebugText::GetInstance()->ConsolePrintf(
					reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			}
			assert(SUCCEEDED(hr));
		}
		//PSOの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
		graphicsPipelineStateDesc.pRootSignature = rootSignature;//ルートシグネチャ
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//入力レイアウト
		graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };//頂点シェーダー
		graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };//ピクセルシェーダー
		graphicsPipelineStateDesc.BlendState = blendDesc;//ブレンドステート
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//ラスタライザーステート
	

	KamataEngine::Finalize();
	return 0;
	
}
