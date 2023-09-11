#include "gui.hpp"
#include "shader.hpp"
#include <glm/vec3.hpp>
#include <vector>
#include <fstream>
//remove this include after done debugging, as it is for debug only
//#include <iostream>
#include <GLFW/glfw3.h>
#include "render.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "global.hpp"

std::unordered_map<std::string, std::function<void()>> buttonActions;

GUIContainer::GUIContainer() {
}

void GUIContainer::render() {
    for (BaseWidget *widget: widgets) {
        widget->render();
    }
}

BaseWidget *GUIContainer::getWidgetByPos(int x, int y) {
    for (BaseWidget *widget: widgets) {
        glm::vec4 vec = widget->getOccpiedArea();
        int widgetMinX = (int) vec.x, widgetMinY = (int) vec.y, widgetMaxX = (int) vec.z, widgetMaxY = (int) vec.w;
        if (widgetMinX <= x && x <= widgetMaxX && widgetMinY <= y && y <= widgetMaxY) {
            return widget;
            break;
        }
    }
    return new BaseWidget();
}

GUIContainer::~GUIContainer() {
}

Window::Window() {
    pos = glm::vec2();
}

void Window::render() {
    for (BaseWidget *widget: widgets) {
        widget->pos = this->pos + widget->localPos;
        widget->render();
    }
}

Window::~Window() {
}

BaseWidget::BaseWidget() {
    pos = glm::vec2();
    localPos = glm::vec2();
}

//Stub, every GUI widget should render themselves
void BaseWidget::render() {
}

glm::vec4 BaseWidget::getOccpiedArea() {
    return glm::vec4();
}

//Destructor method automatically deletes all buffers in buffer array
BaseWidget::~BaseWidget() {
    /*for (size_t i = 0; i < buffers.size(); i++)
    {
        GLuint buffer = buffers[i];
        glDeleteBuffers(1, &buffer);
    }*/
    for (auto &it: buffers) {
        GLuint buffer = it.second;
        glDeleteBuffers(1, &buffer);
    }
    buffers.clear();
}

//Creates a button at pos with size and text
ButtonWidget::ButtonWidget(glm::vec2 pos, glm::vec2 size, std::string text, std::string action) : text(text),
                                                                                                  action(action) {
    this->pos = pos;
    this->size = size;
    center = glm::vec2((pos.x + (size.x / 2)), (pos.y + (size.y / 2) - 7.5f));
    std::vector<glm::vec2> vertices = {
            glm::vec2(pos.x, pos.y + size.y),
            glm::vec2(pos.x, pos.y),
            glm::vec2(pos.x + size.x, pos.y + size.y),
            glm::vec2(pos.x, pos.y),
            glm::vec2(pos.x + size.x, pos.y),
            glm::vec2(pos.x + size.x, pos.y + size.y),
    };
    color = glm::vec4(0, 0.75f, 1, 1);
    GLuint verticesID;
    glGenBuffers(1, &verticesID);
    glBindBuffer(GL_ARRAY_BUFFER, verticesID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    buffers["vertices"] = verticesID;
    //std::cout << "Button: color.x " << color.x << ", color.y " << color.y << ", color.z " << color.z << ", color.w " << color.w << "\n";
}

//Renders button with it's defined position, size and text
void ButtonWidget::render() {
    glDepthMask(GL_FALSE);
    glUseProgram(shaderMap["GUI"]);
    GLuint colorID = glGetUniformLocation(shaderMap["GUI"], "colorIN");
    glUniform4f(colorID, color.x, color.y, color.z, color.w);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers["vertices"]);
    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void *) 0            // array buffer offset
    );
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);

    printText2D(text.c_str(), (int) (center.x - ((text.size() * 7) / 2)), (int) center.y, 15, false, true);
    glDepthMask(GL_TRUE);
}

ButtonWidget *ButtonWidget::parse(std::string str) {
    std::string text = "";
    std::string action = "";
    glm::vec2 pos = glm::vec2();
    glm::vec2 size = glm::vec2();
    bool inQuotes = false;
    bool inBrackets = false;
    std::string tmp;
    int argID = 0;
    int subArgID = 0;
    int charID = 0;
    for (char &ch: str) {
        charID++;
        if (ch == ' ' && !inQuotes) {
            tmp.clear();
            argID++;
            charID = 0;
            continue;
        }
        /*if (!inQuotes && ch != ' ') {
            tmp += ch;
        }*/
        if (argID == 1) {
            if (charID == 1 && ch != '[') {
                throw "Syntax error! First argument must be position in square brackets";
            }
            if (ch == ']') {
                std::vector<std::string> o;
                split(tmp, o, ',');
                pos = glm::vec2(std::stof(o[0]), std::stof(o[1]));
                inQuotes = false;
                continue;
            } else if (ch == '[') {
                inQuotes = true;
                continue;
            }
            tmp += ch;
        } else if (argID == 2) {
            if (charID == 1 && ch != '[') {
                throw "Syntax error! Second argument must be size in square brackets";
            }
            if (ch == ']') {
                std::vector<std::string> o;
                split(tmp, o, ',');
                size = glm::vec2(std::stof(o[0]), std::stof(o[1]));
                inQuotes = false;
                continue;
            }
            if (ch == '[') {
                inQuotes = true;
                continue;
            }
            tmp += ch;
        } else if (argID == 3) {
            if (charID == 1 && ch == '"') {
                inQuotes = true;
                continue;
            } else if (charID != 1 && ch == '"') {
                text = tmp;
                inQuotes = false;
                continue;
            } else if (charID == 1) {
                throw "Syntax error! Third argument must be string";
            }
            tmp += ch;
        } else if (argID == 4) {
            if (charID == 1 && ch == '"') {
                inQuotes = true;
                continue;
            } else if (charID != 1 && ch == '"') {
                action = tmp;
                inQuotes = false;
                continue;
            } else if (charID == 1) {
                throw "Syntax error! Fourth argument must be string";
            }
            tmp += ch;
        }
    }
    return new ButtonWidget(pos, size, text, action);
}

