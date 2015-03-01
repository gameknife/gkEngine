//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//
// yikaiming (c) 2013
// GameKnife ENGINE Source File
//
// Name:   	gk_GeoIntersect.h
// Desc:	
// 	
// 
// Author:  yikaiming
// Date:	2013/6/1	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gk_GeoIntersect_h__
#define gk_GeoIntersect_h__

#include "gk_Geo.h"

namespace Intersect {



	inline bool Ray_Plane(const Ray &ray, const Plane &plane, Vec3 &output, bool bSingleSidePlane = true ) {
		float cosine	=	plane.n | ray.direction;

		//REJECTION 1: if "line-direction" is perpendicular to "plane-normal", an intersection is not possible! That means ray is parallel
		//             to the plane
		//REJECTION 2: if bSingleSidePlane == true we deal with single-sided planes. That means
		//             if "line-direction" is pointing in the same direction as "the plane-normal", 
		//             an intersection is not possible!
		if ((cosine == 0.0f) ||										// normal is orthogonal to vector, cant intersect
			(bSingleSidePlane && (cosine > 0.0f)))  // we are trying to find an intersection in the same direction as the plane normal
			return false;					

		float numer		= plane.DistFromPlane(ray.origin);
		float fLength	= -numer / cosine;
		output				=	ray.origin + (ray.direction * fLength);
		//skip, if cutting-point is "behind" ray.origin
		if (fLength < 0.0f) 
			return false;

		return true;		//intersection occurred	
	}

	inline bool Line_Plane(const Line &line, const Plane &plane, Vec3 &output, bool bSingleSidePlane = true  ) {
		float cosine		=	plane.n | line.direction;

		//REJECTION 1: if "line-direction" is perpendicular to "plane-normal", an intersection is not possible! That means ray is parallel
		//             to the plane
		//REJECTION 2: if bSingleSidePlane == true we deal with single-sided planes. That means
		//             if "line-direction" is pointing in the same direction as "the plane-normal", 
		//             an intersection is not possible!
		if ((cosine == 0.0f) ||										// normal is orthogonal to vector, cant intersect
			(bSingleSidePlane && (cosine > 0.0f)))  // we are trying to find an intersection in the same direction as the plane normal
			return false;					

		//an intersection is possible: calculate the exact point!
		float perpdist	= plane | line.pointonline;
		float pd_c			=	-perpdist / cosine;
		output					=	line.pointonline + (line.direction * pd_c);

		return true;		//intersection occurred	
	}

	// Algorithm description: 
	// http://softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm#Line-Plane%20Intersection 
	template <typename T>
	inline bool Segment_Plane(const Lineseg_tpl<T>& segment, const Plane_tpl<T> &plane, Vec3_tpl<T> &vOutput, bool bSingleSidePlane = true ) {
		Vec3_tpl<T> vSegment								= segment.end - segment.start;
		T						planeNormalDotSegment		=	plane.n | vSegment;

		//REJECTION 1: if "line-direction" is perpendicular to "plane-normal", an intersection is not possible! That means ray is parallel
		//             to the plane
		//REJECTION 2: if bSingleSidePlane == true we deal with single-sided planes. That means
		//             if "line-direction" is pointing in the same direction as "the plane-normal", 
		//             an intersection is not possible!
		if ((planeNormalDotSegment == T(0)) ||									 // normal is orthogonal to vector, cant intersect
			(bSingleSidePlane && (planeNormalDotSegment > T(0))))  // we are trying to find an intersection in the same direction as the plane normal
			return false;					

		// n Dot (segment.start - closest_point_in_plane) = 1 * DistFromPlane(segment.start) * cos(0) = DistFromPlane(segment.start)
		T distanceToStart	= plane.DistFromPlane(segment.start);					
		T scale						= -distanceToStart / planeNormalDotSegment;
		vOutput						=	segment.start + (vSegment * scale);

		// skip, if segment start and ends in one side of the plane
		if ((scale < T(0)) || (scale > T(1)))
			return false;

		return true;		//intersection occurred	
	}

