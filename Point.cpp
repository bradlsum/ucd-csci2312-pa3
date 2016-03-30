// Sumner Bradley
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include "Point.h"
#include "Exceptions.h"

namespace Clustering {

	unsigned int Point::__idGen = 0;
	const char POINT_VALUE_DELIM = ',';

	// Constructor
	Point::Point(unsigned int dim)
	{
		__id = __idGen;
		__idGen++;
		__dim = dim;

		__values = new double[__dim];

		for (int i = 0; i < __dim;++i) {
			__values[i] = 0.0;
		}
	}

	// Two varriable constructor
	Point::Point(unsigned int dim, double * values)
	{
		__id = __idGen;
		__idGen++;
		__dim = dim;
		__values = values;
	}

	// Copy constructor
	Point::Point(const Point & npoint)
	{
		__dim = npoint.__dim;
		__values = new double[__dim];
		__id = npoint.__id;

		for (int i = 0; i < __dim; ++i) {
			__values[i] = npoint.__values[i];
		}
	}

	// Assignment operator
	Point & Point::operator=(const Point & npoint)
	{
		if (__dim != npoint.__dim)
            throw DimensionalityMismatchEx(__dim, npoint.__dim);

			for (unsigned int i = 0; i < __dim;++i) {
				this->__values[i] = npoint.getValue(i);
			}

		return *this;
	}

	// Destructor
	Point::~Point() {
		if (__values != NULL)
		{
			delete[] __values;
		}
		__values = NULL;
	}

	void Point::rewindIdGen() {
		__idGen--;

	}

	// Accessors & mutators
	int Point::getId() const
	{
		return __id;
	}

	unsigned int Point::getDims() const
	{
		return __dim;
	}

	void Point::setValue(unsigned int i, double v)
	{
		if (i >= __dim) {
			throw OutOfBoundsEx(__dim, i);
		}
		__values[i] = v;
	}

	double Point::getValue(unsigned int i) const
	{
		if (i >= __dim) {
			throw OutOfBoundsEx(__dim, i);
		}
		return __values[i];
	}

	// Functions
	double Point::distanceTo(const Point & point) const
	{
		if (__dim != point.__dim)
			throw DimensionalityMismatchEx(__dim, point.__dim);

		double sumOfProducts = 0;

		for (int i = 0; i < __dim; ++i) {
			sumOfProducts += pow(point.getValue(i) - getValue(i), 2);
		}

		return sqrt(sumOfProducts);
	}

	// Members
	Point & Point::operator*=(double x)
	{
		for (int i = 0; i < __dim;++i) {

			__values[i] *= x;
		}

		// TODO: insert return statement here
		return *this;
	}

	Point & Point::operator/=(double x)
	{
		for (int i = 0; i < __dim;++i) {
			__values[i] = __values[i] / x;
		}
		// TODO: insert return statement here
		return *this;
	}

	const Point Point::operator*(double x) const
	{
		for (int i = 0; i < __dim;++i) {
			__values[i] = __values[i] * x;
		}
		return *this;
	}

	const Point Point::operator/(double x) const
	{
		for (int i = 0; i < __dim;++i) {
			__values[i] = __values[i] / x;
		}
		return *this;
	}

	double &Point::operator[](unsigned int index) {
		if (index >= __dim)
			throw OutOfBoundsEx(__dim, index);

		return __values[index];
	}

	const double &Point::operator[](unsigned int index) const {
		if (index >= __dim)
			throw OutOfBoundsEx(__dim, index);

		return __values[index];
	}

	Point &operator+=(Point &arg_Point_left, const Point &arg_Point_right)
	{
		if (arg_Point_left.__dim == arg_Point_right.__dim)
		for (int index = 0; index < arg_Point_left.__dim; ++index)
		arg_Point_left.__values[index] += arg_Point_right.__values[index];

		return arg_Point_left;
	}


	Point &operator-=(Point &arg_Point_left, const Point &arg_Point_right)
	{
		if (arg_Point_left.__dim == arg_Point_right.__dim)
		for (int index = 0; index < arg_Point_left.__dim; ++index)
		arg_Point_left.__values[index] -= arg_Point_right.__values[index];

		return arg_Point_left;
	}


	const Point operator+(const Point & p1, const Point & p2)
	{

		Point p0(p1.getDims());

		if (p1.getDims() == p2.getDims()) {
			for (int i = 0; i < p1.getDims();++i) {
				p0.__values[i] = p1.__values[i] + p2.__values[i];
			}
		}

		return p0;
	}

	const Point operator-(const Point & p1, const Point & p2)
	{
		Point p0(p1.getDims());

		if (p1.getDims() == p2.getDims()) {
			for (int i = 0; i < p1.getDims();++i) {
				p0.__values[i] = p1.__values[i] - p2.__values[i];
			}
		}

		return p0;
	}

	bool operator==(const Point & p1, const Point & p2)
	{
		if (p1.__id != p2.__id) return false;
		if (p1.__dim == p2.__dim) {
			for (int i = 0; i < p1.__dim;++i) {
				if (p1.__values[i] != p2.__values[i]) return false;
			}
			return true;
		}
		return false;
	}

	bool operator!=(const Point & p1, const Point & p2)
	{
		if (p1.__id != p2.__id) return true;
		if (p1.__dim == p2.__dim) {
			for (int i = 0; i < p1.__dim;++i) {
				if (p1.__values[i] != p2.__values[i]) return true;
			}
			return false;
		}
		return true;
	}

	bool operator<(const Point & p1, const Point & p2)
	{
		if (p1.__dim == p2.__dim) {
			for (int i = 0; i < p1.__dim;++i) {
				if (p1.__values[i] < p2.__values[i]) return true;
			}
		}
		return false;
	}

	bool operator>(const Point & p1, const Point & p2)
	{
		if (p1.__dim == p2.__dim) {
			for (int i = 0; i < p1.__dim;++i) {
				if (p1.__values[i] > p2.__values[i]) return true;
			}
		}
		return false;
	}

	bool operator<=(const Point & p1, const Point & p2)
	{
		if (p1.__dim == p2.__dim) {
			for (int i = 0; i < p1.__dim;++i) {
				if (p1.__values[i] <= p2.__values[i]) return true;
			}
		}
		return false;
	}

	bool operator>=(const Point & p1, const Point & p2)
	{
		if (p1.__dim == p2.__dim) {
			for (int i = 0; i < p1.__dim;++i) {
				if (p1.__values[i] >= p2.__values[i]) return true;
			}
		}
		return false;
	}

	std::istream &operator>>(std::istream &is, Clustering::Point &p){
		unsigned int index = 0;
		double temp;

		while ((is.peek() != '\n') || (is.peek() != '\r')){
			is >> temp;
			try{
				p.setValue(index, temp);
			}catch(Clustering::OutOfBoundsEx &ex){
				throw Clustering::DimensionalityMismatchEx(p.__dim,index);
			}

			if((is.peek() == '\n') || (is.peek() == '\r') || (is.eof())){
				return is;
			}
			is.ignore(100, POINT_VALUE_DELIM);
			index++;
		}
		if(index != p.__dim){
			throw Clustering::DimensionalityMismatchEx(p.__dim,index);
		}
		return is;
	}

	std::ostream &operator<<(std::ostream &out, const Point &P1){
		int i = 0;
		for( ; i < P1.getDims()-1; ++i){
			out << P1.getValue(i);
			out << POINT_VALUE_DELIM << " ";
		}
		out << P1.getValue(i);

		return out;
	}
}
