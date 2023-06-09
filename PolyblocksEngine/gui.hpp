#ifndef GUI_HPP
#define GUI_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <functional>

extern std::unordered_map<std::string, std::function<void()>> buttonActions;

class BaseWidget {
public:
    glm::vec2 pos;
    glm::vec2 localPos;
    std::unordered_map<std::string, GLuint> buffers;

    BaseWidget();

    virtual void render();

    virtual glm::vec4 getOccpiedArea();

    ~BaseWidget();

private:

};

class GUIContainer {
public:
    std::vector<BaseWidget *> widgets;

    GUIContainer();

    virtual void render();

    BaseWidget *getWidgetByPos(int x, int y);

    ~GUIContainer();

private:

};

class Window : public GUIContainer {
public:
    glm::vec2 pos;

    Window();

    void render();

    ~Window();

private:

};


class ButtonWidget : public BaseWidget {
public:
    std::string text;
    glm::vec2 center;
    glm::vec4 color;
    glm::vec2 size;
    //std::function<void()> callback;
    std::string action;

    ButtonWidget(glm::vec2 pos, glm::vec2 size, std::string text, std::string action);

    void render();

    static ButtonWidget *parse(std::string str);

    glm::vec4 getOccpiedArea();

    ~ButtonWidget();

private:

};

class TextWidget : public BaseWidget {
public:
    std::string text;
    int size;

    TextWidget(glm::vec2 pos, int size, std::string text);

    void render();

    static TextWidget *parse(std::string str);

    ~TextWidget();

private:

};


GUIContainer loadRes(char *resourceName);

void drawInfo(std::string line1, std::string line2, bool clear = true);

void executeAction(std::string action);


#endif // !GUI_HPP
