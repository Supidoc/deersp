/*
 * dsp.c
 *
 *  Created on: May 22, 2025
 *      Author: dgrob
 */

#include "dsp.h"
#include "audio_status.h"

#define NUM_TAPS              32

typedef struct _DSP_filterStates
{
	arm_fir_instance_q15 firInstanceQ15_l;
	arm_fir_instance_q15 firInstanceQ15_r;
	q15_t firStateQ15_l[AUDIO_BUFFER_LENGTH_HALF + NUM_TAPS - 1];
	q15_t firStateQ15_r[AUDIO_BUFFER_LENGTH_HALF + NUM_TAPS - 1];
} DSP_filterStates_t;

static DSP_filterStates_t DSP_filters[AUDIO_STEREO_CHANNEL_COUNT];

arm_fir_instance_q15 firInstanceQ15_ch1_l;
arm_fir_instance_q15 firInstanceQ15_ch1_r;

const q15_t firCoeffsQ15[NUM_TAPS] =
{ 0, -3, -8, 21, 70, 0, -221, -222, 345, 842, 0, -1848, -1737, 2851, 9745,
		13100, 9745, 2851, -1737, -1848, 0, 842, 345, -222, -221, 0, 70, 21, -8,
		-3, 0, 0 };

arm_status DSP_init(void)
{
	// Initialize DSP related components here
	arm_status filterStatus;
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{
		filterStatus = arm_fir_init_q15(&DSP_filters[i].firInstanceQ15_l,
		NUM_TAPS, firCoeffsQ15, DSP_filters[i].firStateQ15_l,
		AUDIO_BUFFER_LENGTH_HALF);

		if (filterStatus != ARM_MATH_SUCCESS)
		{
			return filterStatus;
		}

		filterStatus = arm_fir_init_q15(&DSP_filters[i].firInstanceQ15_r,
		NUM_TAPS, firCoeffsQ15, DSP_filters[i].firStateQ15_r,
		AUDIO_BUFFER_LENGTH_HALF);
		if (filterStatus != ARM_MATH_SUCCESS)
		{
			return filterStatus;
		}
	}
	return filterStatus;
}

void DSP_process(void)
{
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{
		if (READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_PENDING))
		{

			arm_fir_q15(&DSP_filters[i].firInstanceQ15_l,
					(q15_t*) ABUF_audioBufferPtrs[i].leftRxFirstHalf,
					(q15_t*) ABUF_audioBufferPtrs[i].leftTxFirstHalf,
					AUDIO_BUFFER_LENGTH_HALF);

			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_PENDING);
			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_CPLT);
		}
		else if (READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_PENDING))
		{

			arm_fir_q15(&DSP_filters[i].firInstanceQ15_r,
					(q15_t*) ABUF_audioBufferPtrs[i].rightRxFirstHalf,
					(q15_t*) ABUF_audioBufferPtrs[i].rightTxFirstHalf,
					AUDIO_BUFFER_LENGTH_HALF);

			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_PENDING);
			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_CPLT);
		}
		else if (READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_PENDING))
		{

			arm_fir_q15(&DSP_filters[i].firInstanceQ15_l,
					(q15_t*) ABUF_audioBufferPtrs[i].leftRxSecHalf,
					(q15_t*) ABUF_audioBufferPtrs[i].leftTxSecHalf,
					AUDIO_BUFFER_LENGTH_HALF);

			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_PENDING);
			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_CPLT);
		}
		else if (READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_PENDING))
		{

			arm_fir_q15(&DSP_filters[i].firInstanceQ15_r,
					(q15_t*) ABUF_audioBufferPtrs[i].rightRxFirstHalf,
					(q15_t*) ABUF_audioBufferPtrs[i].rightTxFirstHalf,
					AUDIO_BUFFER_LENGTH_HALF);

			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_PENDING);
			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_CPLT);
		}
	}
}
