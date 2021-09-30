#pragma once
#include <GLFW/glfw3.h>
#include "src/pch.h"


namespace ui {
    enum Mod {
        NO = 0,
        SHIFT = 1,
        CTRL = 2,
        ALT = 3,
        ANY = 4,
    };

    namespace keyboard {
        enum Key {
            ESC = GLFW_KEY_ESCAPE,
            SPACE = GLFW_KEY_SPACE,
            RIGHT = GLFW_KEY_RIGHT,
            LEFT = GLFW_KEY_LEFT,

            W = GLFW_KEY_W,
            S = GLFW_KEY_S,
            A = GLFW_KEY_A,
            D = GLFW_KEY_D,
            C = GLFW_KEY_C,
            Z = GLFW_KEY_Z,
            X = GLFW_KEY_X,
            P = GLFW_KEY_P,
            Q = GLFW_KEY_Q,
            E = GLFW_KEY_E,
            R = GLFW_KEY_R,
            T = GLFW_KEY_T,
            M = GLFW_KEY_M,
            N = GLFW_KEY_N,
            H = GLFW_KEY_H,

            KEY_1 = GLFW_KEY_1,
            KEY_2 = GLFW_KEY_2,
            KEY_3 = GLFW_KEY_3,
            KEY_4 = GLFW_KEY_4,
            KEY_5 = GLFW_KEY_5,
            KEY_6 = GLFW_KEY_6,
            KEY_7 = GLFW_KEY_7,
            KEY_0 = GLFW_KEY_0,
            KP_ADD = GLFW_KEY_KP_ADD,
            KP_SUB = GLFW_KEY_KP_SUBTRACT,
            TAB = GLFW_KEY_TAB,
            //KEY_SHIFT = GLFW_KEY_LEFT_SHIFT,
            //KEY_CONTROL = GLFW_KEY_LEFT_CONTROL,
            //KEY_ALT = GLFW_KEY_LEFT_ALT,

            DEL = GLFW_KEY_DELETE
        };
        enum Action {
            PRESS = GLFW_PRESS,
            REPEAT = GLFW_REPEAT,
            RELEASE = GLFW_RELEASE
        };
        class KeyEvent {
            keyboard::Key key;
            keyboard::Action action;
        public:
            KeyEvent(int key, int action, int mod);
            bool is(keyboard::Key key);
            bool is(Mod mod, keyboard::Key key);
            bool is(Mod mod, keyboard::Action action);
        };
    }

    namespace mouse {
        enum Button {
            NO = 0,
            LEFT = 1,
            RIGHT = 2,
            MIDDLE = 3,
            ANY = 4
        };
        enum Action {
            NONE,
            PRESS,
            RELEASE,
            MOVE
        };
        class MouseEvent {
        public:
            mouse::Button button;
            mouse::Action action;
            int dx, dy;
            MouseEvent(int x, int y);
            MouseEvent(int button, int action, int mod);
            bool is(mouse::Action action);
            bool is(mouse::Action action, mouse::Button button) const;
            bool is(mouse::Action action, Mod mod) const;
            bool is(mouse::Action action, Mod mod, mouse::Button button) const;
            glm::vec2 getDelta() const;
            glm::vec2 getCursor();
        };
    }

    struct UIState {
        static int x;
        static int y;
        static bool pressed[3];
        static mouse::Button mousePressed;
        static bool is(Mod mod);
        static void print();
    };
    glm::vec2 getCursor();
}
