#include "LogNode.h"

#include "NodePin.h"

#include "EngineConsole.h"

LogNode::LogNode() : Node("Log")
{
	AddInput("Message", PinType::String);
}

void LogNode::Update()
{
	// 入力ピンからメッセージを取得してログに出力する
	if (m_Inputs.size() > 0) {
		NodePin* messagePin = m_Inputs[0];
		// ここで実際にはNodeManagerを通じてリンクされたノードから値を取得する必要がある
		// 仮に直接文字列を取得するロジックを実装する場合は以下のようになる
		// ただし、実際の実装ではNodeManager経由で値を取得することが望ましい
		// 仮の実装例:
		std::string message = "Sample log message"; // ここにリンクされたノードからの値を取得するロジックを入れる
		//EngineConsole::Log(message.c_str());
	}
}

void LogNode::DrawNodeUI()
{
}
