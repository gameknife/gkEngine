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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkPhysXStepper.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/8/13
// Modify:	2012/8/13
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkPhysXStepper_h_
#define _gkPhysXStepper_h_

#include "pxtask/PxTask.h"
#include "PxPhysicsAPI.h"
#include "PsTime.h"
#include "PsSync.h"

namespace physx
{
	
class Stepper
{
	public:
							Stepper() {}
	virtual					~Stepper() {}

	virtual	bool			advance(PxScene* scene, PxReal dt, void* scratchBlock, PxU32 scratchBlockSize)	= 0;
	virtual	void			wait(PxScene* scene)				= 0;
	virtual void			substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize) = 0;
	
	virtual void			setSubStepper(const PxReal stepSize, const PxU32 maxSteps)	{}
	virtual	void			renderDone()							{}
	virtual	void			shutdown()								{}

	PxReal					getSimulationTime()	const				{ return mSimulationTime; }

protected:
	physx::shdfnd::Time		mTimer;
	PxReal					mSimulationTime;
};

class MultiThreadStepper;
class StepperTask : public physx::pxtask::LightCpuTask
{
public:
	void setStepper(MultiThreadStepper* stepper) { mStepper = stepper; }
	const char* getName() const { return "Stepper Task"; }
	void run();
private:
	MultiThreadStepper*	mStepper;
};

class MultiThreadStepper : public Stepper
{
public:
	MultiThreadStepper()
		: mFirstCompletionPending(false)
		, mScene(NULL)
		, mSync(NULL)
		, mCurrentSubStep(0)
		, mNbSubSteps(0)
	{
		mCompletion0.setStepper(this);
		mCompletion1.setStepper(this);
	};

	~MultiThreadStepper()	{}

	virtual bool			advance(PxScene* scene, PxReal dt, void* scratchBlock, PxU32 scratchBlockSize);
	virtual void			substepDone(StepperTask* ownerTask);
	virtual void			renderDone();
	
	// if mNbSubSteps is 0 then the sync will never 
	// be set so waiting would cause a deadlock
	virtual void			wait(PxScene* scene)	{	if(mNbSubSteps && mSync)mSync->wait();	}
	virtual void			shutdown()				{	SAFE_DELETE(mSync);	}
	virtual void			reset() = 0;
	virtual void			substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize) = 0;

private:
	
	void substep(StepperTask& completionTask);

	// we need two completion tasks because when multistepping we can't submit completion0 from the
	// substepDone function which is running inside completion0
	bool			mFirstCompletionPending;
	StepperTask		mCompletion0, mCompletion1;
	PxScene*		mScene;
	shdfnd::Sync*	mSync;

	PxU32			mCurrentSubStep;
	PxU32			mNbSubSteps;
	PxReal			mSubStepSize;
	void*			mScratchBlock;
	PxU32			mScratchBlockSize;
};

class DebugStepper : public Stepper
{
public:
	DebugStepper(const PxReal stepSize) : mStepSize(stepSize) {}
	
	virtual void substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize)
	{
		substepCount = 1;
		substepSize = mStepSize;
	}

	virtual bool advance(PxScene* scene, PxReal dt, void* scratchBlock, PxU32 scratchBlockSize)
	{
		mTimer.getElapsedSeconds();
		scene->simulate(mStepSize, NULL, scratchBlock, scratchBlockSize);
		return true;
	}

	virtual void setSubStepper(const PxReal stepSize, const PxU32 maxSteps)
	{
		mStepSize = stepSize;
	}

	virtual void wait(PxScene* scene);

	PxReal mStepSize;
};

// The way this should be called is:
// bool stepped = advance(dt)
//
// ... reads from the scene graph for rendering
//
// if(stepped) renderDone()
//
// ... anything that doesn't need access to the the physics scene
//
// if(stepped) sFixedStepper.wait()
//
// Note that per-substep callbacks to the sample need to be issued out of here, 
// between fetchResults and simulate

class FixedStepper : public MultiThreadStepper
{
public:
	FixedStepper(const PxReal subStepSize, const PxU32 maxSubSteps)
		: MultiThreadStepper()
		, mAccumulator(0)
		, mFixedSubStepSize(subStepSize)
		, mMaxSubSteps(maxSubSteps)
	{
	}

	virtual void	substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize);
	virtual void	reset() { mAccumulator = 0.0f; }
	
	virtual void	setSubStepper(const PxReal stepSize, const PxU32 maxSteps) { mFixedSubStepSize = stepSize; mMaxSubSteps = maxSteps;}

	PxReal	mAccumulator;
	PxReal	mFixedSubStepSize;
	PxU32	mMaxSubSteps;
};

class VariableStepper : public MultiThreadStepper
{
public:
	VariableStepper(const PxReal minSubStepSize, const PxReal maxSubStepSize, const PxU32 maxSubSteps)
		: MultiThreadStepper()
		, mAccumulator(0)
		, mMinSubStepSize(minSubStepSize)
		, mMaxSubStepSize(maxSubStepSize)
		, mMaxSubSteps(maxSubSteps)
	{
	}

	virtual void	substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize);
	virtual void	reset() { mAccumulator = 0.0f; }

private:
			PxReal	mAccumulator;
	const	PxReal	mMinSubStepSize;
	const	PxReal	mMaxSubStepSize;
	const	PxU32	mMaxSubSteps;
};

}
#endif
