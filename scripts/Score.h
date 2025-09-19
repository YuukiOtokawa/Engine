#pragma once
#include "MonoBehavior.h"

#include "SpriteMesh.h"

class Score :
    public MonoBehaviour
{
private:
    int m_Score = 0; // スコアの初期値

    SpriteMesh* m_pScoreSprite1p = nullptr; // スコア表示用のスプライト
    SpriteMesh* m_pScoreSprite10p = nullptr; // スコア表示用のスプライト
    SpriteMesh* m_pScoreSprite100p = nullptr; // スコア表示用のスプライト
public:
    void Start() override;
    void Update() override;
    void AddScore(int score);
};