	/// Intersection between two line segments in 2D (ignoring z coordinate). The two parametric 
	/// values are set to between 0 and 1 if intersection occurs. If intersection does not occur 
	/// their values will indicate the parametric values for intersection of the lines extended 
	/// beyond the segment lengths. Parallel lines will result in a negative result, but the parametric
	/// values will both be equal to 0.5
	template<typename F>
	inline bool Lineseg_Lineseg2D(const Lineseg_tpl<F> &lineA, const Lineseg_tpl<F> &lineB, F &tA, F &tB)
	{
		// used for parallel testing
		static const F epsilon = 0.0000001f;

		Vec3_tpl<F> delta = lineB.start - lineA.start;
		Vec3_tpl<F> lineADir = lineA.end - lineA.start;
		Vec3_tpl<F> lineBDir = lineB.end - lineB.start;
		F crossD = lineADir.x * lineBDir.y - lineADir.y * lineBDir.x;
		F crossDelta1 = delta.x * lineBDir.y - delta.y * lineBDir.x;
		F crossDelta2 = delta.x * lineADir.y - delta.y * lineADir.x;

		if ( fabs(crossD) > epsilon )
		{
			// intersection
			tA = crossDelta1 / crossD;
			tB = crossDelta2 / crossD;
		}
		else
		{
			// parallel - maybe should really test for lines overlapping each other?
			tA = tB = 0.5;
			return false;
		}

		if (tA > (F)1.0 || tA < (F)0.0 || tB > (F)1.0 || tB < (F)0.0)
			return false;

		return true;
	}

	/// Calculates the intersection between a line segment and a polygon, in 2D (i.e. 
	/// ignoring z coordinate). The VecContainer should be a container of Vec3 such 
	/// that we can traverse it using iterators. intersectionPoint is set to the intersection
	/// point or the end of the segment, if no intersection.
	template<typename VecContainer>
	inline bool Lineseg_Polygon2D(const Lineseg & lineseg, const VecContainer & polygon, Vec3 & intersectionPoint, Vec3* pNormal=NULL, bool bForceNormalOutwards = false)
	{
		intersectionPoint = lineseg.end;
		bool gotIntersection = false;

		float tmin = 1.0f;

		typename VecContainer::const_iterator iend = polygon.end();
		typename VecContainer::const_iterator li, linext;
		Lineseg intersectSegment;
		for (li = polygon.begin() ; li != iend; ++li)
		{
			linext = li;
			++linext;
			if (linext == iend)
				linext = polygon.begin();
			Lineseg segmentPoly(*li,*linext) ;
			float s,t;
			if (Intersect::Lineseg_Lineseg2D(lineseg, segmentPoly, s, t))
			{
				if (s < 0.00001f || s > 0.99999f || t < 0.00001f || t > 0.99999f)
					continue;
				if (s < tmin)
				{
					tmin = s;
					gotIntersection = true;
					intersectSegment = segmentPoly;
				}
			}
		}

		intersectionPoint = lineseg.start + tmin*(lineseg.end - lineseg.start);

		if(pNormal && gotIntersection)
		{
			Vec3 vPolyseg =intersectSegment.end - intersectSegment.start;
			Vec3 vIntSeg = (lineseg.end - lineseg.start);
			pNormal->x = vPolyseg.y;
			pNormal->y = -vPolyseg.x;
			pNormal->z = 0;
			pNormal->NormalizeSafe();
			// returns the normal towards the start point of the intersecting segment (if it's not forced to be outwards)
			if(!bForceNormalOutwards && vIntSeg.Dot(*pNormal) >0)
			{
				pNormal->x = -pNormal->x;
				pNormal->y = -pNormal->y;
			}
		}
		return gotIntersection;
	}


	/*
	* calculates intersection between a line and a triangle.
	* IMPORTANT: this is a single-sided intersection test. That means its not enough 
	* that the triangle and line overlap, its also important that the triangle 
	* is "visible" when you are looking along the line-direction.  
	* 
	* If you need a double-sided test, you'll have to call this function twice with 
	* reversed order of triangle vertices.   
	* 
	* return values 
	* if there is an intertection the functions return "true" and stores the  
	* 3d-intersection point in "output". if the function returns "false" the value in 
	* "output" is undefined  
	* 
	*/
	inline bool Line_Triangle( const Line &line, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, Vec3 &output ) {
		//find vectors for two edges sharing v0
		Vec3 edge_1 = v1-v0;
		Vec3 edge_2 = v2-v0;
		//begin calculating determinant - also used to calculate U parameter
		Vec3 pvec  =  line.direction % edge_1; 
		//if determinat is near zero, ray lies in plane of triangel 
		float det = edge_2 | pvec;
		if (det<=0) return 0;
		//calculate distance from v0 to ray origin
		Vec3 tvec=line.pointonline-v0;
		//calculate U parameter and test bounds
		float u=tvec | pvec;
		if (u<0.0f || u>det) return 0;
		//prepare to test V parameter
		Vec3 qvec=tvec % edge_2;
		//calculate V parameter and test bounds
		float v= (line.direction | qvec);
		if ( v<0.0f || (u+v)>det) return 0;
		//------------------------------------------------------
		//we have an intersection and now we can calculate t
		float t = (edge_1 | qvec) / det;
		//we use t als a scale parameter, to get the 3D-intersection point
		output = (line.direction*t)+line.pointonline;
		return 1;
	}







