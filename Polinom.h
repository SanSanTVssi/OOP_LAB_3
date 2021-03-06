//
// Created by AAI2002 on 27.11.2021.
//

#ifndef LBAB3_POLINOM_H
#define LBAB3_POLINOM_H

#include "intrerfaces.h"


template<bool, typename T1, typename T2>
struct is_cond {
    typedef T1 _type;
};

template<typename T1, typename T2>
struct is_cond<false, T1, T2> {
    typedef T2 _type;
};

template<typename T1, typename T2>
struct largest {
    typedef typename is_cond<(sizeof(T1)>sizeof(T2)), T1, T2>::_type _type;
};


template<class decimal = double>
class Polynomial : public IDictionary<decimal, int>{
private:
    int number;
    struct DATA{
        int key;
        decimal value;
    } * data;

    void resizeOnce();

public:
    Polynomial() : number(0), data(nullptr) {}

    explicit Polynomial(const decimal value);

    template<typename otherType>
    explicit Polynomial(const Polynomial<otherType>& other): Polynomial() {
        for (int i = 0; i < other.Length(); ++i) {
            Polynomial::push_back(static_cast<decimal>(other[i]), other.getKey(i));
        }
    }


    Polynomial(const Polynomial& other): Polynomial() {
       for (int i = 0; i < other.Length(); ++i) {
           Polynomial::push_back(other[i], other.getKey(i));
       }
    }

    [[maybe_unused]] Polynomial(const int num, decimal value);

    inline bool isEmpty() const { return data == nullptr; }

    void push_back(const decimal other, const int KEY = -1) override;

    int MaxKey();

    void resize(const int new_size) override;

    [[nodiscard]] int Length() const override{
        return number;
    }

    [[nodiscard]] inline int GetRealIndex(const int index) const{
        return (index >= 0) ? index : number + index;
    }

    decimal operator[](const int index) const override{
        return data[GetRealIndex(index)].value;;
    }

    [[nodiscard]] int getKey(const int index) const override {
        return data[GetRealIndex(index)].key;
    }

    ~Polynomial() override{
        delete[] data;
    }

    friend std::ostream& operator<<(std::ostream& out, const Polynomial<decimal> & other) {
        if (other.data == nullptr) {
            out << "nullptr";
        }
        else {
            for (int i = 0; i < other.Length() - 1; ++i) {
                out << other[i] << "X^" << other.getKey(i) << " + ";
            }
            out << other[-1] << "X^" << other.getKey(-1);
        }
        return out;
    }

    template<typename Type = double>
    double operator()(Type const x) const;

    template<class OtherType>
    Polynomial<OtherType> convert() {
        return Polynomial<OtherType>(*this);
    }

    [[nodiscard]] std::string type() const {
        return typeid(decimal).name();
    }

    void pop(const int degree) override{
        popByIndex(getIndexByKey(degree));
    }

    void popByIndex(const int index);

    template<int index1, int index2>
    Polynomial slice();

    int getDegree() { return MaxKey(); }

    int getRealDegree() { return Length(); }

    template<typename Type>
    Polynomial operator+(Type value);

    template<typename Type>
    Polynomial operator*(Type value);

    template<typename Type>
    Polynomial operator-(Type value);

    template<typename Type>
    Polynomial operator/(Type value);

    bool isExistKey(int degree);


    int getIndexByKey(int key);

    decimal getValueByKey(int degree);

    /**
     * Method for addition polynomial
     * @param polynomial1
     * @param polynomial2
     * @return result of addition
     */
    template<typename other_type>
    Polynomial<typename largest<decimal,other_type>::_type> operator+(Polynomial<other_type> polynomial1) {
        // Polynomial for result
        Polynomial<typename largest<decimal,other_type>::_type> result = Polynomial<typename largest<decimal,other_type>::_type>();
        // Chose max degree of two polynomials
        int maxKey = (polynomial1.MaxKey() > MaxKey()) ? polynomial1.MaxKey() : MaxKey();
        // We need add all coefficient from 0 (the least  coefficient) to Max degree
        // If value by degree is not exist then the result is 0
        for (int i = 0; i <= maxKey; ++i) {
            result.push_back(getValueByKey(i) + polynomial1.getValueByKey(i));
        }
        return result;
    }