glm::vec4 ButtonWidget::getOccpiedArea() {
    int minX = (int) pos.x;
    int minY = (int) pos.y;
    int maxX = (int) pos.x + (int) size.x;
    int maxY = (int) pos.y + (int) size.y;
    return glm::vec4(minX, minY, maxX, maxY);
}

ButtonWidget::~ButtonWidget() {
}

TextWidget::TextWidget(glm::vec2 pos, int size, std::string text) {
    this->pos = pos;
    this->size = size;
    this->text = text;
}

void TextWidget::render() {
    printText2D(text.c_str(), (int) pos.x, (int) pos.y, size);
}

TextWidget *TextWidget::parse(std::string str) {
    std::string text = "";
    glm::vec2 pos = glm::vec2();
    int size = 0;
    bool inQuotes = false;
    bool inBrackets = false;
    std::string tmp;
    int argID = 0;
    int subArgID = 0;
    int charID = 0;
    for (char &ch: str) {
        charID++;
        if (argID == 2 && ch == ' ') {
            size = std::stoi(tmp);
        }
        if (ch == ' ' && !inQuotes) {
            tmp.clear();
            argID++;
            charID = 0;
            continue;
        }
        /*if (!inQuotes && ch != ' ') {
            tmp += ch;
        }*/
        if (argID == 1) {
            if (charID == 1 && ch != '[') {
                throw "Syntax error! First argument must be position in square brackets";
            }
            if (ch == ']') {
                std::vector<std::string> o;
                split(tmp, o, ',');
                pos = glm::vec2(std::stof(o[0]), std::stof(o[1]));
                inQuotes = false;
                continue;
            }
            if (ch == '[') {
                inQuotes = true;
                continue;
            }
            tmp += ch;
        } else if (argID == 2) {
            tmp += ch;
        } else if (argID == 3) {
            if (charID == 1 && ch == '"') {
                inQuotes = true;
                continue;
            } else if (charID != 1 && ch == '"') {
                text = tmp;
                continue;
            } else if (charID == 1) {
                throw "Syntax error! Third argument must be string";
            }
            tmp += ch;
        }
    }
    return new TextWidget(pos, size, text);
}

TextWidget::~TextWidget() {
}

enum class GUIType {
    Panel,
    Window
};

GUIContainer loadRes(char *resourceName) {
    printf("[Resource Loader] Loading %s...\n", resourceName);
    std::ifstream input("resource/" + std::string(resourceName) + ".res");
    int lineID = 1;
    GUIType type;
    std::vector<BaseWidget *> widgets;
    for (std::string line; getline(input, line);) {
        if (line.rfind("#", 0) == 0) continue;
        //Menu initialization, always must be included, or engine will not know what kind of GUI it is!
        if (lineID == 1 && stringContains(line, "Menu")) {
            type = GUIType::Panel;
        } else if (lineID == 1 && stringContains(line, "Window")) {
            printf("[Resource Loader] Warning: Window rendering is not finished yet!\n");
            type = GUIType::Window;
        } else if (lineID == 1) {
            printf("[Resource Loader] ERROR: File must begin with Menu or Window\n");
            return GUIContainer();
        }

        //Widget parsing, add custom widgets here
        if (stringContains(line, "Button")) {
            try {
                ButtonWidget &button = *ButtonWidget::parse(line);
                widgets.push_back(&button);
            }
            catch (std::string e) {
                printf("[Resource Loader] Error %s:%s %s", resourceName, std::to_string(lineID).c_str(), e.c_str());
            }
        } else if (stringContains(line, "Text")) {
            try {
                TextWidget &text = *TextWidget::parse(line);
                widgets.push_back(&text);
            }
            catch (std::string e) {
                printf("[Resource Loader] Error %s:%s %s", resourceName, std::to_string(lineID).c_str(), e.c_str());
            }
        }

        lineID++;
    }
    GUIContainer container = GUIContainer();
    container.widgets = widgets;
    printf("[Resource Loader] Done loading %s\n", resourceName);
    return container;
}

void drawInfo(std::string line1, std::string line2, bool clear) {
    if (clear) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    //printText2D(line1.c_str(), 290, 290, 20);
    printText2D(line1.c_str(), (int) (400 - line1.size() * 5), 290, 20);
    //printText2D(line2.c_str(), 250, 250, 20);
    printText2D(line2.c_str(), (int) (400 - line2.size() * 5), 250, 20);
    if (clear) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}