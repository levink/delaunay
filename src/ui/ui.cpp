#include <iostream>
#include "ui.h"


using namespace ui;


bool UIState::is(KeyMod mod) {
    if (mod == KeyMod::NO) {
        return
            !keyPressed[0] &&
            !keyPressed[1] &&
            !keyPressed[2];
    }
    if (mod == KeyMod::ANY) {
        return
            keyPressed[0] ||
            keyPressed[1] ||
            keyPressed[2];
    }
    if (mod == KeyMod::SHIFT) return keyPressed[0];
    if (mod == KeyMod::CTRL)  return keyPressed[1];
    if (mod == KeyMod::ALT)   return keyPressed[2];
    
    return false;
}
void UIState::print() {
    std::wcout
        << keyPressed[0] << " "
        << keyPressed[1] << " "
        << keyPressed[2] << " "
        << "x=" << x << " "
        << "y=" << y << " "
        << "mousePressed=" << (int)this->mousePressed
        << std::endl;
}
glm::vec2 UIState::getCursor() {
    return glm::vec2(x, y);
}

namespace ui {
    namespace keyboard {
        KeyEvent::KeyEvent(int key, int action, int mod) {
            this->key = (keyboard::Key)key;
            this->action = (keyboard::Action)action;

            if (action == keyboard::Action::PRESS) {
                if (key == GLFW_KEY_LEFT_SHIFT)   ui_state.keyPressed[0] = true;
                if (key == GLFW_KEY_LEFT_CONTROL) ui_state.keyPressed[1] = true;
                if (key == GLFW_KEY_LEFT_ALT)     ui_state.keyPressed[2] = true;
            }
            else if (action == keyboard::Action::REPEAT) {
                if (mod == GLFW_MOD_SHIFT)   ui_state.keyPressed[0] = true;
                if (mod == GLFW_MOD_CONTROL) ui_state.keyPressed[1] = true;
                if (mod == GLFW_MOD_ALT)     ui_state.keyPressed[2] = true;
            }
            else if (action == keyboard::Action::RELEASE) {
                if (key == GLFW_KEY_LEFT_SHIFT)   ui_state.keyPressed[0] = false;
                if (key == GLFW_KEY_LEFT_CONTROL) ui_state.keyPressed[1] = false;
                if (key == GLFW_KEY_LEFT_ALT)     ui_state.keyPressed[2] = false;
            }
        }
        bool KeyEvent::is(keyboard::Key key) {
            return is(KeyMod::NO, key);
        }
        bool KeyEvent::is(KeyMod mod, keyboard::Key key) {
            bool result = 
                this->key == key && (
                this->action == keyboard::Action::PRESS ||
                this->action == keyboard::Action::REPEAT
            ) && ui_state.is(mod);
            return result;
        }
        bool KeyEvent::is(KeyMod mod, keyboard::Action action) {
            using namespace keyboard;
            bool pressed = ui_state.is(mod);
            if (action == Action::PRESS) {
                return pressed;
            }
            if (action == Action::RELEASE) {
                return !pressed;
            }
            return false;
        }
    }

    namespace mouse {
        MouseEvent move(int x, int y) {
            auto dx = ui_state.x - x;
            auto dy = ui_state.y - y;
            ui_state.x = x;
            ui_state.y = y;
            return MouseEvent{ ui_state.mousePressed, MouseAction::MOVE, dx, dy };
        }
        MouseEvent click(int button, int action, int mod) {

            auto mouseButton = MouseButton::NO;
            if (button == GLFW_MOUSE_BUTTON_LEFT)        mouseButton = MouseButton::LEFT;
            else if (button == GLFW_MOUSE_BUTTON_MIDDLE) mouseButton = MouseButton::MIDDLE;
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)  mouseButton = MouseButton::RIGHT;
            else                                         mouseButton = MouseButton::NO;

            auto mouseAction = MouseAction::NONE;
            if (action == GLFW_PRESS)        mouseAction = MouseAction::PRESS;
            else if (action == GLFW_RELEASE) mouseAction = MouseAction::RELEASE;
            else                             mouseAction = MouseAction::NONE;

            auto pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
            ui_state.mousePressed = pressed ? mouseButton : MouseButton::NO;

            return MouseEvent{ mouseButton, mouseAction, 0, 0 };
        }
        bool MouseEvent::is(MouseAction action) const {

            if (action == MouseAction::MOVE) {
                return is(MouseAction::MOVE, MouseButton::NO, KeyMod::NO);
            }

            throw std::runtime_error("Not implemented");
        }
        bool MouseEvent::is(MouseAction action, MouseButton button) const {
            return is(action, button, KeyMod::NO);
        }
        bool MouseEvent::is(MouseAction action, MouseButton button, KeyMod mod) const {
            if (action != this->action) {
                return false;
            }
            
            bool buttonCheck = 
                (button == this->button) ||
                (button == MouseButton::ANY && this->button != MouseButton::NO);
            if (!buttonCheck) {
                return false;
            }

            bool modCheck = ui_state.is(mod);
            return modCheck;
        }
        glm::vec2 MouseEvent::getDelta() const {
            return glm::vec2(dx, dy);
        }
        glm::vec2 MouseEvent::getCursor() {
            return ui_state.getCursor();
        }
    }
}
