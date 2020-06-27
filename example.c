int main(void)
{
  ssd1306_Init();
  
  while (1)
  {
	  ssd1306_SetLine(0, 1);
	  ssd1306_WriteLine("Hi! 2+2*2 = ", Font6x8, White, CLR_BUFFER_SET, DISPLAY_RESET);
	  ssd1306_WriteInt((2+2*2), Font6x8, White, CLR_BUFFER_RESET, DISPLAY_SET);
	  ssd1306_SetLine(20, 7);
	  ssd1306_WriteLine("Lib by Jonssa", Font6x8, Black, CLR_BUFFER_SET, DISPLAY_SET);
    
	  //HAL_Delay(100);
  }
}
