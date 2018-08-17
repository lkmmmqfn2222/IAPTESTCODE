#include "sys.h"



void NVIC_Configuration(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}
void WFI_SET(void)
{
	__asm__ ("WFI");
}
//�ر������ж�
void INTX_DISABLE(void)
{
	__asm__ ("CPSID I");
}
//���������ж�
void INTX_ENABLE(void)
{
	__asm__ ("CPSIE I");
}
//����ջ����ַ
//addr:ջ����ַ
void MSR_MSP(u32 addr)
{
	__asm__("MSR MSP, r0");		//set Main Stack value
	__asm__("BX r14");
}
