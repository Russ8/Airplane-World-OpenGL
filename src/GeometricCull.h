#include <glm/glm.hpp>
#ifndef GEOMETRICCULL_H
#define GEOMETRICCULL_H
#define ANG2RAD 3.14159265358979323846/180.0

struct Plane {
	glm::vec3 point;
	glm::vec3 normal;

};

//used to cull objects not in the frustrum in the cpu
class FrustrumCuller {

private:


public:

	Plane TOPp, BOTTOMp, LEFTp, RIGHTp, NEARp, FARp;
	glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;

	FrustrumCuller() {

	}
	~FrustrumCuller() {

	}

	void setCamInternals(float angle, float ratio, float nearD, float farD) {

		this->ratio = ratio;
		this->angle = angle;
		this->nearD = nearD;
		this->farD = farD;


		tang = (float)tan(ANG2RAD * angle);
		nh = nearD * tang;
		nw = nh * ratio;
		fh = farD * tang;
		fw = fh * ratio;
		
	}
	void setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) {
		glm::vec3 dir, nc, fc, X, Y, Z;

		// compute the Z axis of camera

		Z = p - l;
		Z = glm::normalize(Z);

		// X axis of camera with given "up" vector and Z axis
		X = glm::cross(u, Z);
		X = glm::normalize(X);

		// the real "up" vector is the cross product of Z and X
		Y = glm::cross(Z, X);

		// compute the centers of the near and far planes
		nc = p - (Z * nearD);
		fc = p - (Z * farD);

		// compute the 4 corners of the frustum on the near plane
		ntl = nc + (Y * nh) - (X * nw);
		ntr = nc + (Y * nh) + (X * nw);
		nbl = nc - (Y * nh) - (X * nw);
		nbr = nc - (Y * nh) + (X * nw);

		// compute the 4 corners of the frustum on the far plane
		ftl = fc + Y * fh - X * fw;
		ftr = fc + Y * fh + X * fw;
		fbl = fc - Y * fh - X * fw;
		fbr = fc - Y * fh + X * fw;

		//calculate plane normals
		TOPp.normal = glm::normalize(glm::cross(ftl - ntl, ntr - ntl));
		BOTTOMp.normal = glm::normalize(glm::cross(nbr - nbl, fbl - nbl));
		LEFTp.normal = glm::normalize(glm::cross( nbl - ntl, ftl - ntl));
		RIGHTp.normal = glm::normalize(glm::cross( ftr- ntr,  nbr- ntr));
		NEARp.normal = glm::normalize(glm::cross( nbr - ntr, ntl - ntr));
		FARp.normal = glm::normalize(glm::cross( fbl - ftl, ftr - ftl ));

		//store a point for each plane
		TOPp.point = ntr;
		BOTTOMp.point = nbl;
		LEFTp.point = ntl;
		RIGHTp.point = nbr;
		NEARp.point = ntl;
		FARp.point = ftr;
	}
	//calculate distance from point to plane
	float distPlanePoint(glm::vec3 &pointp, glm::vec3 &normal, glm::vec3 &point) {
		glm::vec3 N = glm::normalize(normal);
		return glm::dot(point - pointp, normal) / glm::length(normal);

	}
	//returns true if a point is in the frustrum
	bool pointInFrustum(glm::vec3 &p) {
		int result = 1;

		if (distPlanePoint(TOPp.point, TOPp.normal, p) < 0) return 0; //outside
		if (distPlanePoint(BOTTOMp.point, BOTTOMp.normal, p) < 0) return 0;
		if (distPlanePoint(RIGHTp.point, RIGHTp.normal, p) < 0) return 0;
		if (distPlanePoint(LEFTp.point, LEFTp.normal, p) < 0) return 0;


		return(result);
	}

};

#endif