    /**
    * Method for addition polynomial
    * @param polynomial1
    * @param polynomial2
    * @return result of addition
    */
    template<typename other_type>
    Polynomial<typename largest<decimal,other_type>::_type> operator-(Polynomial<other_type> polynomial1) {
        // Polynomial for result
        Polynomial<typename largest<decimal,other_type>::_type> result = Polynomial<typename largest<decimal,other_type>::_type>();
        // Chose max degree of two polynomials
        int maxKey = (polynomial1.MaxKey() > MaxKey()) ? polynomial1.MaxKey() : MaxKey();
        // We need add all coefficient from 0 (the least  coefficient) to Max degree
        // If value by degree is not exist then the result is 0
        for (int i = 0; i <= maxKey; ++i) {
            result.push_back(getValueByKey(i) - polynomial1.getValueByKey(i));
        }
        return result;
    }

    Polynomial& operator=(const Polynomial& other);

    Polynomial& operator+=(Polynomial &other);

    /**
     * Method for multiply Polynomials
     * @param polynomial1
     * @param polynomial2
     * @return multiplied polynomial
     */
    template<typename other_type>
    Polynomial<typename largest<decimal,other_type>::_type> operator*(Polynomial<other_type> polynomial1) {
        // Polynomial for result.
        Polynomial<typename largest<decimal,other_type>::_type> result = Polynomial<typename largest<decimal,other_type>::_type>();
        // Chose first item from Polynomial1 and multiplied his on the all items from polynomial2.
        // Loop for chose regular item from Polynomial1.
        for (int i = 0; i < polynomial1.Length(); ++i) {
            // Loop for chose regular item from Polynomial2.
            for (int j = 0; j < Length(); ++j) {
                // Polynomial1 for write into result of multiplied of item from Polynomial1[i] to item from polynomial2[j].
                Polynomial temp = Polynomial();
                // Multiply.
                temp.push_back(
                        polynomial1[i] * operator[](j),
                        polynomial1.getKey(i) + getKey(j)
                        );
                // Add temp into result.
                result += temp;
            }
        }
        return result;
    }

    template<typename other_type>
    Polynomial<typename largest<decimal,other_type>::_type> operator/(Polynomial<other_type>& polynomial1) {
        Polynomial<decimal> numerator = *this;
        Polynomial<other_type> denominator = polynomial1;
        Polynomial<typename largest<decimal,other_type>::_type> result = Polynomial();

        int maxNumDeg, maxDenDeg;
        while ((maxNumDeg = numerator.MaxKey()) >= (maxDenDeg = denominator.MaxKey())) {
            decimal val1 = numerator.getValueByKey(maxNumDeg);
            decimal val2 = denominator.getValueByKey(maxDenDeg);

            Polynomial tmp = Polynomial();
            tmp.push_back(val1/val2, maxNumDeg - maxDenDeg);
            Polynomial tmp2 = numerator - tmp * denominator;
            numerator = tmp2;
            numerator.pop(maxNumDeg);
            result += tmp;
        }
        return result;
    }
};

template<class decimal>
void Polynomial<decimal>::push_back(const decimal other, const int KEY) {
    if (isExistKey(KEY)) {
        data[getIndexByKey(KEY)].value += other;
    }
    else {
        int k = MaxKey();
        resizeOnce();
        data[number - 1].value = other;
        data[number - 1].key = k + 1;
        if (KEY != -1) {
            data[number - 1].key = KEY;
        }
    }
}

template<class decimal>
decimal Polynomial<decimal>::getValueByKey(int degree) {
    if (isExistKey(degree)) {
        return data[getIndexByKey(degree)].value;
    }
    return 0;
}

template<class decimal>
void Polynomial<decimal>::resizeOnce() {
    if (data == nullptr) {
        data = new DATA[++number];
        data[0].key = 0;
    } else {
        data = (DATA *) realloc(data, ++number * sizeof(DATA));
    }
}

template<class decimal>
int Polynomial<decimal>::MaxKey() {
    if (data == nullptr) return -1;
    int res = data[0].key;
    for (int i = 0; i < number; ++i) {
        if (res < data[i].key) {
            res = data[i].key;
        }
    }
    return res;
}

