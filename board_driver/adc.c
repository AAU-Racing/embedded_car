#include <stm32f4xx_hal.h>

int init_adc() {
    ADC_HandleTypeDef    AdcHandle;
  	AdcHandle.Instance          = ADCx;

  	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK) {
	  return -1;
	}

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;          /* Asynchronous clock mode, input ADC clock not divided */
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;             /* 12-bit resolution for converted data */
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
	AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.EOCSelection          = DISABLE;           			/* EOC flag picked-up to indicate conversion end */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
	AdcHandle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.NbrOfDiscConversion   = 0;                             /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;            /* Software start to trig the 1st conversion manually, without external event */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
	AdcHandle.Init.DMAContinuousRequests = DISABLE;                       /* DMA one-shot mode selected (not applied to this example) */


	  if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
		  return -1;
	  }

	  sConfig.Channel      = ADCx_CHANNEL;                /* Sampled channel number */
	  sConfig.Rank         = 1;          /* Rank of sampled channel number ADCx_CHANNEL */
	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;    /* Sampling time (number of clock cycles unit) */
	  sConfig.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */

	  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
	    return -1
	  }

	  if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
		  return -1
	  }
}

void HAL_ADC_MspInit (ADC_HandleTypeDef * hadc) {
	__HAL_RCC_ADC_CLK_ENABLE();
}

void HAL_ADC_DeInit() {
    ADCx_FORCE_RESET();
    ADCx_RELEASE_RESET();

    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);
}

void get_adc_value() {
    HAL_ADC_PollForConversion(&AdcHandle, 10);
 	return HAL_ADC_GetValue(&AdcHandle);
}
