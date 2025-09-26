#include "Score.h"

#include "Editor.h"

void Score::Start()
{
	m_pScoreSprite100p = Editor::GetInstance()->GetObject("ScoreSprite100p")->GetComponent<SpriteMesh>();
	m_pScoreSprite10p = Editor::GetInstance()->GetObject("ScoreSprite10p")->GetComponent<SpriteMesh>();
	m_pScoreSprite1p = Editor::GetInstance()->GetObject("ScoreSprite1p")->GetComponent<SpriteMesh>();
}

void Score::OnUpdate()
{

}

void Score::AddScore(int score)
{
	m_Score += score; // スコアを加算
	// ここでスコアの更新処理を行う

	m_pScoreSprite100p->SetSpriteByIndex((m_Score / 100) % 5); // 100の位
	m_pScoreSprite10p->SetSpriteByIndex((m_Score / 10) % 10); // 10の位
	m_pScoreSprite1p->SetSpriteByIndex(m_Score % 10); // 1の位
}