template<class decimal>
void Polynomial<decimal>::resize(const int new_size) {
    if (data == nullptr) {
        for (int i = 0; i < new_size; ++i) {
            push_back(0);
        }
    }
    if (new_size > number) {
        for (int i = number; i < new_size; ++i) {
            push_back(0.0);
        }
    } else if (new_size < number) {
        auto *newArr = new DATA[new_size];
        for (int i = 0; i < new_size; ++i) {
            newArr[i] = data[i];
        }
        delete[] data;
        data = newArr;
        number = new_size;
    }
}

template<class decimal>
template<typename Type>
double Polynomial<decimal>::operator()(const Type x) const {
    double result = 0;
    double y = x;
    for (int i = 0; i < number; ++i) {
        result += y * data[i].value;
        y *= x;
    }
    return result;
}

template<class decimal>
void Polynomial<decimal>::popByIndex(const int index) {
    for (long i = index; i < number; ++i) {
        data[i] = data[i + 1];
    }
    resize(number - 1);
    if (number == 0) {
        data = nullptr;
    }
}

template<class decimal>
template<typename Type>
Polynomial<decimal> Polynomial<decimal>::operator+(Type value) {
    Polynomial newPol = Polynomial();
    for (int i = 0; i < number; ++i) {
        newPol.push_back(data[i].value + value);
    }
    return newPol;
}

template<class decimal>
template<typename Type>
Polynomial<decimal> Polynomial<decimal>::operator*(Type value) {
    Polynomial newPol = Polynomial();
    for (int i = 0; i < number; ++i) {
        newPol.push_back(data[i].value * value);
    }
    return newPol;
}

template<class decimal>
template<typename Type>
Polynomial<decimal> Polynomial<decimal>::operator-(Type value) {
    Polynomial newPol = Polynomial();
    for (int i = 0; i < number; ++i) {
        newPol.push_back(data[i].value - value);
    }
    return newPol;
}

template<class decimal>
template<typename Type>
Polynomial<decimal> Polynomial<decimal>::operator/(Type value) {
    Polynomial newPol = Polynomial();
    for (int i = 0; i < number; ++i) {
        newPol.push_back(data[i].value / value);
    }
    return newPol;
}

template<class decimal>
bool Polynomial<decimal>::isExistKey(int degree) {
    for (int i = 0; i < number; ++i) {
        if (data[i].key == degree) {
            return true;
        }
    }
    return false;
}

template<class decimal>
int Polynomial<decimal>::getIndexByKey(int key) {
    for (int i = 0; i < number; ++i) {
        if (data[i].key == key) {
            return i;
        }
    }
    return 0;
}

template<class decimal>
Polynomial<decimal> &Polynomial<decimal>::operator=(const Polynomial &other) {
    delete[] data;
    data = nullptr;
    number = 0;
    for (int i = 0; i < other.Length(); ++i) {
        this->push_back(other[i], other.getKey(i));
    }
    return *this;
}

template<class decimal>
Polynomial<decimal> &Polynomial<decimal>::operator+=(Polynomial &other) {
    Polynomial<decimal> temp = *this + other;
    *this = temp;
    return *this;
}

template<class decimal>
template<int index1, int index2>
Polynomial<decimal> Polynomial<decimal>::slice() {
    int real_index1 = GetRealIndex(index1);
    int real_index2 = GetRealIndex(index2);
    Polynomial newPolynomial = Polynomial();
    if (real_index1 < real_index2) {
        for (int i = real_index1; i <= real_index2; ++i) {
            newPolynomial.push_back(data[i].value);
            newPolynomial.data[i - real_index1].key = data[i].key;
        }
    }
    else {
        for (int i = real_index1, j = 0; i >= real_index2; --i, j++) {
            newPolynomial.push_back(data[i].value);
            newPolynomial.data[j].key = data[i].key;
        }
    }
    return newPolynomial;
}

template<class decimal>
Polynomial<decimal>::Polynomial(const decimal value) : Polynomial<decimal>() {
    Polynomial<decimal>::push_back(value);
}

template<class decimal>
Polynomial<decimal>::Polynomial(const int num, decimal value) :Polynomial<decimal> () {
    for (int i = 0; i < num; ++i) {
        Polynomial<decimal>::push_back(value);
    }
}


#endif //LBAB3_POLINOM_H
