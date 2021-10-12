#include "ui.h"
#include <iostream>

using namespace ui;

int UIState::x = 0;
int UIState::y = 0;
bool UIState::pressed[3] = {false, false, false};
mouse::Button UIState::mousePressed = mouse::Button::NO;
bool UIState::is(Mod mod) {
    if (mod == Mod::NO) {
        return
            !pressed[0] &&
            !pressed[1] &&
            !pressed[2];
    }
    if (mod == Mod::ANY) {
        return
            pressed[0] ||
            pressed[1] ||
            pressed[2];
    }
    if (mod == Mod::SHIFT) return pressed[0];
    if (mod == Mod::CTRL)  return pressed[1];
    if (mod == Mod::ALT)   return pressed[2];
    
    return false;
}
void UIState::print() {
    std::wcout
        << pressed[0] << " "
        << pressed[1] << " "
        << pressed[2] << " "
        << "x=" << x << " "
        << "y=" << y << " "
        << "mousePressed=" << (int)mousePressed
        << std::endl;
}

namespace ui {
    glm::vec2 getCursor() {
        return {UIState::x, UIState::y};
    }
}

namespace ui {
    namespace keyboard {
        KeyEvent::KeyEvent(int key, int action, int mod) {
            this->key = (keyboard::Key)key;
            this->action = (keyboard::Action)action;

            if (action == keyboard::Action::PRESS) {
                if (key == GLFW_KEY_LEFT_SHIFT)   UIState::pressed[0] = true;
                if (key == GLFW_KEY_LEFT_CONTROL) UIState::pressed[1] = true;
                if (key == GLFW_KEY_LEFT_ALT)     UIState::pressed[2] = true;
            }
            else if (action == keyboard::Action::REPEAT) {
                if (mod == GLFW_MOD_SHIFT)   UIState::pressed[0] = true;
                if (mod == GLFW_MOD_CONTROL) UIState::pressed[1] = true;
                if (mod == GLFW_MOD_ALT)     UIState::pressed[2] = true;
            }
            else if (action == keyboard::Action::RELEASE) {
                if (key == GLFW_KEY_LEFT_SHIFT)   UIState::pressed[0] = false;
                if (key == GLFW_KEY_LEFT_CONTROL) UIState::pressed[1] = false;
                if (key == GLFW_KEY_LEFT_ALT)     UIState::pressed[2] = false;
            }
        }
        bool KeyEvent::is(keyboard::Key key) {
            return is(Mod::NO, key);
        }
        bool KeyEvent::is(Mod mod, keyboard::Key key) {
            bool result =
                    this->key == key && (
                            this->action == keyboard::Action::PRESS ||
                            this->action == keyboard::Action::REPEAT) &&
                    UIState::is(mod);
            return result;
        }
        bool KeyEvent::is(Mod mod, keyboard::Action action) {
            using namespace keyboard;
            bool pressed = UIState::is(mod);
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
        MouseEvent::MouseEvent(int x, int y) {
            button = UIState::mousePressed;
            action = mouse::Action::MOVE;
            dx = UIState::x - x;
            dy = UIState::y - y;
            UIState::x = x;
            UIState::y = y;
        }
        MouseEvent::MouseEvent(int button, int action, int mod) {

            if (button == GLFW_MOUSE_BUTTON_LEFT)        this->button = mouse::Button::LEFT;
            else if (button == GLFW_MOUSE_BUTTON_MIDDLE) this->button = mouse::Button::MIDDLE;
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)  this->button = mouse::Button::RIGHT;
            else                                         this->button = mouse::Button::NO;


            if (action == GLFW_PRESS)        this->action = mouse::Action::PRESS;
            else if (action == GLFW_RELEASE) this->action = mouse::Action::RELEASE;
            else                             this->action = mouse::Action::NONE;

            this->dx = 0;
            this->dy = 0;

            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                UIState::mousePressed = this->button;
            } else {
                UIState::mousePressed = mouse::Button::NO;
            }
        }
        bool MouseEvent::is(mouse::Action action) {
            using namespace mouse;

            if (action == Action::MOVE) {
                return is(Action::MOVE, Mod::NO, Button::NO);
            }

            throw std::runtime_error("Not implemented");
        }
        bool MouseEvent::is(mouse::Action action, mouse::Button button) const {
            return is(action, Mod::NO, button);
        }
        bool MouseEvent::is(mouse::Action action, Mod mod) const {
            return is(action, mod, mouse::Button::NO);
        }
        bool MouseEvent::is(mouse::Action action, Mod mod, mouse::Button button) const {
            using namespace mouse;
            bool buttonCheck =
                    (button == Button::ANY && this->button != Button::NO) ||
                    (button == this->button);
            bool modCheck = UIState::is(mod);

            return this->action == action &&
                buttonCheck &&
                modCheck;
        }
        glm::vec2 MouseEvent::getDelta() const {
            return glm::vec2(dx, dy);
        }
        glm::vec2 MouseEvent::getCursor() {
            return {UIState::x, UIState::y};
        }
    }
}
