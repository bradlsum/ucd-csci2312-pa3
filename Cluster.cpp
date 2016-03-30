// Sumner Bradley

#include <sstream>
#include <iomanip>
#include <limits>

#include "Point.h"
#include "Cluster.h"
#include "Exceptions.h"
#include <assert.h>

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
	Cluster::Centroid::Centroid(unsigned int d, const Cluster &c) : __c(c), __p(d) { // needs ref to cluster
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
	void Cluster::Centroid::compute() {

		LNodePtr temp = __c.__points;

		if (__c.__points == nullptr) {
			toInfinity();
			return;
		}

		double sum = 0;
		int i = 0;
		while( i < __dimensions) {
			for (int j = 0; j < __c.__size; ++j) {
				if(temp != nullptr){
					sum += temp->point[i];
					temp = temp->next;
				}
			}
			__p.setValue(i, sum / (__c.getSize()));
			sum = 0;
			temp = __c.__points;
			i++;
		}
		setValid(true);
	}

	bool Cluster::Centroid::equal(const Point &p) const {
		bool eq = true;
		for (unsigned int i = 0; i < __dimensions; ++i) {
			if (__p[i] != p[i]) {
				eq = false;
				break;
			}
		}

		return eq;
	}

	void Cluster::Centroid::toInfinity(){
		for(int i = 0; i < __p.getDims(); ++i){
			__p[i] = std::numeric_limits<double>::max();
		}
		setValid(true);
	}

	Cluster::Cluster(unsigned int d) : centroid(d, *this) {
		__dimensionality = d;
		__size = 0;
		__points = nullptr;
		__id = __idGenerator;
		++__idGenerator;
	}

	void Cluster::pickCentroids(unsigned int k, Point **pointArray) { // pick k initial centroids
		if (k >= __size) {
			for (unsigned int i = 0; i < __size; ++i) {
				*(pointArray[i]) = (*this)[i];
			}
			if (k > __size) {
				for (unsigned int i = __size; i < k; ++i) {
					//pointArray[i] = new Point(__dimensionality);
					for (unsigned int d = 0; d < __dimensionality; ++d) {
						pointArray[i]->setValue(d, std::numeric_limits<double>::max());
						//(pointArray[i])[d] = std::numeric_limits<double>::max();
					}
				}
			}
		}
		else {
			if (k > 100) {
				for (unsigned int i = 0; i < k; ++i) {
					*(pointArray[i]) = (*this)[i];
				}
			}
			else {
				*(pointArray[0]) = __points->point;
				// a is index of point k
				// b is index of cluster
				// c is index of distance between pointArray and next point
				for (unsigned int a = 1; a < k; ++a) {
					double avgD = 0;
					unsigned int furIndx = 0;
					for (unsigned int b = 0; b < __size; ++b) {
						double nextD = 0;
						bool used = false;

						// Average distance between current point and pointArray
						for (unsigned int c = 0; c < a; ++c) {
							nextD += ((*this)[b]).distanceTo(*(pointArray[c]));
							if ((*this)[b] == *(pointArray[c]))
								used = true;
						}
						nextD /= a;

						// New furthest point
						if (nextD > avgD && !used) {
							avgD = nextD;
							furIndx = b;
						}
					}
					*(pointArray[a]) = (*this)[furIndx];
					//pointArray[a] = new Point((*this)[furIndx]);
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

	void Cluster::__del(){
		while(__points != nullptr){
			LNodePtr temp = __points->next;
			delete __points;
			__points = temp;
			__size--;
		}
		assert(__size==0);
	}

	Cluster &Cluster::operator=(const Cluster &C) {
		if(this->getDimensionality() != C.getDimensionality())
			throw DimensionalityMismatchEx(this->getDimensionality(),C.getDimensionality());
		{
			__del();
			__dimensionality= C.__dimensionality;
			__size = C.__size;
			__cpy(C.__points);
			__id = C.__id;
			centroid.compute();
		}
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
		if (__dimensionality != p.getDims()) {
			throw DimensionalityMismatchEx(__dimensionality, p.getDims());
		}
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

	Cluster &Cluster::operator+=(const Point &P1) {
		if(__dimensionality != P1.getDims()){
			throw DimensionalityMismatchEx(__dimensionality,P1.getDims());
		}
		this->add(P1);
		return *this;
	}

	Cluster &Cluster::operator-=(const Point &P1) {
		if(__dimensionality != P1.getDims()){
			throw DimensionalityMismatchEx(__dimensionality,P1.getDims());
		}
		this->remove(P1);
		return *this;
	}

	Cluster &Cluster::operator+=(const Cluster &C2)
	{
		if(__dimensionality != C2.getDimensionality()){
			throw DimensionalityMismatchEx(__dimensionality,C2.__dimensionality);
		}
		for(int i = 0; i < C2.getSize(); ++i){
			if(!(this->contains(C2[i]))){
				this->add(C2[i]);
			}
		}

		return *this;
	}

	Cluster &Cluster::operator-=(const Cluster &C2) {
		for(int i = 0; i < C2.getSize(); ++i){
			if(this->contains(C2[i])){
				this->remove(C2[i]);
			}
		}
		return *this;
	}
	
	std::ostream &operator<<(std::ostream & out, const Cluster & C1){

        LNodePtr temp = C1.__points;
        for(int i = 0; i < C1.__size; ++i){
            out << temp->point << " : " << C1.__id << std::endl;
            temp = temp->next;
        }
        return out;
    }

	std::istream &operator>>(std::istream & in, Clustering::Cluster& C1) {

		std::string temp;
		std::getline(in, temp);
		std::stringstream s;
		s.str(temp);
		Clustering::Point *test = new Clustering::Point(C1.__dimensionality);
		while (!(in.eof())) {
			try {
				in >> *test;
				C1.add(*test);
			} catch (Clustering::DimensionalityMismatchEx &ex) {
				std::cerr << "Caught exception: " << ex << std::endl;
				in.ignore(std::numeric_limits<int>::max(), '\n');
			}
		}

		return in;
	}

	bool operator==(const Cluster &c, const Cluster &cr)
	{
		bool Equal = true;
		LNodePtr Cleft = c.__points;
		LNodePtr Cright = cr.__points;

		if (c.__dimensionality != cr.__dimensionality)
			throw DimensionalityMismatchEx(c.__dimensionality, cr.__dimensionality);

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
		if(c.__dimensionality != p.getDims()){
			throw DimensionalityMismatchEx(c.__dimensionality,p.getDims());
		}
		Cluster sum(c);
		sum += p;
		return sum;
	}

	const Cluster operator-(const Cluster &c, const Point &p)
	{
		if(c.__dimensionality != p.getDims()){
			throw DimensionalityMismatchEx(c.__dimensionality,p.getDims());
		}
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