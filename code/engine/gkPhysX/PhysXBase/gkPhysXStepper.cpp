// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
#include "StableHeader.h"
#include "gkPhysXStepper.h"
#include "PxScene.h"

using namespace physx;

void DebugStepper::wait(PxScene* scene)
{
	scene->fetchResults(true, NULL);
	mSimulationTime = (PxReal)mTimer.getElapsedSeconds();
	//mSample->onSubstep(mStepSize);
}

void StepperTask::run()
{
	mStepper->substepDone(this);
	release();
}

void MultiThreadStepper::renderDone()
{
	if(mFirstCompletionPending)
	{
		mCompletion0.removeReference();
		mFirstCompletionPending = false;
	}
}

bool MultiThreadStepper::advance(PxScene* scene, PxReal dt, void* scratchBlock, PxU32 scratchBlockSize)
{
	mScratchBlock = scratchBlock;
	mScratchBlockSize = scratchBlockSize;

	if(!mSync)
		mSync = new shdfnd::Sync;

	substepStrategy(dt, mNbSubSteps, mSubStepSize);
	
	if(mNbSubSteps == 0) return false;

	mScene = scene;

	mSync->reset();

	mCurrentSubStep = 1;

	mCompletion0.setContinuation(*mScene->getTaskManager(), NULL);

	mSimulationTime = 0.0f;
	mTimer.getElapsedSeconds();

	// take first substep
	substep(mCompletion0);	
	mFirstCompletionPending = true;

	return true;
}

void MultiThreadStepper::substepDone(StepperTask* ownerTask)
{
	mScene->fetchResults(true);

	PxReal delta = (PxReal)mTimer.getElapsedSeconds();
	mSimulationTime += delta;

	//mSample->onSubstep(mSubStepSize);

	if(mCurrentSubStep>=mNbSubSteps)
	{
		mSync->set();
	}
	else
	{
		StepperTask &s = ownerTask == &mCompletion0 ? mCompletion1 : mCompletion0;
		s.setContinuation(*mScene->getTaskManager(), NULL);
		mCurrentSubStep++;

		mTimer.getElapsedSeconds();
		substep(s);

		// after the first substep, completions run freely
		s.removeReference();
	}
}

void MultiThreadStepper::substep(StepperTask& completionTask)
{
	// setup any tasks that should run in parallel to simulate()
	//mSample->onSubstepSetup(mSubStepSize, &completionTask);

	// step
	mScene->simulate(mSubStepSize, &completionTask, mScratchBlock, mScratchBlockSize);

	// start any parallel tasks
	//mSample->onSubstepStart(mSubStepSize);
}

void FixedStepper::substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize)
{
	if(mAccumulator > mFixedSubStepSize)
		mAccumulator = 0.0f;

	// don't step less than the step size, just accumulate
	mAccumulator  += stepSize;
	if(mAccumulator < mFixedSubStepSize)
	{
		substepCount = 0;
		return;
	}

	substepSize = mFixedSubStepSize;
	substepCount = PxMin(PxU32(mAccumulator/mFixedSubStepSize), mMaxSubSteps);

	mAccumulator -= PxReal(substepCount)*substepSize;
}

void VariableStepper::substepStrategy(const PxReal stepSize, PxU32& substepCount, PxReal& substepSize)
{
	if(mAccumulator > mMaxSubStepSize)
		mAccumulator = 0.0f;

	// don't step less than the min step size, just accumulate
	mAccumulator  += stepSize;
	if(mAccumulator < mMinSubStepSize)
	{
		substepCount = 0;
		return;
	}

	substepCount = PxMin(PxU32(PxCeil(mAccumulator/mMaxSubStepSize)), mMaxSubSteps);
	substepSize = PxMin(mAccumulator/substepCount, mMaxSubStepSize);

	mAccumulator -= PxReal(substepCount)*substepSize;
}
