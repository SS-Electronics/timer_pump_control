
/****************************************************
 * 
 * Main Setup call from arduino
 *
 ***************************************************/
void setup()
{
    /*
     * Initialize Drivers 
    */
    func_io_driver_int(&application);
    func_module_int(&application);


    /* Read the set time from the eeprom */
    uint8_t val_low, val_high;
    val_low   = EEPROM.read(EEPROM_ADDRESS);
    val_high  = EEPROM.read(EEPROM_ADDRESS + 1);
    pump_operation_time_min = 0;
    pump_operation_time_min = ( ((val_high<<8) | val_low) & (0x0000FFFF) );


    /* Start in idle mode */
    application.curr_state = STATE_IDLE;
    application.cap_signal = SIGNAL_STOP;

    /*
     * Go to syatem UI update
     */
    func_ui_update(&application);
    
}



/****************************************************
 * 
 * Main loop call from arduino
 *
 ***************************************************/
void loop()
{
    /* 
     * Signal polling  
     */
    io_signal_polling(&application);



    switch(application.curr_state)
    {
        
        /**************************
         State: IDLE
         **************************/

         case STATE_IDLE:
            switch(application.cap_signal)
            {
                case SIGNAL_START:

                     pump_current_time_update   = 0;
                     pump_operation_start_time  = millis();
                     ui_operation_update_time   = millis();
                     
                     /* Run with the current timing setup  */
                     application.curr_state = STATE_RUNNING;
                     application.cap_signal = SIGNAL_STOP;
                     func_ui_update(&application);
                     func_pump_start();
                     
                break;

                case SIGNAL_UP:
                
                break;

                case SIGNAL_DOWN:
                
                break;

                case SIGNAL_OK:
                    /* OK Signal came so go to the timer setup state 
                     but no opeation int that state */
                     
                     application.curr_state = STATE_TIMER_SETUP;
                     application.cap_signal = SIGNAL_STOP;
                     func_ui_update(&application);

                break;

                case SIGNAL_SET:
                
                break;

                default:

                break;
            }
         break;

       /**************************
         State: STATE_RUNNING
         **************************/

         case STATE_RUNNING:
            switch(application.cap_signal)
            {
                case SIGNAL_START:
                        /* If running press start to stop */
                       application.curr_state = STATE_IDLE;
                       application.cap_signal = SIGNAL_STOP;
                       func_ui_update(&application);
                       func_pump_stop();
                break;

                case SIGNAL_UP:
                
                break;

                case SIGNAL_DOWN:
                
                break;

                case SIGNAL_OK:
                
                break;

                case SIGNAL_SET:
                
                break;

                default:
                    /* Stop the timer and stop the pump  min to ms*/
                    if( (millis() - pump_operation_start_time) > (pump_operation_time_min * 60 * 1000) )
                    {
                       application.curr_state = STATE_IDLE;
                       application.cap_signal = SIGNAL_STOP;
                       func_ui_update(&application);
                       func_pump_stop();
                    }
                    else
                    {
                        if( (millis() - ui_operation_update_time) >= 1000)
                        {
                            pump_current_time_update++;
                            func_ui_update(&application);

                            ui_operation_update_time = millis();
                        }
                    }
                    
                break;
            }
         break;

       /**************************
         State: STATE_TIMER_SETUP
         **************************/

         case STATE_TIMER_SETUP:
            switch(application.cap_signal)
            {
                case SIGNAL_START:
                
                break;

                case SIGNAL_UP:

                    /* Upto 20 minutes increment by 1 min */
                    if(pump_operation_time_min < 20)
                    {
                        pump_operation_time_min++;
                    }
                    else /* Increment by 10 min*/
                    {
                        pump_operation_time_min+=10;
                    }
  
                    /* If the settime is mpore thn operation time */
                    if(pump_operation_time_min >= MAX_TIMER_OPERATION_IN_MIN)
                    {
                      pump_operation_time_min = MAX_TIMER_OPERATION_IN_MIN;
                    }
                    
                    application.curr_state = STATE_TIMER_SETUP;
                    application.cap_signal = SIGNAL_STOP;
                    func_ui_update(&application);
                      
                break;

                case SIGNAL_DOWN:
                    /* Upto 20 minutes increment by 1 min */
                    if(pump_operation_time_min <=  20)
                    {
                        pump_operation_time_min--;
                    }
                    else /* Increment by 10 min*/
                    {
                        pump_operation_time_min-=10;
                    }
  
                    /* If the settime is mpore thn operation time */
                    if(pump_operation_time_min <= MIN_TIMER_OPERATION_IN_MIN)
                    {
                      pump_operation_time_min = MIN_TIMER_OPERATION_IN_MIN;
                    }
                    
                    application.curr_state = STATE_TIMER_SETUP;
                    application.cap_signal = SIGNAL_STOP;
                    func_ui_update(&application);
                break;

                case SIGNAL_OK:
    
                break;

                case SIGNAL_SET:
                    /* timer operation done 
                       so set the variable in EEPROM 
                       and transit to IDLE operation */

                    uint8_t val_low, val_high;
                    val_low   = (uint8_t)pump_operation_time_min;
                    val_high  = (uint8_t)(pump_operation_time_min >> 8);
    
                    EEPROM.write(EEPROM_ADDRESS, val_low);
                    EEPROM.write((EEPROM_ADDRESS + 1), val_high);

    
                    application.curr_state = STATE_IDLE;
                    application.cap_signal = SIGNAL_STOP;
                    func_ui_update(&application);
                break;

                default:

                break;
            }
         break; 
    }








    
}