	/*
	* calculates intersection between a ray and a triangle.
	* IMPORTANT: this is a single-sided intersection test. That means its not sufficient 
	* that the triangle and rayt overlap, its also important that the triangle 
	* is "visible" when you from the origin along the ray-direction.  
	* 
	* If you need a double-sided test, you'll have to call this function twice with 
	* reversed order of triangle vertices.   
	* 
	* return values 
	* if there is an intertection the functions return "true" and stores the  
	* 3d-intersection point in "output". if the function returns "false" the value in 
	* "output" is undefined  
	*/
	inline bool Ray_Triangle( const Ray &ray, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, Vec3 &output ) {
		//find vectors for two edges sharing v0
		Vec3 edge_1 = v1-v0;
		Vec3 edge_2 = v2-v0;
		//begin calculating determinant - also used to calculate U parameter
		Vec3 pvec  =  ray.direction % edge_1; 
		//if determinat is near zero, ray lies in plane of triangle 
		float det = edge_2 | pvec;
		if (det<=0) return 0;
		//calculate distance from v0 to ray origin
		Vec3 tvec=ray.origin-v0;
		//calculate U parameter and test bounds
		float u=tvec | pvec;
		if (u<0.0f || u>det) return 0;
		//prepare to test V parameter
		Vec3 qvec=tvec % edge_2;
		//calculate V parameter and test bounds
		float v= (ray.direction | qvec);
		if ( v<0.0f || (u+v)>det) return 0;
		//------------------------------------------------------
		//We have an intersection and we can calculate t
		float t = (edge_1 | qvec) / det;
		//we use t als a scale parameter, to get the 3D-intersection point
		output = (ray.direction*t)+ray.origin;
		//skip, if cutting-point is "behind" ray.origin
		if (((output-ray.origin)|ray.direction)<0) return 0;
		return 1;
	}









	/*
	* Description:
	*	Calculates intersection between a line-segment and a triangle.
	* Remarks:
	*	IMPORTANT: this is a single-sided intersection test. That means its not sufficient 
	*	that the triangle and line-segment overlap, its also important that the triangle 
	*	is "visible" when you are looking along the linesegment from "start" to "end".  
	* Notes:
	*	If you need a double-sided test, you'll have to call this function twice with 
	*	reversed order of triangle vertices.   
	* 
	* Return value: 
	*	If there is an intertection the the functions return "true" and stores the  
	*	3d-intersection point in "output". if the function returns "false" the value in 
	*	"output" is undefined. If pT is non-zero then if there is an intersection the "t-value"
	*	(from 0-1) is also returned (unmodified if there is no intersection).
	*/
	inline bool Lineseg_Triangle( const Lineseg &lineseg, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, Vec3 &output, float * pT = 0 ) {
		//find vectors for two edges sharing v0
		Vec3 edge_1 = v1-v0;
		Vec3 edge_2 = v2-v0;
		//direction of lineseg. normalizing is not necessary
		Vec3 direction=(lineseg.end-lineseg.start);
		//begin calculating determinant - also used to calculate U parameter
		Vec3 pvec  =  direction % edge_1; 
		//if determinat is near zero, ray lies in plane of triangle 
		float det = edge_2 | pvec;
		if (det<=-0.00001f) return 0;
		//calculate distance from v0 to ray origin
		Vec3 tvec=lineseg.start-v0;
		//calculate U parameter and test bounds
		float u=tvec | pvec;
		if (u<0.0f || u>det) return 0;
		//prepare to test V parameter
		Vec3 qvec=tvec % edge_2;
		//calculate V parameter and test bounds
		float v= (direction | qvec);
		if ( v<0.0f || (u+v)>=det) return 0;
		//------------------------------------------------------
		//If we ignore the limitations of linesegment, then we have an intersection and we calcultate t
		float t = (edge_1 | qvec) / det;
		//we use t as a scale parameter, to get the 3D-intersection point
		output = (direction*t)+lineseg.start;
		//skip, if lineseg and triangle are not overlapping
		if (((output-lineseg.start)|direction)<0) return 0;
		if (((output-lineseg.end)|direction)>0) return 0;
		if (pT) *pT = t;
		return 1;
	}




