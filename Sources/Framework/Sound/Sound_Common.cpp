#include	"Sound_Common.h"

#pragma comment(lib, "xaudio2.lib")

// マクロ定義
#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

namespace {
	IXAudio2               *g_pXAudio2 = NULL;
	IXAudio2MasteringVoice *g_pMasteringVoice = NULL;
	IXAudio2SourceVoice    *g_pSourceVoice[NSound::SOUND_LABEL_MAX];

	WAVEFORMATEXTENSIBLE	wfx[NSound::SOUND_LABEL_MAX];			// WAVフォーマット
	XAUDIO2_BUFFER			buffer[NSound::SOUND_LABEL_MAX];
	BYTE					*pDataBuffer[NSound::SOUND_LABEL_MAX];

	bool					g_playedInFrame[NSound::SOUND_LABEL_MAX];	// そのフレームで再生されたフラグ

	HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
	HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);
}

namespace NSound {

	HRESULT InitSound()
	{
		HRESULT      hr;

		HANDLE               hFile;
		DWORD                dwChunkSize;
		DWORD                dwChunkPosition;
		DWORD                filetype;

		for (int i = 0; i < ELabel::SOUND_LABEL_MAX; ++i) {
			pDataBuffer[i] = nullptr;
			g_playedInFrame[i] = false;
		}

		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		/**** Create XAudio2 ****/
		hr = XAudio2Create(&g_pXAudio2, 0/* , XAUDIO2_DEFAULT_PROCESSOR*/);
		if (FAILED(hr)) {
			CoUninitialize();
			return -1;
		}

		/**** Create Mastering Voice ****/
		hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice/*, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL*/);
		if (FAILED(hr)) {
			if (g_pXAudio2)	g_pXAudio2->Release();
			CoUninitialize();
			return -1;
		}


		/**** Initalize Sound ****/
		for (int i = 0; i < NSound::SOUND_LABEL_MAX; i++) {
			memset(&wfx[i], 0, sizeof(WAVEFORMATEXTENSIBLE));
			memset(&buffer[i], 0, sizeof(XAUDIO2_BUFFER));

			NSound::ELabel	label_ = (NSound::ELabel)i;
			hFile = CreateFile(NSound::GetAssetInfo(label_).GetFilePath(), GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				return HRESULT_FROM_WIN32(GetLastError());
			}
			if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
				return HRESULT_FROM_WIN32(GetLastError());
			}

			//check the file type, should be fourccWAVE or 'XWMA'
			FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
			ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
			if (filetype != fourccWAVE)		return S_FALSE;

			FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
			ReadChunkData(hFile, &wfx[i], dwChunkSize, dwChunkPosition);

			//fill out the audio data buffer with the contents of the fourccDATA chunk
			FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
			pDataBuffer[i] = new BYTE[dwChunkSize];
			ReadChunkData(hFile, pDataBuffer[i], dwChunkSize, dwChunkPosition);

			CloseHandle(hFile);

			// 	サブミットボイスで利用するサブミットバッファの設定
			buffer[i].AudioBytes = dwChunkSize;
			buffer[i].pAudioData = pDataBuffer[i];
			buffer[i].Flags = XAUDIO2_END_OF_STREAM;
			if (NSound::GetAssetInfo(label_).GetIsLoop())
				buffer[i].LoopCount = XAUDIO2_LOOP_INFINITE;
			else
				buffer[i].LoopCount = 0;

			g_pXAudio2->CreateSourceVoice(&g_pSourceVoice[i], &(wfx[i].Format));
		}

		return hr;
	}

	void DestroySound()
	{
		for (int i = 0; i < NSound::SOUND_LABEL_MAX; i++) {
			if (g_pSourceVoice[i]) {
				g_pSourceVoice[i]->Stop(0);
				g_pSourceVoice[i]->FlushSourceBuffers();
				g_pSourceVoice[i]->DestroyVoice();			// オーディオグラフからソースボイスを削除
			}
		}

		g_pMasteringVoice->DestroyVoice();

		if (g_pXAudio2) g_pXAudio2->Release();

		CoUninitialize();
	}

	void UpdateSound(void)
	{
		for (int i = 0; i < ELabel::SOUND_LABEL_MAX; ++i) {
			g_playedInFrame[i] = false;
		}
	}

	void Play(NSound::ELabel _label)
	{
		// 同フレームで再生済みの場合は無視する
		if (g_playedInFrame[_label]) { return; }

		// ソースボイス作成
		g_pXAudio2->CreateSourceVoice(&(g_pSourceVoice[_label]), &(wfx[_label].Format));
		g_pSourceVoice[_label]->SubmitSourceBuffer(&(buffer[_label]));	// ボイスキューに新しいオーディオバッファーを追加

		// 再生
		g_pSourceVoice[_label]->Start(0);

		g_playedInFrame[_label] = true;
	}

	void Stop(NSound::ELabel _label)
	{
		if (g_pSourceVoice[_label] == NULL) return;

		// 明示的にサウンドを停止した場合は、再生可能状態にしておく
		g_playedInFrame[_label] = false;

		XAUDIO2_VOICE_STATE xa2state;
		g_pSourceVoice[_label]->GetState(&xa2state);
		if (xa2state.BuffersQueued) {
			g_pSourceVoice[_label]->Stop(0);
		}
	}

	void Pause(NSound::ELabel _label)
	{
		assert(false && "未実装");
	}

}

namespace {

	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
	{
		HRESULT hr = S_OK;
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
			return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwChunkType;
		DWORD dwChunkDataSize;
		DWORD dwRIFFDataSize = 0;
		DWORD dwFileType;
		DWORD bytesRead = 0;
		DWORD dwOffset = 0;
		while (hr == S_OK) {
			DWORD dwRead;
			if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			switch (dwChunkType) {
				case fourccRIFF:
					dwRIFFDataSize = dwChunkDataSize;
					dwChunkDataSize = 4;
					if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
						hr = HRESULT_FROM_WIN32(GetLastError());
					break;
				default:
					if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
						return HRESULT_FROM_WIN32(GetLastError());
			}
			dwOffset += sizeof(DWORD) * 2;
			if (dwChunkType == fourcc) {
				dwChunkSize = dwChunkDataSize;
				dwChunkDataPosition = dwOffset;
				return S_OK;
			}
			dwOffset += dwChunkDataSize;
			if (bytesRead >= dwRIFFDataSize) return S_FALSE;
		}
		return S_OK;
	}

	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
	{
		HRESULT hr = S_OK;
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
			return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwRead;
		if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

}