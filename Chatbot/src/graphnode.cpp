#include "graphedge.h"
#include "graphnode.h"
#include "chatlogic.h"

GraphNode::GraphNode(int id)
{
    _id = id;
    _chatBot = nullptr;
}

GraphNode::~GraphNode()
{
    if (_chatBot != nullptr) {
        delete _chatBot;
        _chatBot = nullptr;
    }
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    _childEdges.push_back(std::move(edge));
}

void GraphNode::MoveChatbotHere(ChatBot chatbot)
{
    _chatBot = new ChatBot();
    *_chatBot = std::move(chatbot);
    _chatBot->GetChatLogicHandle()->SetChatbotHandle(_chatBot);
    _chatBot->SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(std::move(*_chatBot));
    _chatBot = nullptr; // invalidate pointer at source
}

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    return _childEdges[index].get();
}