	//----------------------------------------------------------------------------------
	//  Ray_AABB
	//
	//	just ONE intersection point is calculated, and thats the entry point           - 
	//   Lineseg and AABB are assumed to be in the same space
	//
	//--- 0x00 = no intersection (output undefined)        --------------------------
	//--- 0x01 = intersection (intersection point in output)              --------------
	//--- 0x02 = start of Lineseg is inside the AABB (ls.start is output) 
	//----------------------------------------------------------------------------------
	inline uint8 Ray_AABB( const Ray& ray,const AABB& aabb, Vec3& output1 ) {

		uint8 cflags;
		float cosine;
		Vec3 cut;
		//--------------------------------------------------------------------------------------
		//----         check if "ray.origin" is inside of AABB    ---------------------------
		//--------------------------------------------------------------------------------------
		cflags =(ray.origin.x>=aabb.min.x)<<0;
		cflags|=(ray.origin.x<=aabb.max.x)<<1;
		cflags|=(ray.origin.y>=aabb.min.y)<<2;
		cflags|=(ray.origin.y<=aabb.max.y)<<3;
		cflags|=(ray.origin.z>=aabb.min.z)<<4;
		cflags|=(ray.origin.z<=aabb.max.z)<<5;
		if (cflags==0x3f)	{	output1=ray.origin;	return 0x02; }

		//--------------------------------------------------------------------------------------
		//----         check intersection with planes           ------------------------------
		//--------------------------------------------------------------------------------------
		for(int i=0; i<3; i++) {
			if ( (ray.direction[i]>0) && (ray.origin[i]<aabb.min[i]) )	{
				cosine = (-ray.origin[i]+aabb.min[i])/ray.direction[i];
				cut[i]=aabb.min[i]; 
				cut[incm3(i)]=ray.origin[incm3(i)]+(ray.direction[incm3(i)]*cosine); 
				cut[decm3(i)]=ray.origin[decm3(i)]+(ray.direction[decm3(i)]*cosine);
				if ((cut[incm3(i)]>aabb.min[incm3(i)]) && (cut[incm3(i)]<aabb.max[incm3(i)]) && (cut[decm3(i)]>aabb.min[decm3(i)]) && (cut[decm3(i)]<aabb.max[decm3(i)])) {
					output1=cut;	return 0x01;
				}
			}
			if ( (ray.direction[i]<0) && (ray.origin[i]>aabb.max[i]) )	{
				cosine = (+ray.origin[i]-aabb.max[i])/ray.direction[i];
				cut[i]=aabb.max[i]; 
				cut[incm3(i)]=ray.origin[incm3(i)]-(ray.direction[incm3(i)]*cosine); 
				cut[decm3(i)]=ray.origin[decm3(i)]-(ray.direction[decm3(i)]*cosine);
				if ((cut[incm3(i)]>aabb.min[incm3(i)]) && (cut[incm3(i)]<aabb.max[incm3(i)]) && (cut[decm3(i)]>aabb.min[decm3(i)]) && (cut[decm3(i)]<aabb.max[decm3(i)])) {
					output1=cut;	return 0x01;
				}
			}
		}
		return 0x00;//no intersection
	}



