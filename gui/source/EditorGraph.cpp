#include "proto/EditorGraph.h"

#include "ImNodesEz.h"
#include "proto/TypeWidgets.h"

using namespace spvgentwo;

proto::EditorGraph::EditorGraph(spvgentwo::IAllocator* _pAlloc, spvgentwo::ILogger* _pLogger, const char* _pName) :
    m_pAlloc(_pAlloc),
    m_module(_pAlloc, spv::Version, _pLogger),
    m_pName(_pName),
    m_newFunctionPopup(_pAlloc),
    m_nodes(_pAlloc)
{
}

proto::EditorGraph::~EditorGraph()
{
    if (m_pCanvas != nullptr)
    {
        m_pAlloc->destruct(m_pCanvas);
        m_pCanvas = nullptr;
    }
}

void proto::EditorGraph::update()
{
    createCanvas();

    m_module.reset();
    // configure capabilities and extensions
    m_module.addCapability(spv::Capability::Shader);
    Function& main = m_module.addEntryPoint<void>(spv::ExecutionModel::Vertex, "main");
    m_pBB = &main.front();

    if (ImGui::Begin(m_pName, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        ImNodes::BeginCanvas(m_pCanvas);

        updateContextMenu();

        for (auto& node : m_nodes)
        {
            node.data().get().update();
        }

        ImNodes::EndCanvas();
    }
    ImGui::End();

    m_textView.update(m_module);
}

void proto::EditorGraph::clear()
{
    //m_module.clear();
}

void proto::EditorGraph::save()
{
}

void proto::EditorGraph::createCanvas()
{
    if (m_pCanvas == nullptr)
    {
        m_pCanvas = m_pAlloc->construct<ImNodes::CanvasState>();
    }
}

void proto::EditorGraph::updateNodes()
{
    ImVec2 pos = ImGui::GetCursorPos();

    //updateNodeFromContainer(m_module.getEntryPoints(), EditorNode::Type::EntryPoint,
    //    [&](EntryPoint& f) // add node func
    //{
    //    EditorNode& newNode = m_nodes.emplace_back(m_pAlloc, "EntryPoint", ImVec2{}, &f);
    //    newNode.update();
    //    ImNodes::AutoPositionNode(&newNode);
    //}, [&](EditorNode& n) // Remove node func
    //{

    //});

    //EditorNode& bbNode = m_nodes.emplace(&bb, EditorNode(m_pAlloc, "BBNode", pos, &bb)).first->second;
    //ImNodes::AutoPositionNode(&bbNode);

    //if (f.size() == 1u)
    //{
    //    EditorNode::connect({ &bbNode, "FuncEntry", getNode(&f), "EntryBlock" });
    //}
}

void proto::EditorGraph::updateContextMenu()
{
    ImVec2 pos = ImGui::GetCursorPos();

    if (ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered() && !ImGui::IsMouseDragging(1))
    {
        bool anySelected = false;

        for (const auto& node : m_nodes)
        {
            anySelected |= node.data();
        }

        if (anySelected == false)
        {
            ImGui::FocusWindow(ImGui::GetCurrentWindow());
            ImGui::OpenPopup("NodesContextMenu");
        }
    }

    if (ImGui::BeginPopup("NodesContextMenu"))
    {
        spvgentwo::ExprGraph<OpNodeExpr>::NodeType* pNode = nullptr;

        if (ImGui::MenuItem("Constant"))
        {
            pNode = m_nodes.emplace(OpNodeExpr{ pos, OpNodeType::Const });
        }

        if (ImGui::MenuItem("InputVar"))
        {
            pNode = m_nodes.emplace(OpNodeExpr{ pos, OpNodeType::InVar });
        }

        if (ImGui::MenuItem("OutputVar"))
        {
            pNode = m_nodes.emplace(OpNodeExpr{ pos, OpNodeType::OutVar });
        }

        if (ImGui::MenuItem("Add"))
        {
            pNode = m_nodes.emplace(OpNodeExpr{ pos, OpNodeType::Add });
        }

        if (pNode != nullptr)
        {
            auto& editorNode = pNode->data().get();
            editorNode.setBasicBlock(m_pBB);
            editorNode.setParent(&m_nodes, pNode);
            editorNode.update();
            ImNodes::AutoPositionNode(&editorNode);
        }

        if (ImGui::IsAnyMouseDown() && !ImGui::IsWindowHovered())
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}