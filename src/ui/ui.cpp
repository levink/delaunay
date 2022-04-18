#include "ui.h"
#include <iostream>

using namespace ui;

int UIState::x = 0;
int UIState::y = 0;
bool UIState::modPressed[3] = {false, false, false};
mouse::MouseButton UIState::mousePressed = mouse::MouseButton::NO;

bool UIState::is(KeyMod mod) {
    if (mod == KeyMod::NO) {
        return
            !modPressed[0] &&
            !modPressed[1] &&
            !modPressed[2];
    }
    if (mod == KeyMod::ANY) {
        return
            modPressed[0] ||
            modPressed[1] ||
            modPressed[2];
    }
    if (mod == KeyMod::SHIFT) return modPressed[0];
    if (mod == KeyMod::CTRL)  return modPressed[1];
    if (mod == KeyMod::ALT)   return modPressed[2];
    
    return false;
}
void UIState::print() {
    std::wcout
        << modPressed[0] << " "
        << modPressed[1] << " "
        << modPressed[2] << " "
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
                if (key == GLFW_KEY_LEFT_SHIFT)   UIState::modPressed[0] = true;
                if (key == GLFW_KEY_LEFT_CONTROL) UIState::modPressed[1] = true;
                if (key == GLFW_KEY_LEFT_ALT)     UIState::modPressed[2] = true;
            }
            else if (action == keyboard::Action::REPEAT) {
                if (mod == GLFW_MOD_SHIFT)   UIState::modPressed[0] = true;
                if (mod == GLFW_MOD_CONTROL) UIState::modPressed[1] = true;
                if (mod == GLFW_MOD_ALT)     UIState::modPressed[2] = true;
            }
            else if (action == keyboard::Action::RELEASE) {
                if (key == GLFW_KEY_LEFT_SHIFT)   UIState::modPressed[0] = false;
                if (key == GLFW_KEY_LEFT_CONTROL) UIState::modPressed[1] = false;
                if (key == GLFW_KEY_LEFT_ALT)     UIState::modPressed[2] = false;
            }
        }
        bool KeyEvent::is(keyboard::Key key) {
            return is(KeyMod::NO, key);
        }
        bool KeyEvent::is(KeyMod mod, keyboard::Key key) {
            bool result =
                    this->key == key && (
                            this->action == keyboard::Action::PRESS ||
                            this->action == keyboard::Action::REPEAT) &&
                    UIState::is(mod);
            return result;
        }
        bool KeyEvent::is(KeyMod mod, keyboard::Action action) {
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
            action = MouseAction::MOVE;
            dx = UIState::x - x;
            dy = UIState::y - y;
            UIState::x = x;
            UIState::y = y;
        }
        MouseEvent::MouseEvent(int button, int action, int mod) {

            if (button == GLFW_MOUSE_BUTTON_LEFT)        this->button = MouseButton::LEFT;
            else if (button == GLFW_MOUSE_BUTTON_MIDDLE) this->button = MouseButton::MIDDLE;
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)  this->button = MouseButton::RIGHT;
            else                                         this->button = MouseButton::NO;


            if (action == GLFW_PRESS)        this->action = MouseAction::PRESS;
            else if (action == GLFW_RELEASE) this->action = MouseAction::RELEASE;
            else                             this->action = MouseAction::NONE;

            this->dx = 0;
            this->dy = 0;

            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                UIState::mousePressed = this->button;
            } else {
                UIState::mousePressed = MouseButton::NO;
            }
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

            bool modCheck = UIState::is(mod);
            return modCheck;
        }
        
        glm::vec2 MouseEvent::getDelta() const {
            return glm::vec2(dx, dy);
        }
        glm::vec2 MouseEvent::getCursor() {
            return {UIState::x, UIState::y};
        }
    }
}