	//----------------------------------------------------------------------------------
	//  Ray_OBB
	//
	//	just ONE intersection point is calculated, and thats the entry point           - 
	//   Lineseg and OBB are assumed to be in the same space
	//
	//--- 0x00 = no intersection (output undefined)                                 ----
	//--- 0x01 = intersection (intersection point in output)              --------------
	//--- 0x02 = start of Lineseg is inside the OBB (ls.start is output) 
	//----------------------------------------------------------------------------------
	inline uint8 Ray_OBB( const Ray& ray,const Vec3& pos,const OBB& obb, Vec3& output1 ) {

		AABB aabb(obb.c-obb.h,obb.c+obb.h);
		Ray aray((ray.origin-pos)*obb.m33, ray.direction*obb.m33);

		uint8 cflags;
		float cosine;
		Vec3 cut;
		//--------------------------------------------------------------------------------------
		//----         check if "aray.origin" is inside of AABB    ---------------------------
		//--------------------------------------------------------------------------------------
		cflags =(aray.origin.x>aabb.min.x)<<0;
		cflags|=(aray.origin.x<aabb.max.x)<<1;
		cflags|=(aray.origin.y>aabb.min.y)<<2;
		cflags|=(aray.origin.y<aabb.max.y)<<3;
		cflags|=(aray.origin.z>aabb.min.z)<<4;
		cflags|=(aray.origin.z<aabb.max.z)<<5;
		if (cflags==0x3f)	{	output1=aray.origin;	return 0x02; }

		//--------------------------------------------------------------------------------------
		//----         check intersection with planes           ------------------------------
		//--------------------------------------------------------------------------------------
		for(int i=0; i<3; i++) {
			if ( (aray.direction[i]>0) && (aray.origin[i]<aabb.min[i]) )	{
				cosine = (-aray.origin[i]+aabb.min[i])/aray.direction[i];
				cut[i]=aabb.min[i]; 
				cut[incm3(i)]=aray.origin[incm3(i)]+(aray.direction[incm3(i)]*cosine); 
				cut[decm3(i)]=aray.origin[decm3(i)]+(aray.direction[decm3(i)]*cosine);
				if ((cut[incm3(i)]>aabb.min[incm3(i)]) && (cut[incm3(i)]<aabb.max[incm3(i)]) && (cut[decm3(i)]>aabb.min[decm3(i)]) && (cut[decm3(i)]<aabb.max[decm3(i)])) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
			if ( (aray.direction[i]<0) && (aray.origin[i]>aabb.max[i]) )	{
				cosine = (+aray.origin[i]-aabb.max[i])/aray.direction[i];
				cut[i]=aabb.max[i]; 
				cut[incm3(i)]=aray.origin[incm3(i)]-(aray.direction[incm3(i)]*cosine); 
				cut[decm3(i)]=aray.origin[decm3(i)]-(aray.direction[decm3(i)]*cosine);
				if ((cut[incm3(i)]>aabb.min[incm3(i)]) && (cut[incm3(i)]<aabb.max[incm3(i)]) && (cut[decm3(i)]>aabb.min[decm3(i)]) && (cut[decm3(i)]<aabb.max[decm3(i)])) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
		}
		return 0x00;//no intersection
	}

	//----------------------------------------------------------------------------------
	//  Lineseg_AABB
	//
	//	just ONE intersection point is calculated, and thats the entry point           - 
	//   Lineseg and AABB are assumed to be in the same space
	//
	//--- 0x00 = no intersection (output undefined)        --------------------------
	//--- 0x01 = intersection (intersection point in output)              --------------
	//--- 0x02 = start of Lineseg is inside the AABB (ls.start is output) 
	//----------------------------------------------------------------------------------
	inline uint8	Lineseg_AABB( const Lineseg& ls,const AABB& aabb, Vec3& output1 ) {

		uint8 cflags;
		float cosine;
		Vec3 cut;
		Vec3 lnormal=(ls.start-ls.end).GetNormalized();
		//--------------------------------------------------------------------------------------
		//----         check if "ls.start" is inside of AABB    ---------------------------
		//--------------------------------------------------------------------------------------
		cflags =(ls.start.x>aabb.min.x)<<0;
		cflags|=(ls.start.x<aabb.max.x)<<1;
		cflags|=(ls.start.y>aabb.min.y)<<2;
		cflags|=(ls.start.y<aabb.max.y)<<3;
		cflags|=(ls.start.z>aabb.min.z)<<4;
		cflags|=(ls.start.z<aabb.max.z)<<5;
		if (cflags==0x3f)	{ 
			//ls.start is inside of aabb
			output1=ls.start;	return 0x02; 
		}

		//--------------------------------------------------------------------------------------
		//----         check intersection with x-planes           ------------------------------
		//--------------------------------------------------------------------------------------
		if (lnormal.x) {
			if ( (ls.start.x<aabb.min.x) && (ls.end.x>aabb.min.x) ) {
				cosine = (-ls.start.x+(+aabb.min.x))/lnormal.x;
				cut(aabb.min.x, ls.start.y+(lnormal.y*cosine), ls.start.z+(lnormal.z*cosine));
				//check if cut-point is inside YZ-plane border
				if ((cut.y>aabb.min.y) && (cut.y<aabb.max.y) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=cut;	return 0x01;
				}
			}
			if ( (ls.start.x>aabb.max.x) && (ls.end.x<aabb.max.x) ) {
				cosine = (+ls.start.x+(-aabb.max.x))/lnormal.x;
				cut( aabb.max.x, ls.start.y-(lnormal.y*cosine), ls.start.z-(lnormal.z*cosine));
				//check if cut-point is inside YZ-plane border
				if ((cut.y>aabb.min.y) && (cut.y<aabb.max.y) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=cut;	return 0x01;
				}
			}
		}
		//--------------------------------------------------------------------------------------
		//----         check intersection with z-planes           ------------------------------
		//--------------------------------------------------------------------------------------
		if (lnormal.z) {
			if ( (ls.start.z<aabb.min.z) && (ls.end.z>aabb.min.z) ) {
				cosine = (-ls.start.z+(+aabb.min.z))/lnormal.z;
				cut( ls.start.x+(lnormal.x*cosine), ls.start.y+(lnormal.y*cosine), aabb.min.z);
				//check if cut-point is inside XY-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.y>aabb.min.y) && (cut.y<aabb.max.y)) {
					output1=cut;	return 0x01;
				}
			}
			if ( (ls.start.z>aabb.max.z) && (ls.end.z<aabb.max.z) ) {
				cosine = (+ls.start.z+(-aabb.max.z))/lnormal.z;
				cut( ls.start.x-(lnormal.x*cosine), ls.start.y-(lnormal.y*cosine), aabb.max.z);
				//check if cut-point is inside XY-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.y>aabb.min.y) && (cut.y<aabb.max.y)) {
					output1=cut;	return 0x01;
				}
			}
		}
		//--------------------------------------------------------------------------------------
		//----         check intersection with y-planes           ------------------------------
		//--------------------------------------------------------------------------------------
		if (lnormal.y) {
			if ( (ls.start.y<aabb.min.y) && (ls.end.y>aabb.min.y) ) {
				cosine = (-ls.start.y+(+aabb.min.y))/lnormal.y;
				cut( ls.start.x+(lnormal.x*cosine), aabb.min.y, ls.start.z+(lnormal.z*cosine));
				//check if cut-point is inside XZ-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=cut;	return 0x01;
				}
			}
			if ( (ls.start.y>aabb.max.y) && (ls.end.y<aabb.max.y) ) {
				cosine = (+ls.start.y+(-aabb.max.y))/lnormal.y;
				cut( ls.start.x-(lnormal.x*cosine), aabb.max.y, ls.start.z-(lnormal.z*cosine) );
				//check if cut-point is inside XZ-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=cut;	return 0x01;
				}
			}
		}
		//no intersection
		return 0x00;
	}



	//----------------------------------------------------------------------------------
	//  Lineseg_OBB
	//
	//	just ONE intersection point is calculated, and thats the entry point           - 
	//  Lineseg and OBB are assumed to be in the same space
	//
	//--- 0x00 = no intersection (output undefined)           --------------------------
	//--- 0x01 = intersection (intersection point in output)              --------------
	//--- 0x02 = start of Lineseg is inside the OBB (ls.start is output) 
	//----------------------------------------------------------------------------------
	inline uint8	Lineseg_OBB( const Lineseg& lseg,const Vec3& pos,const OBB& obb, Vec3& output1 ) {

		AABB aabb(obb.c-obb.h,obb.c+obb.h);
		Lineseg ls((lseg.start-pos)*obb.m33,(lseg.end-pos)*obb.m33);

		uint8 cflags;
		float cosine;
		Vec3 cut;
		Vec3 lnormal=(ls.start-ls.end).GetNormalized();
		//--------------------------------------------------------------------------------------
		//----         check if "ls.start" is inside of AABB    ---------------------------
		//--------------------------------------------------------------------------------------
		cflags =(ls.start.x>aabb.min.x)<<0;
		cflags|=(ls.start.x<aabb.max.x)<<1;
		cflags|=(ls.start.y>aabb.min.y)<<2;
		cflags|=(ls.start.y<aabb.max.y)<<3;
		cflags|=(ls.start.z>aabb.min.z)<<4;
		cflags|=(ls.start.z<aabb.max.z)<<5;
		if (cflags==0x3f)	{ 
			//ls.start is inside of aabb
			output1=obb.m33*ls.start+pos;	return 0x02; 
		}

		//--------------------------------------------------------------------------------------
		//----         check intersection with x-planes           ------------------------------
		//--------------------------------------------------------------------------------------
		if (lnormal.x) {
			if ( (ls.start.x<aabb.min.x) && (ls.end.x>aabb.min.x) ) {
				cosine = (-ls.start.x+(+aabb.min.x))/lnormal.x;
				cut(aabb.min.x,ls.start.y+(lnormal.y*cosine),ls.start.z+(lnormal.z*cosine));
				//check if cut-point is inside YZ-plane border
				if ((cut.y>aabb.min.y) && (cut.y<aabb.max.y) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
			if ( (ls.start.x>aabb.max.x) && (ls.end.x<aabb.max.x) ) {
				cosine = (+ls.start.x+(-aabb.max.x))/lnormal.x;
				cut(aabb.max.x,ls.start.y-(lnormal.y*cosine),ls.start.z-(lnormal.z*cosine));
				//check if cut-point is inside YZ-plane border
				if ((cut.y>aabb.min.y) && (cut.y<aabb.max.y) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
		}
		//--------------------------------------------------------------------------------------
		//----         check intersection with z-planes           ------------------------------
		//--------------------------------------------------------------------------------------
		if (lnormal.z) {
			if ( (ls.start.z<aabb.min.z) && (ls.end.z>aabb.min.z) ) {
				cosine = (-ls.start.z+(+aabb.min.z))/lnormal.z;
				cut(ls.start.x+(lnormal.x*cosine),ls.start.y+(lnormal.y*cosine),aabb.min.z);
				//check if cut-point is inside XY-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.y>aabb.min.y) && (cut.y<aabb.max.y)) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
			if ( (ls.start.z>aabb.max.z) && (ls.end.z<aabb.max.z) ) {
				cosine = (+ls.start.z+(-aabb.max.z))/lnormal.z;
				cut(ls.start.x-(lnormal.x*cosine),ls.start.y-(lnormal.y*cosine),aabb.max.z);
				//check if cut-point is inside XY-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.y>aabb.min.y) && (cut.y<aabb.max.y)) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
		}
		//--------------------------------------------------------------------------------------
		//----         check intersection with y-planes           ------------------------------
		//--------------------------------------------------------------------------------------
		if (lnormal.y) {
			if ( (ls.start.y<aabb.min.y) && (ls.end.y>aabb.min.y) ) {
				cosine = (-ls.start.y+(+aabb.min.y))/lnormal.y;
				cut(ls.start.x+(lnormal.x*cosine), aabb.min.y, ls.start.z+(lnormal.z*cosine));
				//check if cut-point is inside XZ-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
			if ( (ls.start.y>aabb.max.y) && (ls.end.y<aabb.max.y) ) {
				cosine = (+ls.start.y+(-aabb.max.y))/lnormal.y;
				cut(ls.start.x-(lnormal.x*cosine), aabb.max.y, ls.start.z-(lnormal.z*cosine));
				//check if cut-point is inside XZ-plane border
				if ((cut.x>aabb.min.x) && (cut.x<aabb.max.x) && (cut.z>aabb.min.z) && (cut.z<aabb.max.z)) {
					output1=obb.m33*cut+pos;	return 0x01;
				}
			}
		}
		//no intersection
		return 0x00;
	}



	//----------------------------------------------------------------------------------
	//--- 0x00 = no intersection                               --------------------------
	//--- 0x01 = not possible   --
	//--- 0x02 = not possible  --
	//--- 0x03 = two intersection, lineseg has ENTRY and EXIT point  --
	//----------------------------------------------------------------------------------

	inline unsigned char Line_Sphere( const Line &line, const Sphere &s, Vec3 &i0, Vec3 &i1 ) {

		Vec3 end=line.pointonline+line.direction;

		float a = line.direction|line.direction;
		float b = (line.direction|(line.pointonline-s.center))*2.0f;
		float c = ((line.pointonline-s.center)|(line.pointonline-s.center)) - (s.radius*s.radius); 

		float desc = (b*b) - (4 * a *c); 

		unsigned char intersection=0;
		if (desc >= 0.0f)
		{
			float lamba0 = (-b - gk_sqrtf(desc)) / (2.0f * a); 
			//_stprintf(d3dApp.token,"lamba0: %20.12f",lamba0);
			//d3dApp.m_pFont->DrawText( 2, d3dApp.PrintY, D3DCOLOR_ARGB(255,255,255,0), d3dApp.token );	d3dApp.PrintY+=20;
			i0 = line.pointonline + ((end-line.pointonline)*lamba0); 
			intersection=1;

			float lamba1 = (-b + gk_sqrtf(desc))/(2.0f*a); 
			//_stprintf(d3dApp.token,"lamba1: %20.12f",lamba1);
			//d3dApp.m_pFont->DrawText( 2, d3dApp.PrintY, D3DCOLOR_ARGB(255,255,255,0), d3dApp.token );	d3dApp.PrintY+=20;
			i1 = line.pointonline + ((end-line.pointonline)*lamba1); 
			intersection|=2;
		}

		return intersection; 
	}



	//----------------------------------------------------------------------------------
	//--- 0x00 = no intersection                               --------------------------
	//--- 0x01 = not possible   --
	//--- 0x02 = one intersection, lineseg has just an EXIT point but no ENTRY point (ls.start is inside the sphere)  --
	//--- 0x03 = two intersection, lineseg has ENTRY and EXIT point  --
	//----------------------------------------------------------------------------------

	inline unsigned char Ray_Sphere( const Ray &ray, const Sphere &s, Vec3 &i0, Vec3 &i1 ) {
		Vec3 end=ray.origin+ray.direction;
		float a = ray.direction|ray.direction;
		float b = (ray.direction|(ray.origin-s.center))*2.0f;
		float c = ((ray.origin-s.center)|(ray.origin-s.center)) - (s.radius*s.radius); 

		float desc = (b*b) - (4 * a *c); 

		unsigned char intersection=0;
		if (desc >= 0.0f)
		{
			float lamba0 = (-b - gk_sqrtf(desc)) / (2.0f * a); 
			//	_stprintf(d3dApp.token,"lamba0: %20.12f",lamba0);
			//	d3dApp.m_pFont->DrawText( 2, d3dApp.PrintY, D3DCOLOR_ARGB(255,255,255,0), d3dApp.token );	d3dApp.PrintY+=20;
			if (lamba0>0.0f)	{
				i0 = ray.origin + ((end-ray.origin)*lamba0); 
				intersection=1;
			}

			float lamba1 = (-b + gk_sqrtf(desc))/(2.0f*a); 
			//	_stprintf(d3dApp.token,"lamba1: %20.12f",lamba1);
			//	d3dApp.m_pFont->DrawText( 2, d3dApp.PrintY, D3DCOLOR_ARGB(255,255,255,0), d3dApp.token );	d3dApp.PrintY+=20;
			if (lamba1>0.0f)	{
				i1 = ray.origin + ((end-ray.origin)*lamba1); 
				intersection|=2;
			}
		}
		return intersection; 
	}

	inline bool Ray_SphereFirst( const Ray &ray, const Sphere &s, Vec3 &intPoint )
	{
		Vec3 p2;
		unsigned char res = Ray_Sphere( ray,s,intPoint,p2 );
		if (res == 2) {	intPoint = p2; }
		if (res > 1)	return true;
		return false;
	}



	//----------------------------------------------------------------------------------
	//--- 0x00 = no intersection                               --------------------------
	//--- 0x01 = one intersection, lineseg has just an ENTRY point but no EXIT point (ls.end is inside the sphere)  --
	//--- 0x02 = one intersection, lineseg has just an EXIT point but no ENTRY point (ls.start is inside the sphere)  --
	//--- 0x03 = two intersection, lineseg has ENTRY and EXIT point  --
	//----------------------------------------------------------------------------------
	inline unsigned char Lineseg_Sphere( const Lineseg &ls, const Sphere &s, Vec3 &i0, Vec3 &i1 ) {

		Vec3 dir = (ls.end - ls.start);

		float a = dir|dir;
		if (a==0.0f)
			return 0;

		float b = (dir|(ls.start-s.center))*2.0f;
		float c = ((ls.start-s.center)|(ls.start-s.center)) - (s.radius*s.radius); 
		float desc = (b*b) - (4 * a *c); 

		unsigned char intersection=0;
		if (desc >= 0.0f)	
		{
			float lamba0 = (-b - gk_sqrtf(desc)) / (2.0f * a); 
			if (lamba0 >0.0f)	
			{
				i0 = ls.start + ((ls.end-ls.start)*lamba0); 
				//skip, if 1st cutting-point is "in front" of ls.end
				if (((i0-ls.end)|dir)>0) return 0;
				intersection=0x01;
			}

			float lamba1 = (-b + gk_sqrtf(desc)) / (2.0f * a); 
			if ( lamba1 > 0.0f)	
			{
				i1 = ls.start + ((ls.end-ls.start)*lamba1); 
				//skip, if 2nd cutting-point is "in front" of ls.end (=ls.end is inside sphere)
				if (((i1-ls.end)|dir)>0) return intersection;
				intersection|=0x02;
			}
		}
		return intersection; 
	}


	inline bool Lineseg_SphereFirst( const Lineseg& lineseg, const Sphere& s, Vec3& intPoint )
	{
		Vec3 p2;
		uint8 res = Lineseg_Sphere( lineseg,s,intPoint,p2 );
		if (res == 2) {	intPoint = p2; }
		if (res > 1)	return true;
		return false;
	}




}; //CIntersect

#endif
