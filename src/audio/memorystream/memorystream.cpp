/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "memorystream.h"

namespace SoLoud
{
	MemoryStreamInstance::MemoryStreamInstance(MemoryStream* aParent)
	{
		mParent = aParent;
		mOffset = 0;
	}

	uint32_t MemoryStreamInstance::getAudio(float* aBuffer, uint32_t aSamplesToRead, uint32_t aBufferSize)
	{
		uint32_t j, k;
		uint32_t future_offset = mOffset + (aSamplesToRead * mChannels), current_offset = mParent->mPushOffset / sizeof(float);

		if (future_offset <= current_offset)
		{
			for (j = 0; j < aSamplesToRead; j++)
			{
				for (k = 0; k < mChannels; k++)
				{
					aBuffer[k * aSamplesToRead + j] = mParent->mData[mOffset + (j * mChannels) + k];
				}
			}

			mOffset += aSamplesToRead * mChannels;

			return aSamplesToRead;
		}

		return 0;
	}

	bool MemoryStreamInstance::hasEnded()
	{
		if (mOffset >= mParent->mSampleCount)
		{
			return 1;
		}
		return 0;
	}

	MemoryStream::MemoryStream(float sampleRate, uint32_t sampleCount, uint32_t channels)
	{
		mBaseSamplerate = sampleRate;
		mSampleCount = sampleCount;
		mChannels = channels;

    mData = new float[mSampleCount]{ NULL };
    mPushLength = mSampleCount * sizeof(float);
    mPushOffset = 0;
	}

	MemoryStream::~MemoryStream()
	{
		stop();

		delete[] mData;
	}

  result MemoryStream::push(uint8_t* data, uint32_t size)
  {
    uint32_t leftSpace = mPushLength - mPushOffset;

    if ( size > leftSpace ) size = leftSpace;

    memcpy((uint8_t*)mData + mPushOffset, data, size);

    mPushOffset += size;

    return SO_NO_ERROR;
  }

	AudioSourceInstance* MemoryStream::createInstance()
	{
		return new MemoryStreamInstance(this);
	}

	double MemoryStream::getLength()
	{
		if (mBaseSamplerate == 0)
			return 0;

		return mSampleCount / mBaseSamplerate;
	}
};
