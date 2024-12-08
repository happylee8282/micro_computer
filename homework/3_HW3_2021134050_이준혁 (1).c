/////////////////////////////////////////////////////////
// ������: HW3  Great Escape(��Ż��)

//��������: ROOM ���� ��й�ȣ(1�ڸ� ����)�� ���� Ż��
//����� �ϵ����(���): GPIO,Joy-stick,LED,SW,Keyscan,EXTI,GLCD,SWITCH
//������: 2024.06.07
//������ Ŭ����: ����Ϲ�
//              �й�: 2021134050
//              �̸�: ������
/////////////////////////////////////////////////////////


#include "stm32f4xx.h"
#include "GLCD.h"

void _GPIO_Init(void);  //gpio �ʱ�ȭ
void _EXTI_Init(void); //EXTI �ʱ�ȭ

void DisplayInitScreen(void); //ȭ�� �ʱ�ȭ
uint16_t KEY_Scan(void); //��ư ��ĵ
uint16_t JOY_Scan(void); //���̽�ƽ
void BEEP(void); //beep 30ms
void BEEP_500(void); //beep 500ms

void DelayMS(unsigned short wMS); //Ms delay
void DelayUS(unsigned short wUS); //Us delay

#define RGB_PINK GET_RGB(255,0,255) //��ũ ����

char get = 'S'; //char ����  LCD ȭ�鿡 ���Ⱚ ����

int main(void)
{
	_GPIO_Init(); 		// gpio �ʱ�ȭ
	LCD_Init();		// LCD ��� �ʱ�ȭ
	DelayMS(100);
	_EXTI_Init();	// EXTI �ʱ�ȭ
	DisplayInitScreen();	// LCD �ʱ�ȭ��
	GPIOG->ODR &= ~0x00FF;	// LED �ʱⰪ: LED0~7 Off
        while(1)//while�� ������ ��
        {
         }
    
}

//gpio_init ����
void _GPIO_Init(void) 
{
	// LED (GPIO G) ���� : Output mode
	RCC->AHB1ENR	|=  0x00000040;	// RCC_AHB1ENR : GPIOG(bit#6) Enable							
	GPIOG->MODER 	&=  ~0x0000FFFF;	// GPIOG 0~7 : Clear (0b00)						
	GPIOG->MODER 	|=  0x00005555;	// GPIOG 0~7 : Output mode (0b01)						

	GPIOG->OTYPER	&= ~0x00FF;	// GPIOG 0~7 : Push-pull  (GP8~15:reset state)	

	GPIOG->OSPEEDR &= ~0x0000FFFF;	// GPIOG 0~7 : Clear (0b00) 
	GPIOG->OSPEEDR 	|=  0x00005555;	// GPIOG 0~7 : Output speed 25MHZ Medium speed 
   
	// SW (GPIO H) ���� : Input mode 
	RCC->AHB1ENR    |=  0x00000080;	// RCC_AHB1ENR : GPIOH(bit#7) Enable							
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH 8~15 : Input mode (reset state)				
	GPIOH->PUPDR 	&= ~0xFFFF0000;	// GPIOH 8~15 : Floating input (No Pull-up, pull-down) :reset state

	// Buzzer (GPIO F) ���� : Output mode
	RCC->AHB1ENR	|=  0x00000020;	// RCC_AHB1ENR : GPIOF(bit#5) Enable							

	GPIOF->MODER 	&= ~0x000C0000;	// GPIOF 9 : Clear (0b00)						
	GPIOF->MODER 	|=  0x00040000;	// GPIOF 9 : Output mode (0b01)						

	GPIOF->OTYPER 	&= ~0x0200;	// GPIOF 9 : Push-pull  	

	GPIOF->OSPEEDR 	&= ~0x000C0000;	// GPIOF 9 : Clear (0b00) 
	GPIOF->OSPEEDR 	|=  0x00040000;	// GPIOF 9 : Output speed 25MHZ Medium speed 

	//Joy Stick SW(PORT I) ����
	RCC->AHB1ENR	|= 0x00000100;	// RCC_AHB1ENR GPIOI Enable
	GPIOI->MODER	&=~0x0000C000;	// GPIOI 5~9 : Input mode (reset state)

}

