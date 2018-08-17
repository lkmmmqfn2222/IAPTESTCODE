#include "sys.h"



void NVIC_Configuration(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}
void WFI_SET(void)
{
	__asm__ ("WFI");
}
//关闭所有中断
void INTX_DISABLE(void)
{
	__asm__ ("CPSID I");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__asm__ ("CPSIE I");
}
//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(u32 addr)
{
	__asm__("MSR MSP, r0");		//set Main Stack value
	__asm__("BX r14");
}
