#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

namespace Visage {

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

