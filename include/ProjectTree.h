#pragma once
#include <string>
#include <vector>

struct ProjectNode {
    std::string name;
    bool isFolder;
    bool startExpanded;
    std::vector<ProjectNode> children;
};

void DrawFolderTree(const ProjectNode &node);