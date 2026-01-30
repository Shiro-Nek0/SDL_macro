#include "ProjectTree.h"
#include "imgui.h"

void DrawFolderTree(const ProjectNode &node) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    if (node.isFolder) {
        if (node.startExpanded) flags |= ImGuiTreeNodeFlags_DefaultOpen;
        if (ImGui::TreeNodeEx(node.name.c_str(), flags)) {
            for (const auto &child : node.children) DrawFolderTree(child);
            ImGui::TreePop();
        }
    } else {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        std::string displayName = "[F] " + node.name;
        ImGui::TreeNodeEx(displayName.c_str(), flags);
    }
}