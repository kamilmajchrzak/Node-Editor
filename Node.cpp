//
// Created by Kamil Majchrzak on 01/12/2025.
//

#include "Node.h"

Node::Node(int id_, const std::string &name, int numInputs, int numOutputs): position{
                                                                                    (float)(200 + id_ * 150),
                                                                                    200
                                                                                    },
                                                                            label(name),
                                                                            defaultFontSize(16),
                                                                            content_color(BLUE),
                                                                            id(id_)
{
    this->textWidth = MeasureText(name.c_str(), defaultFontSize);
    // Losuj: które porty są aktywne
    for (int i = 0; i < numInputs; ++i) {
        Port p;
        p.active = (rand() % 3 == 0); // ~33% szansy
        inputs.push_back(p);
    }
    for (int i = 0; i < numOutputs; ++i) {
        Port p;
        p.active = (rand() % 3 == 0);
        outputs.push_back(p);
    }

    content_color = {(unsigned char)(rand() % 255), (unsigned char)(rand() % 255), (unsigned char)(rand() % 255), 255};

}

void Node::UpdateHitboxes() {
    const int portSpacing = 20;
    const int portSize = 10;
    const int nodeWidth = 140 + std::max(defaultFontSize, 200) * 20;
    const int nodeHeight = 60 + std::max(inputs.size(), outputs.size()) * portSpacing;


    // Input ports (lewa strona)
    for (size_t i = 0; i < inputs.size(); ++i) {
        float y = position.y + 40 + i * portSpacing;
        inputs[i].hitbox = { position.x - portSize, y - portSize/2.0f, (float)portSize, (float)portSize };
    }
    // Output ports (prawa strona)
    for (size_t i = 0; i < outputs.size(); ++i) {
        float y = position.y + 40 + i * portSpacing;
        outputs[i].hitbox = { position.x + nodeWidth, y - portSize/2.0f, (float)portSize, (float)portSize };
    }
}

void Node::Draw() const {
    const int nodeWidth = 140;
    const int nodeHeight = 60 + std::max(inputs.size(), outputs.size()) * 20;
    DrawRectangleRounded({ position.x, position.y, (float)nodeWidth, (float)nodeHeight }, 0.2f, 10, color);
    DrawRectangleRoundedLines({ position.x, position.y, (float)nodeWidth, (float)nodeHeight }, 0.2f, 20, WHITE);

    DrawText(label.c_str(), (int)position.x + 10, (int)position.y + 15, defaultFontSize, WHITE); // Replace with std::function, so to draw whatever digit/text/vector/bitmap/
    //DrawCircle(this->position.x + (nodeWidth/2), this->position.y + (nodeHeight/2), 40, content_color);

    const int portSpacing = 20;
    const int portSize = 10;

    // Rysuj inputy (lewa strona)
    for (size_t i = 0; i < inputs.size(); ++i) {
        float y = position.y + 40 + i * portSpacing;
        Color c = inputs[i].active ? GREEN : DARKGRAY;
        DrawCircle(position.x, y, portSize/2.0f, c);
    }

    // Rysuj outputy (prawa strona)
    for (size_t i = 0; i < outputs.size(); ++i) {
        float y = position.y + 40 + i * portSpacing;
        Color c = outputs[i].active ? GREEN : DARKGRAY;
        DrawCircle(position.x + nodeWidth, y, portSize/2.0f, c);
    }
}

// Pomocnicza: pozycja portu (output)
Vector2 GetOutputPos(const Node& node, size_t portIdx) {
    const int nodeWidth = 140;
    const int portSpacing = 20;
    float y = node.position.y + 40 + portIdx * portSpacing;
    return { node.position.x + nodeWidth, y };
}

// Pomocnicza: pozycja portu (input)
Vector2 GetInputPos(const Node& node, size_t portIdx) {
    const int portSpacing = 20;
    float y = node.position.y + 40 + portIdx * portSpacing;
    return { node.position.x, y };
}