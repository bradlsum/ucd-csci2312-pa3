// Sumner Bradley

#include <sstream>
#include <iomanip>
#include <limits>

#include "Point.h"
#include "Cluster.h"
#include "Exceptions.h"

namespace Clustering {
	unsigned int Cluster::__idGenerator = 0;
	typedef Point * PointPtr;
	LNode::LNode(const Point & p, LNodePtr n) : point(p), next(n) {}

	/*void Cluster::__del()
	{
	}

	void Cluster::__cpy(LNodePtr pts)
	{

	}

	bool Cluster::__in(const LNodePtr pts) const
	{

	}*/

	// Centroid class
	Cluster::Centroid::Centroid(unsigned int d, const Cluster & c) : __c(c), __p(d)
	{
		__dimensions = d;

		if (__c.__size == 0) {

			__valid = false;

			toInfinity();
		}
	}

	const Point Cluster::Centroid::get() const
	{
		return __p;
	}

	void Cluster::Centroid::set(const Point & p)
	{
		__valid = true;

		__p = p;
	}

	bool Cluster::Centroid::isValid() const
	{
		return __valid;
	}

	void Cluster::Centroid::setValid(bool valid)
	{
		__valid = valid;
	}

	// Centroid functions
	void Cluster::Centroid::compute()
	{
		double avg;

		if (__c.__size > 0) {
			for (unsigned int d = 0; d < __dimensions; ++d) {
				avg = 0;

				for (unsigned int i = 0; i < __c.__size; ++i) {
					avg += (__c[i])[d];
				}

				__p[d] = avg / __c.__size;

			}
		}
		else {
			toInfinity();
		}

		__valid = true;
	}

	bool Cluster::Centroid::equal(const Point & p) const
	{
		bool eq = true;
		for (unsigned int i = 0; i < __dimensions; ++i) {
			if (__p[i] != p[i]) {
				eq = false;
				break;
			}
		}

		return eq;
	}

	void Cluster::Centroid::toInfinity()
	{
		for (unsigned int i = 0; i < __dimensions; ++i) {
			__p[i] = std::numeric_limits<double>::max();
		}
	}

	Cluster::Cluster(unsigned int d) : centroid(d, *this) {
		__dimensionality = d;
		__size = 0;
		__points = nullptr;
		__id = __idGenerator;
		++__idGenerator;
	}

	void Cluster::pickCentroids(unsigned int k, Point ** pointArray)
	{
		if (k >= __size) {
			for (unsigned int i = 0; i < __size; ++i) {
				*(pointArray[i]) = (*this)[i];
			}
			if (k > __size) {
				for (unsigned int i = __size; i < k; ++i) {

					for (unsigned int d = 0; d < __dimensionality; ++d) {
						pointArray[i]->setValue(d, std::numeric_limits<double>::max());

					}
				}
			}
		}
	}

	void Cluster::__cpy(LNodePtr pts) {
		if(pts == nullptr){
			__points = nullptr;
			__size = 0;
			return;
		}
		LNodePtr origCurrent = pts;
		LNodePtr current = __points = new LNode(origCurrent->point,nullptr);
		while (origCurrent->next != nullptr) {
			origCurrent = origCurrent->next;
			current->next = new LNode(origCurrent->point,nullptr);
			current =current->next;

		}
	}

	Cluster::Cluster(const Cluster & entry):centroid(entry.__dimensionality,*this)
	{
		if(this != &entry)
		{
			__dimensionality= entry.__dimensionality;
			__size = entry.__size;
			__cpy(entry.__points); // TODO uncomment - just testing the build!!!
			__id = entry.__id;
			centroid.compute();
		}
	}

	Cluster &Cluster::operator=(const Cluster &origin) {
		if (__size > 0) {
			LNodePtr destroy;
			LNodePtr cursor = __points;

			while (cursor != nullptr) {
				destroy = cursor;
				cursor = cursor->next;

				delete destroy;
			}
			__size = 0;
		}

		for (int i = 0; i < origin.getSize(); ++i)
		{
			add(origin[i]);
		}

		__id = origin.__id;
		centroid.compute();

		return *this;
	}

	Cluster::~Cluster()
	{
		__points = NULL;
		__size = 0;
	}

	unsigned int Cluster::getSize() const
	{
		return __size;
	}

	unsigned int Cluster::getDimensionality() const
	{
		return __dimensionality;
	}

	unsigned int Cluster::getId() const
	{
		return __id;
	}

	void Cluster::add(const Clustering::Point & p) {

		PointPtr Newp;
		Newp = new Point(p);

		if (__points == NULL)
		{
			LNodePtr newNode;
			newNode = new LNode(*Newp, NULL);
			__points = newNode;
			__size = 1;

		}
		else
		{
			LNodePtr Npre;
			LNodePtr cur;


			Npre = __points;

			cur = Npre->next;

			LNodePtr iBef = NULL;

			if (cur == NULL)
			{
				if (p < Npre->point)

					iBef = Npre;

				else iBef = cur;
			}
			else
			{
				if (p < Npre->point)
					iBef = Npre;

				for (; iBef == NULL && cur != NULL; cur = cur->next)
				{
					if (p < cur->point)
						iBef = cur;
					else
						Npre = cur;
				}
			}
			if (iBef == __points)
			{
				LNodePtr newNode;

				newNode = new LNode(*Newp, __points);

				__points = newNode;
				++__size;
			}
			else
			{
				LNodePtr newNode;

				newNode = new LNode(*Newp, iBef);

				Npre->next = newNode;

				++__size;
			}
		}
	}