void _EXTI_Init(void)
{
    	RCC->AHB1ENR 	|= 0x00000080;	// RCC_AHB1ENR GPIOH Enable
	RCC->AHB1ENR	|= 0x00000100;	// RCC_AHB2NR  Enable
	RCC->APB2ENR 	|= 0x00004000;	// Enable System Configuration Controller Clock
	
	GPIOH->MODER 	&= ~0xFFFF0000;	// GPIOH PIN8~PIN15 Input mode (reset state)				 
	GPIOI->MODER	&=~0x0000C000;	// GPIOI 5~9 : Input mode


	SYSCFG->EXTICR[1] &= ~ 0x00FF; 	// EXTI[1] �ʱ�
	SYSCFG->EXTICR[1] |= 0x8000; 	// EXTI7 �� ���� �ҽ� �Է��� GPIO��

	SYSCFG->EXTICR[2] &= ~ 0x00FF; 	// EXTI[2] �ʱ�
	SYSCFG->EXTICR[2] |= 0x7077; 	// EXTI8,9,11 �� ���� �ҽ� �Է��� GPIO��

	SYSCFG->EXTICR[3] &= ~ 0x00FF; 	// EXTI[3] �ʱ�
	SYSCFG->EXTICR[3] |= 0x7777; 	// EXTI 12,13,14,15 �� ���� �ҽ� �Է��� GPIO��

	EXTI->FTSR |= 0x0080;		// EXTI7: Falling Trigger Enable
	EXTI->FTSR |= 0x0100;		// EXTI8: Falling Trigger Enable
	EXTI->FTSR |= 0x0200;		// EXTI9: Falling Trigger Enable
	EXTI->FTSR |= 0x0800;		// EXTI11: Falling Trigger Enable
	EXTI->FTSR |= 0x1000;		// EXTI12: Falling Trigger Enable
	EXTI->FTSR |= 0x2000;		// EXTI13: Falling Trigger Enable
	EXTI->FTSR |= 0x4000;		// EXTI14: Falling Trigger Enable
	EXTI->FTSR |= 0x8000;		// EXTI15: Falling Trigger Enable

    	EXTI->IMR  |= 0x000100;  	// EXTI8���ͷ�Ʈ mask (Interrupt Enable) ����
	
	NVIC->ISER[0] |= (1 << 23);	// Enable 'Global Interrupt 

	NVIC->ISER[1] |= (1 << (40-32));	// Enable 'Global Interrupt 
}

