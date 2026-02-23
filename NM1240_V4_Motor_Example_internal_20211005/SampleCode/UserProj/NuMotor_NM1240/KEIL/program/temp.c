//========Detect_Initial_Hall_Position_M0()===============================
// Detect the initial Hall Postion.
// Set the corresponding values of MotorA.info.i16_hall_angle and Hall_Accumulated_Theda_Q26.
//========================================================================

void Detect_Initial_Hall_Position_M0(void)
{
	INT16 	Initial_Shift_Angle;
	INT32 	Hall_Accumulated_Theda_Q26;
	UINT8	Hall_Position, Last_Hall_Position, Forward_Direction_CMD_M0;
	
	Hall_Position = (HALL_PORT_M0 & 0x07);	// Reserve PB2~PB0
	Last_Hall_Position = Hall_Position;
	Forward_Direction_CMD_M0 = MotorA.cmd.u8_direction;
	
	Initial_Shift_Angle = 1024*0/360;

	if(Hall_Position != HALL_NULL1_M0 && Hall_Position != HALL_NULL2_M0)
	{
		Hall_Accumulated_Theda_Q26 = MotorA.other.i32_Hall_Accumulated_Theda_Q26;
		
		if(Forward_Direction_CMD_M0 == 1)			//For Forward running
		{
			switch(Hall_Position)
			{
				case HALL_STATE1_FW_M0: 		// prepare to turn on CAP1 Falling for Hall_V
					MotorA.info.i16_hall_angle = HALL_STATE1_F_ANGLE_M0 + Initial_Shift_Angle;	//Update new info.hall_angle
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR0.BIT27 = 0;		//Disable Reload function, 20180523 dont need it				
					
				break;

				case HALL_STATE2_FW_M0:		//1
					MotorA.info.i16_hall_angle = HALL_STATE2_F_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;
									
				break;

				case HALL_STATE3_FW_M0:		//5
					MotorA.info.i16_hall_angle = HALL_STATE3_F_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR1.BIT1 = 0;		//Set IC0 as FALLING trigger for next IC00
					//bCAP0_CTR1.BIT0 = 1;
					/* 20180523: dont need it */
					//bCAP0_CTR0.BIT27 = 1;		//Enable Reload function for the next IC00 	reload "CAP0_CNTCMP = C_CAP0_Reload_Value";
					
				break;

				case HALL_STATE4_FW_M0:		//4
					MotorA.info.i16_hall_angle = HALL_STATE4_F_ANGLE_M0 + Initial_Shift_Angle;	//Update new info.hall_angle
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR0.BIT27 = 0;		//Disable Reload function, 20180523: dont need it
									
				break;

				case HALL_STATE5_FW_M0:		//6
					MotorA.info.i16_hall_angle = HALL_STATE5_F_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;
									
				break;

				case HALL_STATE6_FW_M0:		//2
					MotorA.info.i16_hall_angle = HALL_STATE6_F_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR1.BIT1 = 0;		//Set IC0 as RISING trigger for next IC00
					//bCAP0_CTR1.BIT0 = 0;
					/* 20180523: dont need it */
					//bCAP0_CTR0.BIT27 = 1;		//Enable Reload function for the next IC00 	reload "CAP0_CNTCMP = C_CAP0_Reload_Value";				
					
				break;

				default:
					Stop_Motor_PWMOUT_OFF();
				
				break;
			}//---end of switch(Hall_Position)
		}//--- end of "if(Forward_Direction_CMD_M0 == 1)	//For Forward running"

		else //if(Forward_Direction_CMD_M0 == 0)			//For Rewind running
		{
			switch(Hall_Position)
			{
				case HALL_STATE1_RW_M0: 		// 
					MotorA.info.i16_hall_angle = HALL_STATE1_R_ANGLE_M0 + Initial_Shift_Angle;	//Update new info.hall_angle
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR0.BIT27 = 0;		//Disable Reload function 				
					
				break;

				case HALL_STATE2_RW_M0:		//
					MotorA.info.i16_hall_angle = HALL_STATE2_R_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;
									
				break;

				case HALL_STATE3_RW_M0:		//
					MotorA.info.i16_hall_angle = HALL_STATE3_R_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR1.BIT1 = 0;		//Set IC0 as FALLING trigger for next IC00
					//bCAP0_CTR1.BIT0 = 1;

					//bCAP0_CTR0.BIT27 = 1;		//Enable Reload function for the next IC00 	reload "CAP0_CNTCMP = C_CAP0_Reload_Value";
					
				break;

				case HALL_STATE4_RW_M0:		//
					MotorA.info.i16_hall_angle = HALL_STATE4_R_ANGLE_M0 + Initial_Shift_Angle;	//Update new info.hall_angle
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR0.BIT27 = 0;		//Disable Reload function 
									
				break;

				case HALL_STATE5_RW_M0:		//
					MotorA.info.i16_hall_angle = HALL_STATE5_R_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;
									
				break;

				case HALL_STATE6_RW_M0:		//
					MotorA.info.i16_hall_angle = HALL_STATE6_R_ANGLE_M0 + Initial_Shift_Angle;
					if(MotorA.info.i16_hall_angle > 1023)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle - 1024;
					else if (MotorA.info.i16_hall_angle < 0)
						MotorA.info.i16_hall_angle = MotorA.info.i16_hall_angle + 1024;

					Hall_Accumulated_Theda_Q26 = (signed int) (MotorA.info.i16_hall_angle) << 16;

					//bCAP0_CTR1.BIT1 = 0;		//Set IC0 as RISING trigger for next IC00
					//bCAP0_CTR1.BIT0 = 0;

					//bCAP0_CTR0.BIT27 = 1;		//Enable Reload function for the next IC00 	reload "CAP0_CNTCMP = C_CAP0_Reload_Value";				
					
				break;

				default:
					Stop_Motor_PWMOUT_OFF();
				
				break;
			}//---end of switch(Hall_Position)
		}//--- end of "else //if(Forward_Direction_CMD_M0 == 0)	//For Rewind running"
	
		
		MotorA.other.i32_Hall_Accumulated_Theda_Q26 = Hall_Accumulated_Theda_Q26;
		MotorA.info.u8_Hall_Position = Hall_Position;		
		MotorA.info.u8_Last_Hall_Position = Last_Hall_Position;
	}//--end of if "	if(Hall_Position != HALL_NULL1_M0 && Hall_Position != HALL_NULL2_M0)"

}

//====End of Detect_Initial_Hall_Position_M0()================================================