	const Point &Cluster::remove(const Point &p) {
		if (contains(p)) {
			LNodePtr Nptr;
			LNodePtr Pptr = nullptr;

			Nptr = __points;

			while (Nptr != nullptr) {
				if (Nptr->point == p) {
					if (Pptr == nullptr) {
						__points = Nptr->next;
						delete Nptr;
						__size--;

						break;
					}
					else {
						Pptr->next = Nptr->next;
						delete Nptr;

						--__size;
						break;
					}
				}
				Pptr = Nptr;

				Nptr = Nptr->next;
			}
		}

		return p;
	}

	bool Cluster::contains(const Point &p) const {
		LNodePtr next = __points;

		while (next != nullptr) {
			if (next->point.getId() == p.getId())
				return true;
			else
				next = next->next;
		}
		return false;
	}

	const Point & Cluster::operator[](unsigned int index) const
		{
			if(__size == 0)
				throw EmptyClusterEx();// zero member on cluster exception
			if(index > __size-1)
				throw OutOfBoundsEx(__size,index); //out of bound exception
			LNodePtr current = __points;
			for (int i = 0; i < index; ++i) {
				current = current->next;
			}
			return current->point;
	}

	Cluster &Cluster::operator+=(const Point &p) {
		add(p);

		return *this;
	}

	Cluster &Cluster::operator-=(const Point &p) {
		remove(p);

		return *this;
	}

	Cluster &Cluster::operator+=(const Cluster &arg_Cluster)
	{
		LNodePtr cursor_right = arg_Cluster.__points;

		for (; cursor_right != NULL; cursor_right = cursor_right->next)
		{
			if (!(this->contains(cursor_right->point)))
				add(cursor_right->point);
		}

		return *this;
	}

	Cluster &Cluster::operator-=(const Cluster &rhs) {
		for (int i = 0; i < rhs.getSize(); ++i) {
			remove(rhs[i]);
		}

		return *this;
	}
	
	std::ostream &operator<<(std::ostream &out, const Cluster &cluster) {
		out << std::setprecision(20);
		for (int i = 0; i < cluster.getSize(); ++i) {
			out << cluster[i] << std::endl;
			std::cout << cluster[i] << std::endl;
		}

		return out;
	}

	std::istream &operator>>(std::istream &ins, Cluster &c)
	{
		std::string value, buf;
		PointPtr Newp = NULL;
		while (getline(ins, value))
		{
			int pointSize = 0;

			std::stringstream lineStream_count(value);

			std::stringstream lineStream_do(value);

			while (getline(lineStream_count, buf, ','))
				++pointSize;

			Newp = new Point(pointSize);
			lineStream_do >> *Newp;
			c.add(*Newp);

		}
		return ins;
	}
	
	bool operator==(const Cluster &c, const Cluster &cr)
	{
		bool Equal = true;
		LNodePtr Cleft = c.__points;
		LNodePtr Cright = cr.__points;

		while (Equal && Cleft != NULL && Cright != NULL)
		{
			if (Cleft->point != Cright->point)
			{
				Equal = false;
				continue;
			}
			else
			{
				Cleft = Cleft->next;
				Cright = Cright->next;
			}
		}
		if (Equal)
		{
			if (Cleft != NULL || Cright != NULL) Equal = false;
		}
		return Equal;
	}

	bool operator!=(const Cluster &c, const Cluster &cr)
	{
		return (!(c == cr));
	}

	const Cluster operator+(const Cluster &c, const Point &p)
	{
		Cluster sum(c);
		sum += p;
		return sum;
	}

	const Cluster operator-(const Cluster &c, const Point &p)
	{
		Cluster sub(c);
		sub -= p;
		return sub;
	}

	const Cluster operator+(const Cluster &c, const Cluster &cr)
	{
		Cluster newCluster(c);
		return newCluster += cr;
	}

	const Cluster operator-(const Cluster &c, const Cluster &cr)
	{
		Cluster newCluster(c);
		return newCluster -= cr;
	}
	
	Cluster::Move::Move(const Point &p, Cluster &from, Cluster &to)
		: __p(p), __from(from), __to(to) {
		//perform();
	}

	void Cluster::Move::perform() {
		if (__from.contains(__p)) {
			__to.add(__from.remove(__p));

			//__to.centroid.setValid(false);
			//__from.centroid.setValid(false);
		}

		if (__to.__size == 0) {
			__to.centroid.toInfinity();
		}

		if (__from.__size == 0) {
			__from.centroid.toInfinity();
		}
	}

}