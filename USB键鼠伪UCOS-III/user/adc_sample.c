#include "adc_sample.h"

u32 frequency;
__IO uint16_t ADC1OscConver[320];

extern OS_TCB EmwindemoTaskTCB;
void Adc1Config(void)
{
	NVIC_InitTypeDef        NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);

  	DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA2_Stream0);
    DMA_StructInit( &DMA_InitStructure);
    DMA_InitStructure.DMA_Channel=DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)ADC1OscConver;
    DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize=320;
    DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority=DMA_Priority_Low;
    DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0,&DMA_InitStructure);
	DMA_ClearFlag(DMA2_Stream0,DMA_IT_TC);
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE);

	gpio_init_pin(GPIOA,GPIO_Pin_0,GPIO_Mode_AN,GPIO_OType_PP,GPIO_PuPd_NOPULL);

    ADC_CommonInitTypeDef   ADC_CommonInitStructure;
    //ADC为独立模式
    ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;
    //ADC时钟为APB2的2分频->84/4=21MHZ(F407ADC在2.4-3.6V供电电压下最大速率36M,稳定速度为30M)
    ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div8;
    //使用ADC的DMA复用
    ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_2;
    //两次采样的间隔时间为5个ADC时钟周期(5/21≈0.24us)
    ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;
    //将以上设置应用于对应的寄存器
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitTypeDef ADC_InitStructure;
    //开启ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    //12位转换精度
    ADC_InitStructure.ADC_Resolution=ADC_Resolution_12b;
    //使用单通道转换模式
    ADC_InitStructure.ADC_ScanConvMode=DISABLE;
    //不使用多次转换模式
    ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;
    //使用外部上升沿触发模式
    ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_Rising;
    //TIM2溢出触发
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
    //选择右对齐方式
    ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
    //选用转换通道数为1个
    ADC_InitStructure.ADC_NbrOfConversion=1;
    //将以上设置应用于对应寄存器
    ADC_Init(ADC1,&ADC_InitStructure);
    //使能ADC在DMA模式下的连续转换
    ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);
    //使能ADC的DMA模式
    ADC_DMACmd(ADC1,ENABLE);
    //配置ADC1规则组(得出其单次采样时间为:(3+12)/21≈0.7us)->12bit
    ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_3Cycles);
    //使能ADC
    ADC_Cmd(ADC1,ENABLE);
}

 void Tim2Config(void)
{

    TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);


    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInitStructure.TIM_Prescaler=0;//分频值=84M/(Prescaler+1)

    TIM_TimeBaseInitStructure.TIM_Period=750;//32位

    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);

    TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update);

	TIM_Cmd(TIM2,ENABLE);
}

void DMA2_Stream0_IRQHandler(void)
{
 	OS_ERR err;
 	OSIntEnter();
	DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
	OSTaskSemPost(&EmwindemoTaskTCB,OS_OPT_POST_NONE,&err);
	OSIntExit();
}

void Tim4Config()
{

	gpio_init_pin(GPIOD,GPIO_Pin_12,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_NOPULL);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);

 	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInitStructure.TIM_Prescaler=0;
    TIM_TimeBaseInitStructure.TIM_Period=60000;//16位 

    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);


	TIM_TIxExternalClockConfig(TIM4,TIM_TS_TI1FP1,TIM_ICPolarity_Rising,0);																																

	TIM_SelectOutputTrigger(TIM4,TIM_TRGOSource_Enable);

	TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable); //启动定时器的被动触发

	TIM_SelectOnePulseMode(TIM4,TIM_OPMode_Single);

	//TIM_Cmd(TIM4,ENABLE);


}

void TIM8_BRK_TIM12_IRQHandler()
{
  	static float t;
	OS_ERR err;
	frequency=TIM4->CNT;
	TIM_GenerateEvent(TIM4,TIM_EventSource_Update);//强制TIM4更新并停止
	OSIntEnter();
	TIM_ClearITPendingBit(TIM12,TIM_IT_Update); 
	
	if(frequency==0) frequency=1000;
	t=3.0/(float)frequency/320/(CPU_PERIOD*2)*1000000;

	TIM2->ARR=(u32)t;
	TIM_GenerateEvent(TIM2,TIM_EventSource_Update);	//更新TIM2
	OSTaskSemPost(&EmwindemoTaskTCB,OS_OPT_POST_NONE,&err);
	OSIntExit();
}

void Tim12Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInitStructure.TIM_Prescaler=8400-1;//分频值=84M/(Prescaler+1)=0.01Mhz T=100us

    TIM_TimeBaseInitStructure.TIM_Period=10000;//16位 T=1s

    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM12,&TIM_TimeBaseInitStructure);

	TIM_SelectInputTrigger(TIM12,TIM_TS_ITR0);

	TIM_SelectSlaveMode(TIM12,TIM_SlaveMode_Trigger);

	TIM_ClearITPendingBit(TIM12,TIM_IT_Update);  //清除中断标志位

	TIM_ITConfig(TIM12,TIM_IT_Update,ENABLE);

	TIM_SelectOnePulseMode(TIM12,TIM_OPMode_Single);

	TIM_Cmd(TIM12,ENABLE);
	TIM_ClearITPendingBit(TIM12,TIM_IT_Update);  //清除中断标志位
}

void StartSample()
{
	DMA2_Stream0->NDTR=320;
	DMA_Cmd(DMA2_Stream0, ENABLE);
	ADC_ClearFlag(ADC1,ADC_FLAG_OVR);
}