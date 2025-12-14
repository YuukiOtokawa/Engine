#include "GameViewWindow.h"

#include "GUI.h"
#include "imgui.h"
#include "MainEngine.h"

void GameViewWindow::Render()
{
	if (!StartWindow("GameView", ImGuiWindowFlags_NoScrollbar)) return;


	auto renderCore = MainEngine::GetInstance()->GetRenderCore();

	auto tex = renderCore->GetGameViewTexture();

	// テクスチャサイズ
	ImVec2 texSize = ImVec2((float)tex->width, (float)tex->height);
	// ウィンドウサイズ
	ImVec2 avail = ImGui::GetWindowSize();

	// 拡縮倍率を計算
	float scaleX = texSize.x / avail.x;
	float scaleY = texSize.y / avail.y;
	float scale = 1.0f;

	if (scaleX < 1.0f) { // 横幅がウィンドウ内に収まっている場合
		if (scaleY >= 1.0f) { // 縦幅がウィンドウからはみ出している場合
			scale = 1.0f / scaleX;
		}
		else if (scaleY < 1.0f) { // 縦幅もウィンドウ内に収まっている場合
			scale = (std::max)(1.0f / scaleX, 1.0f / scaleY);
		}

	}
	else if (scaleY < 1.0f) { // 縦幅がウィンドウ内に収まっている場合
		if (scaleX >= 1.0f) { // 横幅がウィンドウからはみ出している場合
			scale = 1.0f / scaleY;
		}
	}
	else { // 横幅・縦幅共にウィンドウからはみ出している場合
		scale = 1.0f / (std::min)(scaleX, scaleY);
	}

	// 実際のテクスチャの描画サイズを計算
	ImVec2 drawSize = ImVec2(texSize.x * scale, texSize.y * scale);

	// テクスチャを中央に配置するためのオフセット計算
	ImVec2 offset = ImVec2(
		(avail.x - drawSize.x) * 0.5f,
		(avail.y - drawSize.y) * 0.5f
	);

	// カーソル位置を中央に移動
	ImGui::SetCursorPos(offset);

	ImGui::Image((ImTextureID)tex->shaderResourceView, drawSize, ImVec2(0, 0), ImVec2(1, 1));
	m_IsHovered = ImGui::IsItemHovered();

	ImGui::End();

}

