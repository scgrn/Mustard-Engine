/**

zlib License

(C) 2020 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#ifndef AB_INTERPOLATOR_H
#define AB_INTERPOLATOR_H

namespace AB {

template<class T>
class Interpolator {
    public:
        Interpolator();
        ~Interpolator();

        void set(T startValue, T endValue, int steps);
        void update();
        T getValue(float const& delta = 1.0f);
        bool isFinished() { return finished; }

    protected:
        T value, oldValue, endValue, step;
        bool finished;

};

template<class T>
Interpolator<T>::Interpolator() {
    finished = true;
}

template<class T>
Interpolator<T>::~Interpolator() {
    finished = true;
}

template<class T>
void Interpolator<T>::set(T startValue, T endValue, int steps) {
    value = startValue;
    oldValue = value;
    this->endValue = endValue;

    if (steps <= 0) {
        value = endValue;
        oldValue = value;
        finished = true;
    } else {
        step = (endValue - startValue) / steps;
        finished = false;
    }
}

template<class T>
void Interpolator<T>::update() {
    oldValue = value;

    if (!finished) {
        value += step;
        if (step > 0.0f) {
            if (value >= endValue) {
                value = endValue;
                finished = true;
            }
        }
        if (step < 0.0f) {
            if (value <= endValue) {
                value = endValue;
                finished = true;
            }
        }
    }
}

template<class T>
T Interpolator<T>::getValue(float const& delta) {
    T blendValue = (value * delta) + (oldValue * (1.0f - delta));

    return blendValue;
}

}

#endif

