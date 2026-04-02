#include "Fractal.h"

#include "ScriptFactory.h"

#include "MeshField.h"

#undef min
#undef max

REGISTER_SCRIPT(Fractal)

void Fractal::Start()
{
	auto field = gameobject->GetComponent<MeshField>();
	if (!field) return;

    srand(timeGetTime());

	heightMap.resize(size * size, 0.0f);

    Generate();

    if (field->GetVertexCount() == size * size) {
        field->SetHeight(heightMap);
    }
}

void Fractal::Generate(float roughness) {
    int stepSize = size - 1;
    float scale = roughness;

    while (stepSize > 1)
    {
        int halfStep = stepSize / 2;

        // Diamond ステップ
        for (int y = halfStep; y < size; y += stepSize)
        {
            for (int x = halfStep; x < size; x += stepSize)
            {
                DiamondStep(x, y, halfStep, scale);
            }
        }

        // Square ステップ
        for (int y = 0; y < size; y += halfStep)
        {
            for (int x = (y + halfStep) % stepSize; x < size; x += stepSize)
            {
                SquareStep(x, y, halfStep, scale);
            }
        }

        stepSize /= 2;
        scale *= 0.5f;  // ラフネスを減少させる
    }

    // 正規化（0.0 ~ 1.0の範囲に収める）
    Normalize();
}

void Fractal::DiamondStep(int x, int y, int stepSize, float scale)
{
    float average = (
        heightMap[(y - stepSize) * size + (x - stepSize)] +
        heightMap[(y - stepSize) * size + (x + stepSize)] +
        heightMap[(y + stepSize) * size + (x - stepSize)] +
        heightMap[(y + stepSize) * size + (x + stepSize)]
        ) * 0.25f;

    heightMap[y * size + x] = average + GetRandomValue() * scale;
}

void Fractal::SquareStep(int x, int y, int stepSize, float scale)
{
    float sum = 0.0f;
    int count = 0;

    // 上
    if (y - stepSize >= 0)
    {
        sum += heightMap[(y - stepSize) * size + x];
        count++;
    }
    // 右
    if (x + stepSize < size)
    {
        sum += heightMap[y * size + (x + stepSize)];
        count++;
    }
    // 下
    if (y + stepSize < size)
    {
        sum += heightMap[(y + stepSize) * size + x];
        count++;
    }
    // 左
    if (x - stepSize >= 0)
    {
        sum += heightMap[y * size + (x - stepSize)];
        count++;
    }

    heightMap[y * size + x] = (sum / count) + GetRandomValue() * scale;
}

void Fractal::Normalize()
{
    float minHeight = heightMap[0];
    float maxHeight = heightMap[0];

    // 最小値と最大値を探す
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            minHeight = std::min(minHeight, heightMap[y * size + x]);
            maxHeight = std::max(maxHeight, heightMap[y * size + x]);
        }
    }

    // 0.0 ~ 1.0 の範囲に正規化
    float range = maxHeight - minHeight;
    if (range > 0.0f)
    {
        for (int y = 0; y < size; y++)
        {
            for (int x = 0; x < size; x++)
            {
                heightMap[y * size + x] = (heightMap[y * size + x] - minHeight) / range;
            }
        }
    }
}

float Fractal::GetRandomValue() {
    return (float)rand() / RAND_MAX * 2.0f - 1.0f;
}

void Fractal::Update()
{
}

REGISTERCLASS(Fractal);