void EXTI9_5_IRQHandler(void)		// handler value(5 ~ 9)
{       
        LCD_SetBrushColor(RGB_PINK); //��ü pink ��ĥ
	if(EXTI->PR & 0x0100)                   // EXTI8 Interrupt Pending(�߻�) ����
	{
               BEEP(); //beep��
		EXTI->PR |= 0x0100; 		// EXTI8 Interrupt Pending(�߻�) ����
                LCD_DrawFillRect(18, 26, 4, 4);//��ȫ�� �簢�� ä��°�

                EXTI->IMR  &= ~0x0100;  	// EXTI8 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x000200;  	// EXTI9���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0001; //LED 0 on
              if(get=='S') // LCD �� S���� W�� �ٲ�
              {
                 get='W';
                 LCD_DisplayText(2, 18,"W");	
              }      
	}


	else if(EXTI->PR & 0x0200)                   // EXTI9 Interrupt Pending(�߻�) ����
	{
               BEEP();
		EXTI->PR |= 0x0200; 		 // EXTI9 Interrupt Pending(�߻�) ����
                LCD_DrawFillRect(34, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x200;  	// EXTI9 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x00080;  	// EXTI7���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0002;
	}

    	else if(EXTI->PR & 0x0080)			// EXTI7 Interrupt Pending(�߻�) ����
	{
               BEEP();
		EXTI->PR |= 0x0080;		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
                LCD_DrawFillRect(50, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x0080;   	// EXTI7 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x0800;  	// EXTI11���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0004; //led ��Ŵ
	}
}

void EXTI15_10_IRQHandler(void) //EXTI 10 ~ 15���
{
        LCD_SetBrushColor(RGB_PINK); //pink ����
    	if(EXTI->PR & 0x0800)			// EXTI11 Interrupt Pending(�߻�) ����
	{
               BEEP();
		EXTI->PR |= 0x0800;		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
                LCD_DrawFillRect(66, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x800; 	// EXTI11 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x1000;  	// EXTI12���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0008; //led on
	}
	//EXTI Example(2) : SW1�� Low���� High�� �� ��(Rising edge Trigger mode) EXTI9 ���ͷ�Ʈ �߻�, LED1 toggle
	else if(EXTI->PR & 0x1000)		// EXTI12 Interrupt Pending(�߻�) ����
	{        
               BEEP();                                                     
		EXTI->PR |= 0x1000;		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
                LCD_DrawFillRect(82, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x1000; 	// EXTI12 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x2000;  	// EXTI13���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0010; //led ��Ŵ
	}
	else if(EXTI->PR & 0x2000)		// EXTI13 Interrupt Pending(�߻�) ����
	{                 
               BEEP();                                                     
		EXTI->PR |= 0x2000;		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
                LCD_DrawFillRect(98, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x2000; 	// EXTI13 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x4000;  	// EXTI14���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0020; //led ��
        }
	else if(EXTI->PR & 0x4000)		// EXTI14 Interrupt Pending(�߻�) ����?
	{                 
               BEEP();                                                     
		EXTI->PR |= 0x4000;		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
                LCD_DrawFillRect(114, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x4000; 	// EXTI14 Interrupt Pending(�߻�) �ʱ�ȭ
                EXTI->IMR  |= 0x8000;  	// EXTI15���ͷ�Ʈ mask (Interrupt Enable) ����
                GPIOG->ODR |= 0x0040; //led ��Ŵ
        }
	else if(EXTI->PR & 0x8000)		// EXTI15 Interrupt Pending(�߻�) ����?
	{                 
               BEEP();                                                     
		EXTI->PR |= 0x8000;		// Pending bit Clear (clear�� ���ϸ� ���ͷ�Ʈ ������ �ٽ� ���ͷ�Ʈ �߻�)
                LCD_DrawFillRect(130, 26, 4, 4);//��ȫ�� �簢�� ä��°�
                EXTI->IMR  &= ~0x8000; 	// EXTI15 Interrupt Pending(�߻�) �ʱ�ȭ
                GPIOG->ODR |= 0x0080; //led ��
              if(get=='W')
              {
                 get='C';
                 LCD_DisplayText(2, 18,"C");	// ���� �ٲ�
                 DelayMS(1000);
                BEEP(); 
                 DelayMS(500);
                BEEP(); 
                 DelayMS(500);
                BEEP(); 
                 DelayMS(3000);
                BEEP_500(); //beep 500ms
                 LCD_SetBrushColor(RGB_WHITE); //��ȫ�� ���� -> �Ͼ��
                LCD_DrawFillRect(18, 26, 4, 4);  //����
                LCD_DrawFillRect(34, 26, 4, 4);
                LCD_DrawFillRect(50, 26, 4, 4);
                LCD_DrawFillRect(66, 26, 4, 4);
                LCD_DrawFillRect(82, 26, 4, 4);
                LCD_DrawFillRect(98, 26, 4, 4);
                LCD_DrawFillRect(114, 26, 4, 4);
                LCD_DrawFillRect(130, 26, 4, 4);
                 get='S';                //Get�� S��
                 LCD_DisplayText(2, 18,"S");	// Title���� S�� 
                GPIOG->ODR &= ~0x00FF;	// LED �ʱⰪ: LED0~7 Off
                 EXTI->IMR  |= 0x000100;  	// EXTI8���ͷ�Ʈ mask (Interrupt Enable) ����
              }
        }
}



/* Switch�� �ԷµǾ����� ���ο� � switch�� �ԷµǾ������� ������ return�ϴ� �Լ�  */ 
uint8_t key_flag = 0;
uint16_t KEY_Scan(void)	// input key SW0 - SW7 
{ 
	uint16_t key;
	key = GPIOH->IDR & 0xFF00;	// any key pressed ?
	if(key == 0xFF00)		// if no key, check key off
	{  	if(key_flag == 0)
        		return key;
      		else
		{	DelayMS(10);
        		key_flag = 0;
        		return key;
        	}
    	}
  	else				// if key input, check continuous key
	{	if(key_flag != 0)	// if continuous key, treat as no key input
        		return 0xFF00;
      		else			// if new key,delay for debounce
		{	key_flag = 1;
			DelayMS(10);
 			return key;
        	}
	}
}

uint8_t joy_flag = 0;
uint16_t JOY_Scan(void)	// input joy stick NAVI_* 
{ 
	uint16_t key;
	key = GPIOI->IDR & 0x03E0;	// any key pressed ?
	if(key == 0x03E0)		// if no key, check key off
	{	if(joy_flag == 0)
			return key;
		else
		{	DelayMS(10);
			joy_flag = 0;
			return key;
		}
	}
	else				// if key input, check continuous key
	{	if(joy_flag != 0)	// if continuous key, treat as no key input
			return 0x03E0;
		else			// if new key,delay for debounce
		{	joy_flag = 1;
			DelayMS(10);
			return key;
		}
	}
}


void BEEP(void)			
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(30);		// Delay 30 ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void BEEP_500(void) //500m �Լ�
{ 	
	GPIOF->ODR |=  0x0200;	// PF9 'H' Buzzer on
	DelayMS(500);		// Delay 500ms
	GPIOF->ODR &= ~0x0200;	// PF9 'L' Buzzer off
}

void DelayMS(unsigned short wMS)
{
	register unsigned short i;
	for (i=0; i<wMS; i++)
		DelayUS(1000);	// 1000us => 1ms
}

void DelayUS(unsigned short wUS)
{
	volatile int Dly = (int)wUS*17;
	for(; Dly; Dly--);
}

void DisplayInitScreen(void)
{
	LCD_Clear(RGB_WHITE);		// ȭ�� Ŭ����
	LCD_SetFont(&Gulim8);		// ��Ʈ : ���� 8 & : ������ ǥ�� Ȱ��
	LCD_SetBackColor(RGB_WHITE); //ȭ�� ���
        LCD_SetTextColor(RGB_BLUE);	// ���ڹ��� : Blue
	LCD_DisplayText(0,0,"2021134050 LJH");
	LCD_SetTextColor(RGB_BLACK);	// ���ڻ� : Blackg
	LCD_DisplayText(1,0,"R 0 1 2 3 4 5 6 7");	// Title two
        LCD_SetPenColor(RGB_BLACK); //����Ʋ��
        LCD_DrawRectangle(17, 25, 5, 5); //���� �׸� �׸���
        LCD_DrawRectangle(33, 25, 5, 5);
        LCD_DrawRectangle(49, 25, 5, 5);
        LCD_DrawRectangle(65, 25, 5, 5);
        LCD_DrawRectangle(81, 25, 5, 5);
        LCD_DrawRectangle(97, 25, 5, 5);
        LCD_DrawRectangle(113, 25, 5, 5);
        LCD_DrawRectangle(129, 25, 5, 5);
         LCD_SetBrushColor(RGB_WHITE); //�Ͼ��
        LCD_DrawFillRect(18, 26, 4, 4);  //����
        LCD_DrawFillRect(34, 26, 4, 4);
        LCD_DrawFillRect(50, 26, 4, 4);
        LCD_DrawFillRect(66, 26, 4, 4);
        LCD_DrawFillRect(82, 26, 4, 4);
        LCD_DrawFillRect(98, 26, 4, 4);
        LCD_DrawFillRect(114, 26, 4, 4);
        LCD_DrawFillRect(130, 26, 4, 4);
        LCD_DisplayText(2, 18,"S");	// text S ����
        
}