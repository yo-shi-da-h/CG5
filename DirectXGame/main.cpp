#include <Windows.h>
#include "KamataEngine.h"
#include "cassert"
#include "d3dcompiler.h"

using namespace KamataEngine;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"LE3D_17_ヨシダ_ハルト_CG5");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

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
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;//1つのRTVに書き込む　*二つ同時に書き込むこともできる
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトロポジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;//マルチサンプリングをしない
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//準備は整ったので、PSOを生成する
	ID3D12PipelineState* graphicsPipeLineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipeLineState)
	);
	assert(SUCCEEDED(hr));

	//VertexResourceの生成　---------------------------
	//頂点リソース用のヒープの生成
	D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//CPUから書き込むヒープ
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc = {};
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;//バッファ
	vertexResourceDesc.Width = sizeof(Vector4) * 3;//頂点数
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを生成する
	ID3D12Resource* vertexResource = nullptr;
	hr = dxCommon->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,//ヒープの設定
		D3D12_HEAP_FLAG_NONE,//ヒープフラグ
		&vertexResourceDesc,//リソースの設定
		D3D12_RESOURCE_STATE_GENERIC_READ,//リソースの状態
		nullptr,
		IID_PPV_ARGS(&vertexResource)
	);
	assert(SUCCEEDED(hr));
	//VertexBufferViewを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	//リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	//1つの頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(Vector4);
	//頂点リソースにデータを書き込む　------------------
	Vector4* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f };//上
	vertexData[1] = { 0.0f, 0.5f, 0.0f, 1.0f };//左
	vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f };//右
	//頂点リソースの書き込みが終わったら、マップを解除する
	vertexResource->Unmap(0, nullptr);

	while (true)
	{
		if (KamataEngine::Update()) {
			break;
		}
	
		//描画開始
		dxCommon->PreDraw();
		//コマンドを積む
		commandList->SetGraphicsRootSignature(rootSignature);//ルートシグネチャの設定
		commandList->SetPipelineState(graphicsPipeLineState);//PSOの設定
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVの設定
		//トポロジの設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//頂点数、インデックス数、インデックスの開始位置、インデックスのオフセット
		commandList->DrawInstanced(3, 1, 0, 0);
		//描画終了
		dxCommon->PostDraw();
		
	
	}
	//解放処理
		vertexResource->Release();
		graphicsPipeLineState->Release();
		vsBlob->Release();
		psBlob->Release();
		rootSignature->Release();
		signatureBlob->Release();
		//エラーオブジェクトの解放
		if (errorBlob) {
			errorBlob->Release();
		}
	KamataEngine::Finalize();
	return 0;
	
}
